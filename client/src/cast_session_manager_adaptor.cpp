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
 * Description: supply cast session manager adaptor.
 * Author: zhangge
 * Create: 2022-5-29
 */

#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "cast_session_manager_adaptor.h"
#include "cast_service_listener_impl_stub.h"
#include "cast_session.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-ManagerAdaptor");

CastSessionManagerAdaptor::~CastSessionManagerAdaptor()
{
    CLOGD("destructor in");
    UnsubscribeDeathRecipient();
}

int32_t CastSessionManagerAdaptor::RegisterListener(std::shared_ptr<ICastSessionManagerListener> listener,
    sptr<IRemoteObject::DeathRecipient> deathRecipient)
{
    sptr<ICastServiceListenerImpl> impl = new (std::nothrow) CastServiceListenerImplStub(listener);
    if (impl == nullptr) {
        CLOGE("Failed to malloc service listener");
        return CAST_ENGINE_ERROR;
    }
    auto object = proxy_ ? proxy_->GetSessionManagerService() : nullptr;
    if (!object) {
        CLOGW("Failed to get session manager service");
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = proxy_ ? proxy_->RegisterListener(impl) : CAST_ENGINE_ERROR;
    if (ret != CAST_ENGINE_SUCCESS) {
        return ret;
    }
    if (object->AddDeathRecipient(deathRecipient)) {
        std::lock_guard<std::mutex> lock(mutex_);
        deathRecipient_ = deathRecipient;
        remote_ = object;
    } else {
        CLOGE("Add cast engine service death recipient failed");
    }

    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionManagerAdaptor::UnregisterListener()
{
    UnsubscribeDeathRecipient();
    return proxy_ ? proxy_->UnregisterListener() : CAST_ENGINE_ERROR;
}

int32_t CastSessionManagerAdaptor::Release()
{
    UnsubscribeDeathRecipient();
    return proxy_ ? proxy_->Release() : CAST_ENGINE_ERROR;
}

int32_t CastSessionManagerAdaptor::SetLocalDevice(const CastLocalDevice &localDevice)
{
    return proxy_ ? proxy_->SetLocalDevice(localDevice) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManagerAdaptor::CreateCastSession(const CastSessionProperty &property,
    std::shared_ptr<ICastSession> &castSession)
{
    if (!proxy_) {
        CLOGE("proxy is null");
        return CAST_ENGINE_ERROR;
    }
    sptr<ICastSessionImpl> impl;
    int32_t ret = proxy_->CreateCastSession(property, impl);
    CHECK_AND_RETURN_RET_LOG(ret != CAST_ENGINE_SUCCESS, ret, "CastEngine Errors");
    if (impl == nullptr) {
        CLOGE("cast session is NULL");
        return CAST_ENGINE_ERROR;
    }

    auto session = std::make_shared<CastSession>(impl);
    if (!session) {
        CLOGE("Failed to malloc cast session");
        return ERR_NO_MEMORY;
    }
    std::string sessionId{};
    impl->GetSessionId(sessionId);
    session->SetSessionId(sessionId);
    castSession = session;

    return ret;
}

int32_t CastSessionManagerAdaptor::SetSinkSessionCapacity(int sessionCapacity)
{
    return proxy_ ? proxy_->SetSinkSessionCapacity(sessionCapacity) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManagerAdaptor::StartDiscovery(int protocols)
{
    return proxy_ ? proxy_->StartDiscovery(protocols) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManagerAdaptor::SetDiscoverable(bool enable)
{
    return proxy_ ? proxy_->SetDiscoverable(enable) : CAST_ENGINE_ERROR;
}

int32_t CastSessionManagerAdaptor::StopDiscovery()
{
    return proxy_ ? proxy_->StopDiscovery() : CAST_ENGINE_ERROR;
}

int32_t CastSessionManagerAdaptor::GetCastSession(std::string sessionId, std::shared_ptr<ICastSession> &castSession)
{
    sptr<ICastSessionImpl> impl;
    int32_t ret = proxy_->GetCastSession(sessionId, impl);
    CHECK_AND_RETURN_RET_LOG(ret != CAST_ENGINE_SUCCESS, ret, "CastEngine Errors");
    if (impl == nullptr) {
        CLOGE("cast session is NULL");
        return CAST_ENGINE_ERROR;
    }

    auto session = std::make_shared<CastSession>(impl);
    if (!session) {
        CLOGE("Failed to malloc cast session");
        return CAST_ENGINE_ERROR;
    }
    std::string id{};
    impl->GetSessionId(id);
    session->SetSessionId(id);
    castSession = session;

    return ret;
}

void CastSessionManagerAdaptor::UnsubscribeDeathRecipient()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!deathRecipient_) {
        CLOGE("deathRecipient is null");
        return;
    }
    sptr<IRemoteObject> remote = remote_.promote();
    if (!!remote) {
        remote->RemoveDeathRecipient(deathRecipient_);
    }
    deathRecipient_ = nullptr;
    CLOGD("Unsubscribe Death Recipient Success");
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
