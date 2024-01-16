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
 * Description: supply cast mirror player implement stub class.
 * Author: zhangjingnan
 * Create: 2023-05-17
 */

#include "mirror_player_impl_stub.h"
#include "cast_engine_common_helper.h"
#include "permission.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-MirrorPlayerImpl");

int MirrorPlayerImplStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    RETRUEN_IF_WRONG_TASK(code, data, reply, option);
    return EXECUTE_SINGLE_STUB_TASK(code, data, reply);
}

MirrorPlayerImplStub::MirrorPlayerImplStub()
{
    FILL_SINGLE_STUB_TASK(PLAY, &MirrorPlayerImplStub::DoPlayTask);
    FILL_SINGLE_STUB_TASK(PAUSE, &MirrorPlayerImplStub::DoPauseTask);
    FILL_SINGLE_STUB_TASK(SET_SURFACE, &MirrorPlayerImplStub::DoSetSurface);
    FILL_SINGLE_STUB_TASK(DELIVER_INPUT_EVENT, &MirrorPlayerImplStub::DoDeliverInputEvent);
    FILL_SINGLE_STUB_TASK(INJECT_EVENT, &MirrorPlayerImplStub::DoInjectEvent);
    FILL_SINGLE_STUB_TASK(RELEASE, &MirrorPlayerImplStub::DoRelease);
    FILL_SINGLE_STUB_TASK(SET_APP_INFO, &MirrorPlayerImplStub::DoSetAppInfo);
    FILL_SINGLE_STUB_TASK(GET_DISPLAYID, &MirrorPlayerImplStub::DoGetDisplayId);
    FILL_SINGLE_STUB_TASK(RESIZE_VIRTUAL_SCREEN, &MirrorPlayerImplStub::DoResizeVirtualScreen);
}

int32_t MirrorPlayerImplStub::DoPlayTask(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }
    std::string deviceId = data.ReadString();
    if (deviceId.empty()) {
        CLOGE("The device id is empty");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(Play(deviceId))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoPauseTask(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    std::string deviceId = data.ReadString();
    if (deviceId.empty()) {
        CLOGE("The device id is empty");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(Pause(deviceId))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoSetSurface(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
    if (remoteObj == nullptr) {
        CLOGE("BufferProducer is null");
        return ERR_NULL_OBJECT;
    }

    sptr<IBufferProducer> producer = iface_cast<IBufferProducer>(remoteObj);
    if (!reply.WriteInt32(SetSurface(producer))) {
        CLOGE("Failed to write int SetSurface");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoSetAppInfo(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    if (!reply.WriteInt32(SetAppInfo({data.ReadInt32(), data.ReadUint32(), data.ReadInt32()}))) {
        CLOGE("Failed to write int SetSurface");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoDeliverInputEvent(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    auto remoteControlEvent = ReadRemoteControlEvent(data);
    if (remoteControlEvent == nullptr) {
        CLOGE("Invalid remote control event comes");
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteInt32(DeliverInputEvent(*remoteControlEvent))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoInjectEvent(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    auto remoteControlEvent = ReadRemoteControlEvent(data);
    if (remoteControlEvent == nullptr) {
        CLOGE("Invalid remote control event comes");
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteInt32(InjectEvent(*remoteControlEvent))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoRelease(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    static_cast<void>(data);

    if (!reply.WriteInt32(Release())) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoGetDisplayId(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    static_cast<void>(data);
    std::string displayId{};
    int32_t ret = GetDisplayId(displayId);
    if (!reply.WriteInt32(ret)) {
        CLOGE("Failed to write ret:%{public}d", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    if (!reply.WriteString(displayId)) {
        CLOGE("Failed to write displayId id:%s", displayId.c_str());
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}

int32_t MirrorPlayerImplStub::DoResizeVirtualScreen(MessageParcel &data, MessageParcel &reply)
{
    if (!Permission::CheckMirrorPermission()) {
        return ERR_UNKNOWN_TRANSACTION;
    }

    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    if (!reply.WriteInt32(ResizeVirtualScreen(width, height))) {
        CLOGE("Failed to write int value");
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    return ERR_NONE;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
