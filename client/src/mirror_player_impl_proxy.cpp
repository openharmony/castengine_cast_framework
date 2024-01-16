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
 * Description: supply mirror player implement proxy
 * Author: zhangjingnan
 * Create: 2023-5-27
 */

#include "mirror_player_impl_proxy.h"
#include "cast_engine_common_helper.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-MirrorPlayer");

MirrorPlayerImplProxy::~MirrorPlayerImplProxy()
{
    CLOGI("Stop the client mirror player proxy.");
}

int32_t MirrorPlayerImplProxy::Play(const std::string &deviceId)
{
    MessageParcel data, reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteString(deviceId)) {
        CLOGE("Failed to write the the device id");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(PLAY, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when executing the playing action");
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        CLOGE("Invalid parameter when executing the playing action");
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when executing the playing action");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t MirrorPlayerImplProxy::Pause(const std::string &deviceId)
{
    MessageParcel data, reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteString(deviceId)) {
        CLOGE("Failed to write the the device id");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(PAUSE, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when executing the pausing action");
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        CLOGE("Invalid parameter when executing the pausing action");
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when executing the pausing action");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t MirrorPlayerImplProxy::SetSurface(sptr<IBufferProducer> producer)
{
    MessageParcel data, reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteRemoteObject(producer->AsObject())) {
        CLOGE("Failed to write surface producer");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(SET_SURFACE, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when setting surface");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when setting surface");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t MirrorPlayerImplProxy::SetAppInfo(const AppInfo &appInfo)
{
    MessageParcel data, reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteInt32(appInfo.appUid) || !data.WriteUint32(appInfo.appTokenId) ||
        !data.WriteInt32(appInfo.appPid)) {
        CLOGE("Failed to write appInfo");
        return CAST_ENGINE_ERROR;
    }
    CLOGI("start set app info impl proxy");
    int32_t ret = Remote()->SendRequest(SET_APP_INFO, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when setting surface");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when setting surface");
        return CAST_ENGINE_ERROR;
    }
    return reply.ReadInt32();
}

int32_t MirrorPlayerImplProxy::DeliverInputEvent(const OHRemoteControlEvent &event)
{
    MessageParcel data, reply;
    MessageOption option;

    CLOGD("In, eventType:%d", static_cast<uint32_t>(event.eventType));

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!WriteRemoteControlEvent(data, event)) {
        CLOGE("Failed to write the remote control event");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(DELIVER_INPUT_EVENT, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when deliver input event");
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        CLOGE("Invalid parameter when deliver input event");
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when deliver input event");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t MirrorPlayerImplProxy::InjectEvent(const OHRemoteControlEvent &event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    CLOGD("In, eventType:%d", static_cast<uint32_t>(event.eventType));

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!WriteRemoteControlEvent(data, event)) {
        CLOGE("Failed to write the remote control event");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(INJECT_EVENT, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when deliver input event");
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        CLOGE("Invalid parameter when deliver input event");
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when deliver input event");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t MirrorPlayerImplProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(RELEASE, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when setting surface");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when setting surface");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t MirrorPlayerImplProxy::GetDisplayId(std::string &displayId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    displayId = std::string{};
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(GET_DISPLAYID, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when getDisplayId");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when getDisplayId");
        return CAST_ENGINE_ERROR;
    }
    int32_t errorCode = reply.ReadInt32();
    CHECK_AND_RETURN_RET_LOG(errorCode != CAST_ENGINE_SUCCESS, errorCode, "CastEngine Errors");
    displayId = reply.ReadString();
    return errorCode;
}

int32_t MirrorPlayerImplProxy::ResizeVirtualScreen(uint32_t width, uint32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteUint32(width)) {
        CLOGE("Failed to write the width");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteUint32(height)) {
        CLOGE("Failed to write the height");
        return CAST_ENGINE_ERROR;
    }
    if (Remote()->SendRequest(RESIZE_VIRTUAL_SCREEN, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when ResizeVirtualScreen");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
