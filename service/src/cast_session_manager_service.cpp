/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
#include "softbus_error_code.h"
#include "hisysevent.h"
#include "permission.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Service");

REGISTER_SYSTEM_ABILITY_BY_ID(CastSessionManagerService, CAST_ENGINE_SA_ID, false);

namespace SessionServer {
constexpr char SESSION_NAME[] = "CastPlusSessionName";
constexpr int ROLE_CLENT = 1;
constexpr int SOFTBUS_OK = 0;

int OnSessionOpened(int sessionId, int result)
{
    CLOGI("OnSessionOpened, session id = %{public}d, result is %{public}d", sessionId, result);
    if (sessionId <= INVALID_ID || result != SOFTBUS_OK) {
        auto device = CastDeviceDataManager::GetInstance().GetDeviceByTransId(sessionId);
        if (device == std::nullopt) {
            CLOGE("device is empty");
            return result;
        }
        ConnectionManager::GetInstance().NotifySessionEvent(device->deviceId, ConnectEvent::DISCONNECT_START);
        return result;
    }
    int role = GetSessionSide(sessionId);
    if (role == ROLE_CLENT) {
        ConnectionManager::GetInstance().OnConsultSessionOpened(sessionId, true);
    } else {
        ConnectionManager::GetInstance().OnConsultSessionOpened(sessionId, false);
    }
    return SOFTBUS_OK;
}

void OnSessionClosed(int sessionId)
{
    CLOGI("OnSessionClosed, session id = %{public}d", sessionId);
    if (sessionId <= INVALID_ID) {
        return;
    }
}

void OnBytesReceived(int sessionId, const void *data, unsigned int dataLen)
{
    CLOGI("OnBytesReceived,session id = %{public}d, len = %{public}u", sessionId, dataLen);
    if (sessionId <= INVALID_ID || data == nullptr || dataLen == 0) {
        return;
    }
    int role = GetSessionSide(sessionId);
    if (role != ROLE_CLENT) {
        ConnectionManager::GetInstance().OnConsultDataReceived(sessionId, data, dataLen);
    }
}

ISessionListener g_SessionListener = {
    OnSessionOpened, OnSessionClosed, OnBytesReceived, nullptr, nullptr, nullptr
};

// true: softbus service is up, and the session server has been created;
// false: softbus service is up, but the session server failed to create;
// nullopt: softbus service is down.
std::optional<bool> WaitSoftBusInit()
{
    constexpr int sleepTime = 50; // uint: ms
    constexpr int retryTimes = 60 * 20; // total 60s
    int ret = SoftBusErrNo::SOFTBUS_TRANS_SESSION_ADDPKG_FAILED;
    int retryTime = 0;
    while (ret == SoftBusErrNo::SOFTBUS_TRANS_SESSION_ADDPKG_FAILED && retryTime < retryTimes) {
        CLOGI("create session server");
        ret = CreateSessionServer(PKG_NAME, SessionServer::SESSION_NAME, &SessionServer::g_SessionListener);
        if (ret == SOFTBUS_OK) {
            return true;
        }
        retryTime++;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }

    if (retryTime == retryTimes) {
        CLOGE("softbus service is down.");
        return std::nullopt;
    }

    return false;
}

bool SetupSessionServer()
{
    int32_t result = SoftBusErrNo::SOFTBUS_ERR;
    int32_t retryTime = 0;
    constexpr int32_t retryTimes = 20;
    while (result != SessionServer::SOFTBUS_OK && retryTime < retryTimes) {
        CLOGI("retry create session server");
        result = CreateSessionServer(PKG_NAME, SessionServer::SESSION_NAME, &SessionServer::g_SessionListener);
        retryTime++;
    }
    if (result != SessionServer::SOFTBUS_OK) {
        CLOGE("CreateSessionServer failed, ret:%d", result);
        return false;
    }

    return true;
}
}

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
    auto result = SessionServer::WaitSoftBusInit();
    if (result == std::nullopt) {
        CastEngineDfx::WriteErrorEvent(SOURCE_CREATE_SESSION_SERVER_FAIL);
        CLOGE("softbus service is down.");
        return;
    }

    if (result) {
        hasServer_ = true;
        return;
    }

    if (!SessionServer::SetupSessionServer()) {
        CastEngineDfx::WriteErrorEvent(SOURCE_CREATE_SESSION_SERVER_FAIL);
        return;
    }
    hasServer_ = true;
}

