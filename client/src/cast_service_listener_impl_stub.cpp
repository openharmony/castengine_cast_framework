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
 * Description: supply cast service listener implement proxy apis.
 * Author: zhangge
 * Create: 2022-6-15
 */

#include "cast_service_listener_impl_stub.h"

#include <variant>

#include "cast_engine_common.h"
#include "cast_engine_common_helper.h"
#include "cast_engine_log.h"
#include "cast_session.h"
#include "cast_stub_helper.h"
#include "i_cast_session_manager_listener.h"
#include "iremote_stub.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-ServiceListener");

CastServiceListenerImplStub::CastServiceListenerImplStub(std::shared_ptr<ICastSessionManagerListener> userListener)
    : userListener_(userListener)
{
    FILL_SINGLE_STUB_TASK(ON_DEVICE_FOUND, &CastServiceListenerImplStub::DoDeviceFoundTask);
    FILL_SINGLE_STUB_TASK(ON_DEVICE_OFFLINE, &CastServiceListenerImplStub::DoDeviceOfflineTask);
    FILL_SINGLE_STUB_TASK(ON_SESSION_CREATE, &CastServiceListenerImplStub::DoSessionCreateTask);
    FILL_SINGLE_STUB_TASK(ON_SERVICE_DIE, &CastServiceListenerImplStub::DoServiceDieTask);
    FILL_SINGLE_STUB_TASK(ON_LOG_EVENT, &CastServiceListenerImplStub::DoOnLogEventTask);
}

int CastServiceListenerImplStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    RETURN_IF_WRONG_TASK(code, data, reply, option);
    return EXECUTE_SINGLE_STUB_TASK(code, data, reply);
}

bool CastServiceListenerImplStub::GetCastRemoteDevices(MessageParcel &data, std::vector<CastRemoteDevice> &deviceList)
{
    auto size = data.ReadInt32();
    if (size <= 0 || size > MAX_DEVICE_NUM) {
        return false;
    }
    deviceList.resize(size);
    for (int32_t i = 0; i < size; ++i) {
        if (!ReadCastRemoteDevice(data, deviceList[i])) {
            return false;
        }
    }

    return true;
}

bool CastServiceListenerImplStub::GetCastSession(MessageParcel &data, std::shared_ptr<ICastSession> &castSession)
{
    auto object = data.ReadRemoteObject();
    auto impl = (object == nullptr) ? nullptr : iface_cast<ICastSessionImpl>(object);
    if (impl == nullptr) {
        CLOGE("Failed to cast ICastSessionImpl");
        return false;
    }

    auto session = std::make_shared<CastSession>(impl);
    if (session == nullptr) {
        CLOGE("Failed to malloc cast session");
        return false;
    }
    castSession = session;
    return true;
}

int32_t CastServiceListenerImplStub::DoDeviceFoundTask(MessageParcel &data, MessageParcel &reply)
{
    CLOGI("%{public}s in", __func__);
    std::vector<CastRemoteDevice> remoteDevices;
    if (!GetCastRemoteDevices(data, remoteDevices)) {
        CLOGE("DoDeviceFoundTask, failed get remote devices info");
        return ERR_INVALID_DATA;
    }
    userListener_->OnDeviceFound(remoteDevices);

    return ERR_NONE;
}

int32_t CastServiceListenerImplStub::DoDeviceOfflineTask(MessageParcel &data, MessageParcel &reply)
{
    CLOGI("%{public}s in", __func__);
    std::string deviceId = data.ReadString();
    if (deviceId.empty()) {
        CLOGE("%{public}s, failed get deviceId", __func__);
        return ERR_INVALID_DATA;
    }
    userListener_->OnDeviceOffline(deviceId);
    return ERR_NONE;
}

int32_t CastServiceListenerImplStub::DoSessionCreateTask(MessageParcel &data, MessageParcel &reply)
{
    CLOGI("%{public}s in", __func__);
    std::shared_ptr<ICastSession> castSession;
    if (!GetCastSession(data, castSession)) {
        CLOGE("%{public}s, failed get cast session info", __func__);
        return ERR_INVALID_DATA;
    }
    userListener_->OnSessionCreated(castSession);

    return ERR_NONE;
}

int32_t CastServiceListenerImplStub::DoServiceDieTask(MessageParcel &data, MessageParcel &reply)
{
    userListener_->OnServiceDied();
    return ERR_NONE;
}

int32_t CastServiceListenerImplStub::DoOnLogEventTask(MessageParcel &data, MessageParcel &reply)
{
    CLOGI("%{public}s in", __func__);
    int32_t eventId = data.ReadInt32();
    int64_t param = data.ReadInt64();
    userListener_->OnLogEvent(eventId, param);
    return ERR_NONE;
}

void CastServiceListenerImplStub::OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList)
{
    static_cast<void>(deviceList);
}

void CastServiceListenerImplStub::OnDeviceOffline(const std::string &deviceId)
{
    static_cast<void>(deviceId);
}

void CastServiceListenerImplStub::OnSessionCreated(const sptr<ICastSessionImpl> &castSession)
{
    static_cast<void>(castSession);
}

void CastServiceListenerImplStub::OnServiceDied() {}

void CastServiceListenerImplStub::OnLogEvent(int32_t eventId, int64_t param)
{
    static_cast<void>(eventId);
    static_cast<void>(param);
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
