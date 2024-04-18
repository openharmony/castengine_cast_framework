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
 * Description: supply cast service listener implement proxy class.
 * Author: zhangge
 * Create: 2022-6-15
 */

#include "cast_service_listener_impl_proxy.h"

#include <cast_engine_common_helper.h>
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Service-ListenerImpl");

void CastServiceListenerImplProxy::OnServiceDied()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (Remote()->SendRequest(ON_SERVICE_DIE, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when reporting service die");
    }
}

namespace {
bool FillCastRemoteDevices(MessageParcel &data, const std::vector<CastRemoteDevice> &devices)
{
    auto size = devices.size();
    if (size <= 0) {
        CLOGE("devices size <= 0");
        return false;
    }
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        return false;
    }
    for (auto &device : devices) {
        if (!WriteCastRemoteDevice(data, device)) {
            CLOGE("Failed to write the remote device item");
            return false;
        }
    }
    return true;
}

bool FillCastSession(MessageParcel &data, const sptr<ICastSessionImpl> &castSession)
{
    if (!castSession) {
        return false;
    }
    if (!data.WriteRemoteObject(castSession->AsObject())) {
        CLOGE("Failed to write cast session");
        return false;
    }
    return true;
}
} // namespace

void CastServiceListenerImplProxy::OnDeviceFound(const std::vector<CastRemoteDevice> &devices)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!FillCastRemoteDevices(data, devices)) {
        CLOGE("Failed to Write remote devices");
        return;
    }
    Remote()->SendRequest(ON_DEVICE_FOUND, data, reply, option);
}

void CastServiceListenerImplProxy::OnSessionCreated(const sptr<ICastSessionImpl> &castSession)
{
    MessageParcel data, reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!FillCastSession(data, castSession)) {
        CLOGE("Failed to Write cast session");
        return;
    }
    Remote()->SendRequest(ON_SESSION_CREATE, data, reply, option);
}

void CastServiceListenerImplProxy::OnDeviceOffline(const std::string &deviceId)
{
    MessageParcel data, reply;
    MessageOption option;
    CLOGD("OnDeviceOffline in");
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!data.WriteString(deviceId)) {
        CLOGE("Failed to Write deviceId");
        return;
    }
    CLOGD("send request");
    Remote()->SendRequest(ON_DEVICE_OFFLINE, data, reply, option);
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS