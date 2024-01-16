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
 * Description: supply cast session implement stub class.
 * Author: zhangge
 * Create: 2022-06-15
 */

#include "cast_session_impl_stub.h"

#include "cast_engine_common_helper.h"
#include "cast_session_listener_impl_proxy.h"
#include "permission.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-SessionImpl");

int CastSessionImplStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    RETRUEN_IF_WRONG_TASK(code, data, reply, option);
    return EXECUTE_SINGLE_STUB_TASK(code, data, reply);
}

CastSessionImplStub::CastSessionImplStub()
{
    FILL_SINGLE_STUB_TASK(REGISTER_LISTENER, &CastSessionImplStub::DoRegisterListenerTask);
    FILL_SINGLE_STUB_TASK(UNREGISTER_LISTENER, &CastSessionImplStub::DoUnregisterListenerTask);
    FILL_SINGLE_STUB_TASK(ADD_DEVICE, &CastSessionImplStub::DoAddDeviceTask);
    FILL_SINGLE_STUB_TASK(REMOVE_DEVICE, &CastSessionImplStub::DoRemoveDeviceTask);
    FILL_SINGLE_STUB_TASK(START_AUTH, &CastSessionImplStub::DoStartAuthTask);
    FILL_SINGLE_STUB_TASK(GET_SESSION_ID, &CastSessionImplStub::DoGetSessionIdTask);
    FILL_SINGLE_STUB_TASK(GET_DEVICE_STATE, &CastSessionImplStub::DoGetDeviceStateTask);
    FILL_SINGLE_STUB_TASK(SET_SESSION_PROPERTY, &CastSessionImplStub::DoSetSessionProperty);
    FILL_SINGLE_STUB_TASK(CREAT_MIRROR_PLAYER, &CastSessionImplStub::DoCreateMirrorPlayer);
    FILL_SINGLE_STUB_TASK(CREAT_STREAM_PLAYER, &CastSessionImplStub::DoCreateStreamPlayer);
    FILL_SINGLE_STUB_TASK(NOTIFY_EVENT, &CastSessionImplStub::DoNotifyEvent);
    FILL_SINGLE_STUB_TASK(SET_CAST_MODE, &CastSessionImplStub::DoSetCastMode);
    FILL_SINGLE_STUB_TASK(RELEASE, &CastSessionImplStub::DoRelease);
}

int32_t CastSessionImplStub::DoRegisterListenerTask(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        return ERR_NULL_OBJECT;
    }

    sptr<ICastSessionListenerImpl> listener{ new CastSessionListenerImplProxy(obj) };
    if (!reply.WriteInt32(RegisterListener(listener))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t CastSessionImplStub::DoUnregisterListenerTask(MessageParcel &data, MessageParcel &reply)
{
    static_cast<void>(data);
    if (!reply.WriteInt32(UnregisterListener())) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t CastSessionImplStub::DoAddDeviceTask(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission() && !Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }
    auto device = ReadCastRemoteDevice(data);
    if (device == nullptr) {
        CLOGE("Invalid remote device object comes");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(AddDevice(*device))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t CastSessionImplStub::DoRemoveDeviceTask(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission() && !Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    std::string deviceId = data.ReadString();
    if (deviceId.empty()) {
        CLOGE("The device id is empty");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(RemoveDevice(deviceId))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t CastSessionImplStub::DoStartAuthTask(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission() && !Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    auto authInfo = ReadAuthInfo(data);
    if (authInfo == nullptr) {
        CLOGE("Invalid auth info comes");
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteInt32(StartAuth(*authInfo))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t CastSessionImplStub::DoGetSessionIdTask(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission() && !Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }
    static_cast<void>(data);
    std::string sessionId{};
    int32_t ret = GetSessionId(sessionId);
    if (!reply.WriteInt32(ret)) {
        CLOGE("Failed to write ret:%d", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    if (!reply.WriteString(sessionId)) {
        CLOGE("Failed to write session id:%s", sessionId.c_str());
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t CastSessionImplStub::DoGetDeviceStateTask(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission() && !Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    std::string deviceId = data.ReadString();
    if (deviceId.empty()) {
        CLOGE("The device id is empty");
        return ERR_INVALID_DATA;
    }

    DeviceState state = DeviceState::DISCONNECTED;
    int32_t ret = GetDeviceState(deviceId, state);
    if (!reply.WriteInt32(ret)) {
        CLOGE("Failed to write ret:%d", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    if (!reply.WriteInt32(static_cast<int32_t>(state))) {
        CLOGE("Failed to write device state:%d", static_cast<int32_t>(state));
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t CastSessionImplStub::DoSetSessionProperty(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission() && !Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    auto property = ReadCastSessionProperty(data);
    if (property == nullptr) {
        CLOGE("Invalid property object");
        return ERR_NULL_OBJECT;
    }

    if (!reply.WriteInt32(SetSessionProperty(*property))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t CastSessionImplStub::DoCreateMirrorPlayer(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }
    sptr<IMirrorPlayerImpl> mirrorPlayerStub;
    int32_t ret = CreateMirrorPlayer(mirrorPlayerStub);
    if (mirrorPlayerStub == nullptr) {
        return IPC_STUB_ERR;
    }
    if (!reply.WriteInt32(ret)) {
        CLOGE("Failed to write ret:%d", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    if (!reply.WriteRemoteObject(mirrorPlayerStub->AsObject())) {
        CLOGE("Failed to write mirror player");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    CLOGI("CreateMirrorPlayer");
    return ERR_NONE;
}

int32_t CastSessionImplStub::DoCreateStreamPlayer(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    sptr<IStreamPlayerIpc> streamPlayerStub;
    int32_t ret = CreateStreamPlayer(streamPlayerStub);
    if (!streamPlayerStub) {
        return IPC_STUB_ERR;
    }
    if (!reply.WriteInt32(ret)) {
        CLOGE("Failed to write ret:%d", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    if (!reply.WriteRemoteObject(streamPlayerStub->AsObject())) {
        CLOGE("Failed to write stream player");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t CastSessionImplStub::DoRelease(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission() && !Permission::CheckStreamPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    static_cast<void>(data);

    if (!reply.WriteInt32(Release())) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t CastSessionImplStub::DoSetCastMode(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }
    int32_t mode = data.ReadInt32();
    std::string jsonParam = data.ReadString();
    if (!reply.WriteInt32(SetCastMode(static_cast<CastMode>(mode), jsonParam))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t CastSessionImplStub::DoNotifyEvent(MessageParcel &data, MessageParcel &reply)
{
    EventId eventId = static_cast<EventId>(data.ReadInt32());
    std::string param = data.ReadString();
    NotifyEvent(eventId, param);
    return ERR_NONE;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
