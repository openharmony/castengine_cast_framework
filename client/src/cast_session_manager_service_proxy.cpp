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
 * Description: supply cast session manager service proxy
 * Author: zhangge
 * Create: 2022-5-29
 */

#include "cast_engine_errors.h"
#include "cast_session_manager_service_proxy.h"
#include "cast_engine_common_helper.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-Manager");

int32_t CastSessionManagerServiceProxy::RegisterListener(sptr<ICastServiceListenerImpl> listener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteRemoteObject(listener->AsObject())) {
        CLOGE("Failed to write cast service listener");
        return CAST_ENGINE_ERROR;
    }
    if (Remote()->SendRequest(REGISTER_LISTENER, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when initing the cast service");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::UnregisterListener()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = Remote()->SendRequest(UNREGISTER_LISTENER, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when unregistering listener");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when unregistering listener");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::Release()
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
        CLOGE("No permission when Releasing the cast service");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when Releasing the cast service");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::SetLocalDevice(const CastLocalDevice &localDevice)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!WriteCastLocalDevice(data, localDevice)) {
        CLOGE("Failed to write cast local device");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(SET_LOCAL_DEVICE, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when setting local device");
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        CLOGE("Invalid parameter when setting local device");
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when setting local device");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::CreateCastSession(const CastSessionProperty &property,
    sptr<ICastSessionImpl> &castSession)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!WriteCastSessionProperty(data, property)) {
        CLOGE("Failed to write cast session property");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(CREATE_CAST_SESSION, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when creating cast session");
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        CLOGE("Invalid parameter when creating cast session");
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when creating cast session");
        return CAST_ENGINE_ERROR;
    }

    int32_t errorCode = reply.ReadInt32();
    CHECK_AND_RETURN_RET_LOG(errorCode != CAST_ENGINE_SUCCESS, errorCode, "CastEngine Errors");
    auto object = reply.ReadRemoteObject();
    if (object == nullptr) {
        CLOGE("Failed to get the cast session object");
        return CAST_ENGINE_ERROR;
    }
    castSession = iface_cast<ICastSessionImpl>(object);

    return errorCode;
}

int32_t CastSessionManagerServiceProxy::SetSinkSessionCapacity(int sessionCapacity)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteInt32(sessionCapacity)) {
        CLOGE("Failed to write the session capacity");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(SET_SINK_SESSION_CAPACITY, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when setting sink session capacity");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when setting sink session capacity");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::StartDiscovery(int protocols)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteInt32(protocols)) {
        CLOGE("Failed to write the protocol type");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(START_DISCOVERY, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when starting discovery");
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        CLOGE("Invalid parameter when starting discovery");
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when starting discovery");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::SetDiscoverable(bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteBool(enable)) {
        CLOGE("Failed to write discoverable value");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(SET_DISCOVERABLE, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when setting discoverable");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when setting discoverable");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::StopDiscovery()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = Remote()->SendRequest(STOP_DISCOVERY, data, reply, option);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        CLOGE("No permission when setting discoverable");
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        CLOGE("Failed to send ipc request when setting discoverable");
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CastSessionManagerServiceProxy::GetCastSession(std::string sessionId, sptr<ICastSessionImpl> &castSession)
{
    MessageParcel data, reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return CAST_ENGINE_ERROR;
    }
    if (!data.WriteString(sessionId)) {
        CLOGE("Failed to write session ID");
        return CAST_ENGINE_ERROR;
    }
    if (Remote()->SendRequest(GET_CAST_SESSION, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when get cast session");
        return CAST_ENGINE_ERROR;
    }

    int32_t errorCode = reply.ReadInt32();
    CHECK_AND_RETURN_RET_LOG(errorCode != CAST_ENGINE_SUCCESS, errorCode, "CastEngine Errors");
    auto object = reply.ReadRemoteObject();
    if (object == nullptr) {
        CLOGE("Failed to get the cast session object");
        return CAST_ENGINE_ERROR;
    }
    castSession = iface_cast<ICastSessionImpl>(object);

    return errorCode;
}

sptr<IRemoteObject> CastSessionManagerServiceProxy::GetSessionManagerService()
{
    return this->AsObject();
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
