/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: cast session manager service class
 * Author: zhangge
 * Create: 2022-06-15
 */

#include "cast_session_manager_service.h"

#include <algorithm>
#include <atomic>

#include <ipc_skeleton.h>
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "cast_engine_dfx.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "cast_session_impl.h"
#include "connection_manager.h"
#include "discovery_manager.h"
#include "hisysevent.h"
#include "permission.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Service");

REGISTER_SYSTEM_ABILITY_BY_ID(CastSessionManagerService, CAST_ENGINE_SA_ID, false);

CastSessionManagerService::CastSessionManagerService(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    CLOGD("construction in");
    myPid_ = getpid();
};

CastSessionManagerService::~CastSessionManagerService()
{
    CLOGD("destruction in");
}

void CastSessionManagerService::OnStart()
{
    bool ret = Publish(this);
    if (!ret) {
        CLOGE("Failed to publish cast session manager service");
        return;
    }

    AddSystemAbilityListener(CAST_ENGINE_SA_ID);
}

void CastSessionManagerService::OnStop() {}

namespace {
using namespace OHOS::DistributedHardware;

class DiscoveryManagerListener : public IDiscoveryManagerListener {
public:
    DiscoveryManagerListener(sptr<CastSessionManagerService> service) : service_(service) {}

    void OnDeviceFound(const std::vector<CastInnerRemoteDevice> &devices) override
    {
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return;
        }

        std::vector<CastRemoteDevice> remoteDevices;
        for (const auto &device : devices) {
            remoteDevices.push_back(CastRemoteDevice{ device.deviceId, device.deviceName, device.deviceType,
                device.subDeviceType, device.ipAddress, device.channelType });
        }
        service->ReportDeviceFound(remoteDevices);
    }

private:
    wptr<CastSessionManagerService> service_;
};

class ConnectionManagerListener : public IConnectionManagerListener {
public:
    ConnectionManagerListener(sptr<CastSessionManagerService> service) : service_(service) {}

    int NotifySessionIsReady() override
    {
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return INVALID_ID;
        }

        sptr<ICastSessionImpl> session;
        service->CreateCastSession({}, session);
        if (session == nullptr) {
            return INVALID_ID;
        }

        service->ReportSessionCreate(session);
        std::string sessionId{};
        session->GetSessionId(sessionId);
        return Utils::StringToInt((sessionId));
    }

    bool NotifyRemoteDeviceIsReady(int castSessionId, const CastInnerRemoteDevice &device) override
    {
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return INVALID_ID;
        }
        sptr<ICastSessionImpl> session;
        service->GetCastSession(std::to_string(castSessionId), session);
        if (session == nullptr) {
            CLOGE("Session is null when consultation data comes!");
            return false;
        }
        return session->AddDevice(device);
    }

    void NotifyDeviceIsOffline(const std::string &deviceId) override
    {
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return;
        }
        service->ReportDeviceOffline(deviceId);
        CLOGD("OnDeviceOffline out");
    }

    void OnError(const std::string &deviceId) override
    {
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return;
        }
        int sessionId = CastDeviceDataManager::GetInstance().GetSessionIdByDeviceId(deviceId);
        if (sessionId == INVALID_ID) {
            CLOGE("The obtained sessionId is null");
            return;
        }
        sptr<ICastSessionImpl> session;
        service->GetCastSession(std::to_string(sessionId), session);
        if (!session) {
            CLOGE("The session is null. Failed to obtain the session.");
            return;
        }
        session->RemoveDevice(deviceId);
    }

private:
    wptr<CastSessionManagerService> service_;
};
} // namespace

int32_t CastSessionManagerService::RegisterListener(sptr<ICastServiceListenerImpl> listener)
{
    CLOGI("RegisterListener in");
    HiSysEventWrite(CAST_ENGINE_DFX_DOMAIN_NAME, "CAST_ENGINE_EVE", HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "SEQUENTIAL_ID", CastEngineDfx::GetSequentialId(), "BIZ_PACKAGE_NAME", CastEngineDfx::GetBizPackageName());
    SharedWLock lock(mutex_);
    if (listener == nullptr) {
        CLOGE("RegisterListener failed, listener is null");
        return CAST_ENGINE_ERROR;
    }
    bool needInitMore = !HasListenerLocked();
    if (!AddListenerLocked(listener)) {
        return CAST_ENGINE_ERROR;
    }

    if (needInitMore) {
        DiscoveryManager::GetInstance().Init(std::make_shared<DiscoveryManagerListener>(this));
        ConnectionManager::GetInstance().Init(std::make_shared<ConnectionManagerListener>(this));
        sessionMap_.clear();
    }

    serviceStatus_ = ServiceStatus::CONNECTED;
    CLOGI("RegisterListener out");
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerService::UnregisterListener()
{
    SharedWLock lock(mutex_);
    CLOGI("UnregisterListener in");

    return RemoveListenerLocked(IPCSkeleton::GetCallingPid());
}

int32_t CastSessionManagerService::Release()
{
    SharedWLock lock(mutex_);
    CLOGI("Release in");
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }

    return ReleaseLocked();
}

