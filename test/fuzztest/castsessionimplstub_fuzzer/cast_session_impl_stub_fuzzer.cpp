/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: CastSessionImplStub OnRemoteRequest Fuzz.
 * Author: renshuang
 * Create: 2023-06-29
 */

#include "cast_session_impl_stub_fuzzer.h"

#include <string>

#include "add_cast_engine_token_fuzzer.h"
#include "cast_engine_common.h"
#include "cast_engine_common_helper.h"
#include "cast_engine_errors.h"
#include "cast_session_impl.h"

using namespace OHOS::CastEngine::CastEngineService;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-SessionImplStubFuzzer");

namespace {
enum {
    REGISTER_LISTENER = 1,
    UNREGISTER_LISTENER,
    ADD_DEVICE,
    REMOVE_DEVICE,
    START_AUTH,
    GET_SESSION_ID,
    GET_DEVICE_STATE,
    SET_SESSION_PROPERTY,
    CREAT_MIRROR_PLAYER,
    CREAT_STREAM_PLAYER,
    RELEASE,
    NOTIFY_EVENT,
    SET_CAST_MODE,
};
} // namespace

int32_t CastSessionImplStubOnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    OHOS::CastEngine::CastSessionProperty property{};
    OHOS::CastEngine::CastLocalDevice localDevice{};
    sptr<CastSessionImpl> castSessionImpl = new (std::nothrow) CastSessionImpl(property, localDevice);
    if (!castSessionImpl) {
        CLOGE("CastSessionImpl is null");
        return CAST_ENGINE_ERROR;
    }
    castSessionImpl->Init();
    MessageOption option;
    CLOGE("CastSessionImplStubOnRemoteRequest, code:%{public}d", code);
    return castSessionImpl->OnRemoteRequest(code, data, reply, option);
}

int32_t UnregisterListenerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (CastSessionImplStubOnRemoteRequest(UNREGISTER_LISTENER, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }
    return reply.ReadInt32();
}

int32_t RemoveDeviceFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    std::string deviceId(reinterpret_cast<const char *>(data), size);

    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteString(deviceId)) {
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = CastSessionImplStubOnRemoteRequest(REMOVE_DEVICE, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetSessionIdFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = CastSessionImplStubOnRemoteRequest(GET_SESSION_ID, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetDeviceStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    std::string deviceId(reinterpret_cast<const char *>(data), size);

    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteString(deviceId)) {
        return CAST_ENGINE_ERROR;
    }

    int32_t ret = CastSessionImplStubOnRemoteRequest(GET_DEVICE_STATE, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CreateMirrorPlayerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionImplStubOnRemoteRequest(CREAT_MIRROR_PLAYER, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t CreateStreamPlayerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionImplStubOnRemoteRequest(CREAT_STREAM_PLAYER, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t ReleaseFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionImplStubOnRemoteRequest(RELEASE, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t SetCastModeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t mode = *reinterpret_cast<const int32_t *>(data);
    std::string jsonParam(reinterpret_cast<const char *>(data), size);
    MessageParcel dataParcel;
    MessageParcel reply;

    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(mode)) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteString(jsonParam)) {
        return CAST_ENGINE_ERROR;
    }
    if (CastSessionImplStubOnRemoteRequest(SET_CAST_MODE, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t NotifyEventFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t eventId = *reinterpret_cast<const int32_t *>(data);
    std::string jsonParam(reinterpret_cast<const char *>(data), size);
    MessageParcel dataParcel;
    MessageParcel reply;

    if (!dataParcel.WriteInterfaceToken(CastSessionImpl::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(eventId)) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteString(jsonParam)) {
        return CAST_ENGINE_ERROR;
    }
    if (CastSessionImplStubOnRemoteRequest(NOTIFY_EVENT, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }
    return CAST_ENGINE_SUCCESS;
}

} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::AddCastEngineTokenFuzzer::AddCastEngineToken();
    UnregisterListenerFuzzTest(data, size);
    RemoveDeviceFuzzTest(data, size);
    GetSessionIdFuzzTest(data, size);
    GetDeviceStateFuzzTest(data, size);
    SetCastModeFuzzTest(data, size);
    NotifyEventFuzzTest(data, size);
    CreateMirrorPlayerFuzzTest(data, size);
    CreateStreamPlayerFuzzTest(data, size);
    ReleaseFuzzTest(data, size);
    return 0;
}