void CastSessionManagerService::OnStop()
{
    CLOGI("Stop in");
    RemoveSessionServer(PKG_NAME, SessionServer::SESSION_NAME);
}

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
                device.subDeviceType, device.ipAddress, device.channelType,
                CapabilityType::CAST_PLUS, device.networkId, "", 0, nullptr});
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
            return false;
        }
        sptr<ICastSessionImpl> session;
        service->GetCastSession(std::to_string(castSessionId), session);
        if (session == nullptr) {
            CLOGE("Session is null when consultation data comes!");
            return false;
        }
        if (!session->AddDevice(device)) {
            CLOGE("Session addDevice fail");
            return false;
        }
        ConnectionManager::GetInstance().NotifySessionEvent(device.deviceId, ConnectEvent::CONNECT_START);
        return true;
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

    void OnEvent(const std::string &deviceId, EventCode currentEventCode) override
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
        session->OnSessionEvent(deviceId, currentEventCode);
    }

    void GrabDevice(int32_t sessionId) override
    {
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return;
        }

        sptr<ICastSessionImpl> session;
        service->GetCastSession(std::to_string(sessionId), session);
        if (!session) {
            CLOGE("The session is null. Failed to obtain the session.");
            return;
        }
        session->Release();
    }

    int32_t GetSessionProtocolType(int sessionId, ProtocolType &protocolType) override
    {
        CLOGI("GetSessionProtocolType in");
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return CAST_ENGINE_ERROR;
        }
        sptr<ICastSessionImpl> session;
        service->GetCastSession(std::to_string(sessionId), session);
        if (!session) {
            CLOGE("The session is null. Failed to obtain the session.");
            return CAST_ENGINE_ERROR;
        }
        return session->GetSessionProtocolType(protocolType);
    }

    int32_t SetSessionProtocolType(int sessionId, ProtocolType protocolType) override
    {
        CLOGI("SetSessionProtocolType in");
        auto service = service_.promote();
        if (!service) {
            CLOGE("service is null");
            return CAST_ENGINE_ERROR;
        }
        sptr<ICastSessionImpl> session;
        service->GetCastSession(std::to_string(sessionId), session);
        if (!session) {
            CLOGE("The session is null. Failed to obtain the session.");
            return CAST_ENGINE_ERROR;
        }
        session->SetSessionProtocolType(protocolType);
        return CAST_ENGINE_SUCCESS;
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
    SharedRLock lock(mutex_);
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }

    auto session = GetCastSessionInner(sessionId);
    if (session == nullptr) {
        return ERR_SESSION_NOT_EXIST;
    }
    castSession = session;
    return CAST_ENGINE_SUCCESS;
}

sptr<ICastSessionImpl> CastSessionManagerService::GetCastSessionInner(std::string sessionId)
{
    auto innerSessionId = Utils::StringToInt(sessionId);
    auto it = sessionMap_.find(innerSessionId);
    if (it == sessionMap_.end()) {
        CLOGE("No sessionId=%{public}d session.", innerSessionId);
        return nullptr;
    }
    return it->second;
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
    ConnectionManager::GetInstance().UpdateGrabState(true, Utils::StringToInt(sessionId));
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

    ConnectionManager::GetInstance().UpdateGrabState(false, -1);
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

    if (enable) {
        if (ConnectionManager::GetInstance().EnableDiscoverable() &&
            DiscoveryManager::GetInstance().StartAdvertise()) {
            return CAST_ENGINE_SUCCESS;
        }
    } else {
        if (ConnectionManager::GetInstance().DisableDiscoverable() &&
            DiscoveryManager::GetInstance().StopAdvertise()) {
            return CAST_ENGINE_SUCCESS;
        }
    }
    return CAST_ENGINE_ERROR;
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
