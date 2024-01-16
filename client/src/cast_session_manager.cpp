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
 * Description: implement the cast session manager
 * Author: zhangge
 * Create: 2022-06-15
 */

#include <thread>

#include "cast_session_manager.h"

#include <thread>

#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "cast_engine_service_load_callback.h"
#include "cast_session_manager_adaptor.h"
#include "cast_session_manager_service_proxy.h"
#include "i_cast_session.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-SessionManager");

CastSessionManager::CastSessionManager()
{
    CLOGI("in");
}

CastSessionManager &CastSessionManager::GetInstance()
{
    static CastSessionManager instance {};
    return instance;
}

std::shared_ptr<ICastSessionManagerAdaptor> CastSessionManager::GetAdaptor()
{
    CLOGI("in");
    std::lock_guard<std::mutex> lock(mutex_);
    if (adaptor_) {
        return adaptor_;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        CLOGE("Failed to get SA manager");
        return nullptr;
    }
    sptr<CastEngineServiceLoadCallback> loadCallback = new (std::nothrow) CastEngineServiceLoadCallback();
    if (loadCallback == nullptr) {
        CLOGE("Failed to new object");
        return nullptr;
    }

    auto result = samgr->LoadSystemAbility(CAST_ENGINE_SA_ID, loadCallback);
    if (result != ERR_OK) {
        CLOGE("systemAbilityId: %d load failed, result code: %d", CAST_ENGINE_SA_ID, result);
        return nullptr;
    }

    constexpr int32_t sleepTimeMs = 30;
    constexpr int32_t retryTimes = 150; // The service startup timeout interval is 4s.
    int32_t retryTime = 0;

    sptr<IRemoteObject> object;
    while ((object = samgr->CheckSystemAbility(CAST_ENGINE_SA_ID)) == nullptr && (retryTime < retryTimes)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
        retryTime++;
    }
    if (object == nullptr) {
        CLOGE("Failed to get cast engine manager");
        return nullptr;
    }

    auto proxy = iface_cast<CastSessionManagerServiceProxy>(object);
    adaptor_ = std::make_shared<CastSessionManagerAdaptor>(proxy);
    return adaptor_;
}

int32_t CastSessionManager::RegisterListener(std::shared_ptr<ICastSessionManagerListener> listener)
{
    CLOGI("in");
    if (listener == nullptr) {
        CLOGE("Failed to init due to the null listener");
        return CAST_ENGINE_ERROR;
    }
    auto adaptor = GetAdaptor();
    if (!adaptor) {
        return CAST_ENGINE_ERROR;
    }
    sptr<CastEngineServiceDeathRecipient> deathRecipient(
        new (std::nothrow) CastEngineServiceDeathRecipient(listener));
    if (!deathRecipient) {
        CLOGE("Death recipient malloc failed");
        return CAST_ENGINE_ERROR;
    }
    if (adaptor->RegisterListener(listener, deathRecipient) == CAST_ENGINE_SUCCESS) {
        std::lock_guard<std::mutex> lock(mutex_);
        deathRecipient_ = deathRecipient;
        return CAST_ENGINE_SUCCESS;
    }
    return CAST_ENGINE_ERROR;
}

int32_t CastSessionManager::UnregisterListener()
{
    CLOGI("in");
    ReleaseServiceDeathRecipient();
    auto adaptor = GetAdaptor();
    int32_t ret = adaptor ? adaptor->UnregisterListener() : CAST_ENGINE_ERROR;
    std::lock_guard<std::mutex> lock(mutex_);
    adaptor_ = nullptr;
    return ret;
}

int32_t CastSessionManager::Release()
{
    CLOGI("in");
    ReleaseServiceDeathRecipient();
    auto adaptor = GetAdaptor();
    int32_t ret = adaptor ? adaptor->Release() : CAST_ENGINE_ERROR;
    std::lock_guard<std::mutex> lock(mutex_);
    adaptor_ = nullptr;
    return ret;
}

int32_t CastSessionManager::SetLocalDevice(const CastLocalDevice &localDevice)
{
    CLOGI("in");
    if (localDevice.deviceId.empty()) {
        CLOGE("Local device id is null");
        return CAST_ENGINE_ERROR;
    }
    auto adaptor = GetAdaptor();
    return adaptor ? adaptor->SetLocalDevice(localDevice) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManager::CreateCastSession(const CastSessionProperty &property,
    std::shared_ptr<ICastSession> &castSession)
{
    CLOGI("in");
    auto adaptor = GetAdaptor();
    return adaptor ? adaptor->CreateCastSession(property, castSession) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManager::SetSinkSessionCapacity(int sessionCapacity)
{
    CLOGD("in");
    auto adaptor = GetAdaptor();
    return adaptor ? adaptor->SetSinkSessionCapacity(sessionCapacity) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManager::StartDiscovery(int protocols)
{
    CLOGD("in");
    auto adaptor = GetAdaptor();
    return adaptor ? adaptor->StartDiscovery(protocols) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManager::SetDiscoverable(bool enable)
{
    CLOGI("in");
    auto adaptor = GetAdaptor();
    return adaptor ? adaptor->SetDiscoverable(enable) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManager::StopDiscovery()
{
    CLOGI("in");
    auto adaptor = GetAdaptor();
    return adaptor ? adaptor->StopDiscovery() : CAST_ENGINE_ERROR;
}

int32_t CastSessionManager::GetCastSession(std::string sessionId, std::shared_ptr<ICastSession> &castSession)
{
    CLOGI("in");
    auto adaptor = GetAdaptor();
    return adaptor ? adaptor->GetCastSession(sessionId, castSession) : CAST_ENGINE_ERROR;
}

void CastSessionManager::ReleaseClientResources()
{
    CLOGD("Release client resources");
    std::lock_guard<std::mutex> lock(mutex_);
    std::shared_ptr<ICastSessionManagerListener> listener;
    {
        if (!!deathRecipient_) {
            listener = deathRecipient_->GetListener();
        }
        deathRecipient_ = nullptr;
        adaptor_ = nullptr;
    }
    if (listener) {
        listener->OnServiceDied();
    } else {
        CLOGE("Report is nullptr");
    }
}

void CastSessionManager::ReleaseServiceDeathRecipient()
{
    std::lock_guard<std::mutex> lock(mutex_);
    deathRecipient_ = nullptr;
}

void CastSessionManager::CastEngineServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    CLOGE("Service died, need release resources");
    CastSessionManager::GetInstance().ReleaseClientResources();
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