int32_t CastSessionManagerService::ReleaseLocked()
{
    CLOGI("ReleaseLocked in");
    serviceStatus_ = ServiceStatus::DISCONNECTED;
    ReportServiceDieLocked();

    ClearListenersLocked();
    DiscoveryManager::GetInstance().Deinit();
    ConnectionManager::GetInstance().Deinit();
    sessionMap_.clear();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        CLOGE("get samgr failed");
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = samgr->UnloadSystemAbility(CAST_ENGINE_SA_ID);
    if (ret != ERR_OK) {
        CLOGE("remove system ability failed");
        return CAST_ENGINE_ERROR;
    }
    CLOGI("Release success done");
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerService::SetLocalDevice(const CastLocalDevice &localDevice)
{
    CLOGD("SetLocalDevice in");
    SharedWLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    localDevice_ = localDevice;
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerService::GetCastSession(std::string sessionId, sptr<ICastSessionImpl> &castSession)
{
    auto innerSessionId = Utils::StringToInt(sessionId);
    SharedRLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }

    auto it = sessionMap_.find(innerSessionId);
    if (it == sessionMap_.end()) {
        CLOGE("No sessionId=%{public}d session.", innerSessionId);
        return ERR_SESSION_NOT_EXIST;
    }

    castSession = it->second;
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerService::CreateCastSession(const CastSessionProperty &property,
    sptr<ICastSessionImpl> &castSession)
{
    SharedWLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    CLOGD("CreateCastSession in, protocol:%{public}d, endType:%{public}d.", property.protocolType, property.endType);
    if (serviceStatus_ != ServiceStatus::CONNECTED) {
        CLOGE("not connected");
        return ERR_SERVICE_STATE_NOT_MATCH;
    }

    if (localDevice_.deviceId.empty()) {
        auto local = ConnectionManager::GetInstance().GetLocalDeviceInfo();
        if (local == nullptr) {
            return CAST_ENGINE_ERROR;
        }
        localDevice_ = *local;
    }

    auto tmp = new (std::nothrow) CastSessionImpl(property, localDevice_);
    if (tmp == nullptr) {
        CLOGE("CastSessionImpl is null");
        return ERR_NO_MEMORY;
    }
    sptr<ICastSessionImpl> session(static_cast<ICastSessionImpl *>(tmp));
    tmp->Init();
    tmp->SetServiceCallbackForRelease([this](int32_t sessionId) { DestroyCastSession(sessionId); });

    sessionIndex_++;
    std::string sessionId{};
    session->GetSessionId(sessionId);
    sessionMap_.insert({ Utils::StringToInt(sessionId), session });

    CLOGD("CreateCastSession success, session(%{public}d) count:%{public}zu",
        Utils::StringToInt(sessionId), sessionMap_.size());
    castSession = session;
    return CAST_ENGINE_SUCCESS;
}

bool CastSessionManagerService::DestroyCastSession(int32_t sessionId)
{
    CLOGD("DestroyCastSession in");
    sptr<ICastSessionImpl> session;
    {
        SharedWLock lock(mutex_);
        auto it = sessionMap_.find(sessionId);
        if (it == sessionMap_.end()) {
            CLOGE("Cast session(%d) has gone.", sessionId);
            return true;
        }
        session = it->second;
        sessionMap_.erase(it);
    }

    session->Stop();
    CLOGD("Session refcount is %d, session count:%zu", session->GetSptrRefCount(), sessionMap_.size());
    return true;
}

int32_t CastSessionManagerService::SetSinkSessionCapacity(int sessionCapacity)
{
    CLOGD("SetSinkSessionCapacity in, sessionCapacity = %d", sessionCapacity);
    SharedRLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    sessionCapacity_ = sessionCapacity;
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerService::StartDiscovery(int protocols)
{
    static_cast<void>(protocols);
    CLOGD("StartDiscovery in, protocolType = %d", protocols);
    SharedRLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    DiscoveryManager::GetInstance().StartDiscovery();
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerService::StopDiscovery()
{
    CLOGD("StopDiscovery in");
    SharedRLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    DiscoveryManager::GetInstance().StopDiscovery();
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerService::SetDiscoverable(bool enable)
{
    CLOGD("SetDiscoverable in, enable = %{public}d", enable);
    SharedRLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    bool ret = false;
    if (enable) {
        ret = ConnectionManager::GetInstance().EnableDiscoverable();
    } else {
        ret = ConnectionManager::GetInstance().DisableDiscoverable();
    }

    return ret ? CAST_ENGINE_SUCCESS : CAST_ENGINE_ERROR;
}

void CastSessionManagerService::ReleaseServiceResources(pid_t pid)
{
    {
        SharedWLock lock(mutex_);
        RemoveListenerLocked(pid);
        for (auto it = sessionMap_.begin(); it != sessionMap_.end();) {
            if (it->second->ReleaseSessionResources(pid)) {
                sessionMap_.erase(it++);
                continue;
            }
            it++;
        }
        if (HasListenerLocked()) {
            return;
        }
    }
    CLOGD("Release service resources");
    if (Release() != CAST_ENGINE_SUCCESS) {
        CLOGE("Release service resources failed");
    }
}

void CastSessionManagerService::AddClientDeathRecipientLocked(pid_t pid, sptr<ICastServiceListenerImpl> listener)
{
    sptr<CastEngineClientDeathRecipient> deathRecipient(
        new (std::nothrow) CastEngineClientDeathRecipient(wptr<CastSessionManagerService>(this), pid));
    if (deathRecipient == nullptr) {
        CLOGE("Alloc death recipient filed");
        return;
    }
    if (!listener->AsObject()->AddDeathRecipient(deathRecipient)) {
        CLOGE("Add cast client death recipient failed");
        return;
    }
    CLOGD("add death recipient pid:%d", pid);
    deathRecipientMap_[pid] = deathRecipient;
}

void CastSessionManagerService::RemoveClientDeathRecipientLocked(pid_t pid, sptr<ICastServiceListenerImpl> listener)
{
    auto it = deathRecipientMap_.find(pid);
    if (it != deathRecipientMap_.end()) {
        listener->AsObject()->RemoveDeathRecipient(it->second);
        deathRecipientMap_.erase(it);
        CLOGD("remove death recipient pid:%d", pid);
    }
}

bool CastSessionManagerService::AddListenerLocked(sptr<ICastServiceListenerImpl> listener)
{
    pid_t pid = IPCSkeleton::GetCallingPid();
    Permission::SavePid(pid);
    if (std::find_if(listeners_.begin(), listeners_.end(),
        [pid](std::pair<pid_t, sptr<ICastServiceListenerImpl>> member) { return member.first == pid; }) ==
        listeners_.end()) {
        listeners_.push_back({ pid, listener });
        AddClientDeathRecipientLocked(pid, listener);
        return true;
    }

    CLOGE("The process(%u) has register the listener", pid);
    return false;
}

int32_t CastSessionManagerService::RemoveListenerLocked(pid_t pid)
{
    Permission::RemovePid(pid);
    auto iter = std::find_if(listeners_.begin(), listeners_.end(),
        [pid](std::pair<pid_t, sptr<ICastServiceListenerImpl>> element) { return element.first == pid; });
    if (iter != listeners_.end()) {
        RemoveClientDeathRecipientLocked(pid, (*iter).second);
        listeners_.erase(iter);
        if (listeners_.size() == 0) {
            ReleaseLocked();
        }
        return CAST_ENGINE_SUCCESS;
    }
    return CAST_ENGINE_ERROR;
}

void CastSessionManagerService::ClearListenersLocked()
{
    listeners_.clear();
    Permission::ClearPids();
}

bool CastSessionManagerService::HasListenerLocked()
{
    return listeners_.size() > 0;
}

void CastSessionManagerService::ReportServiceDieLocked()
{
    pid_t pid = IPCSkeleton::GetCallingPid();
    if (pid == myPid_) {
        for (const auto &listener : listeners_) {
            listener.second->OnServiceDied();
        }
        return;
    }

    auto it = std::find_if(listeners_.begin(), listeners_.end(),
        [pid](std::pair<pid_t, sptr<ICastServiceListenerImpl>> element) { return element.first == pid; });
    if (it != listeners_.end()) {
        it->second->OnServiceDied();
        return;
    }
}

void CastSessionManagerService::ReportDeviceFound(const std::vector<CastRemoteDevice> &deviceList)
{
    SharedRLock lock(mutex_);
    for (const auto &listener : listeners_) {
        listener.second->OnDeviceFound(deviceList);
    }
}

void CastSessionManagerService::ReportSessionCreate(const sptr<ICastSessionImpl> &castSession)
{
    SharedRLock lock(mutex_);
    for (const auto &listener : listeners_) {
        listener.second->OnSessionCreated(castSession);
    }
}

void CastSessionManagerService::ReportDeviceOffline(const std::string &deviceId)
{
    SharedRLock lock(mutex_);
    for (const auto &listener : listeners_) {
        listener.second->OnDeviceOffline(deviceId);
    }
}

void CastSessionManagerService::CastEngineClientDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    CLOGD("Client died, need release resources, client pid_: %d", pid_);
    sptr<CastSessionManagerService> service = service_.promote();
    if (service == nullptr) {
        CLOGE("ServiceStub is nullptr");
        return;
    }
    service->ReleaseServiceResources(pid_);
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
