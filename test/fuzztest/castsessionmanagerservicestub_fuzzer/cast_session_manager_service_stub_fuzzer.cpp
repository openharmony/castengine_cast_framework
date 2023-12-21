/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: CastSessionManagerServiceStub OnRemoteRequest Fuzz.
 * Author: renshuang
 * Create: 2023-06-29
 */

#include "cast_session_manager_service_stub_fuzzer.h"

#include "add_cast_engine_token_fuzzer.h"
#include "cast_engine_common.h"
#include "cast_engine_common_helper.h"
#include "cast_engine_errors.h"
#include "cast_session_manager_service.h"

using namespace OHOS::CastEngine::CastEngineService;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-SessionManagerServiceStubFuzzer");
namespace {
enum {
    REGISTER_LISTENER = 0,
    UNREGISTER_LISTENER,
    RELEASE,
    SET_LOCAL_DEVICE,
    CREATE_CAST_SESSION,
    SET_SINK_SESSION_CAPACITY,
    START_DISCOVERY,
    SET_DISCOVERABLE,
    STOP_DISCOVERY,
    GET_CAST_SESSION
};
} // namespace

int32_t CastSessionManagerServiceStubOnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    bool runOnCreate = false;
    sptr<CastSessionManagerService> service =
        new (std::nothrow) CastSessionManagerService(CAST_ENGINE_SA_ID, runOnCreate);
    if (!service) {
        CLOGE("CastSessionManagerService is null");
        return CAST_ENGINE_ERROR;
    }
    MessageOption option;
    CLOGE("CastSessionManagerServiceStubOnRemoteRequest, code:%{public}d", code);
    return service->OnRemoteRequest(code, data, reply, option);
}

int32_t UnregisterListenerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionManagerService::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionManagerServiceStubOnRemoteRequest(UNREGISTER_LISTENER, dataParcel, reply);
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
    if (!dataParcel.WriteInterfaceToken(CastSessionManagerService::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionManagerServiceStubOnRemoteRequest(RELEASE, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t SetSinkSessionCapacityFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t sessionCapacity = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionManagerService::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(sessionCapacity)) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionManagerServiceStubOnRemoteRequest(SET_SINK_SESSION_CAPACITY, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t StartDiscoveryFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t protocols = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionManagerService::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(protocols)) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionManagerServiceStubOnRemoteRequest(START_DISCOVERY, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret == ERR_INVALID_DATA) {
        return ERR_INVALID_PARAM;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t SetDiscoverableFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(bool))) {
        return CAST_ENGINE_ERROR;
    }
    bool enable = static_cast<bool>(*data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionManagerService::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteBool(enable)) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionManagerServiceStubOnRemoteRequest(SET_DISCOVERABLE, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t StopDiscoveryFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(CastSessionManagerService::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    int32_t ret = CastSessionManagerServiceStubOnRemoteRequest(STOP_DISCOVERY, dataParcel, reply);
    if (ret == ERR_UNKNOWN_TRANSACTION) {
        return ERR_NO_PERMISSION;
    } else if (ret != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetCastSessionFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    std::string sessionId(reinterpret_cast<const char *>(data), size);
    if (!dataParcel.WriteInterfaceToken(CastSessionManagerService::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteString(sessionId)) {
        return CAST_ENGINE_ERROR;
    }
    if (CastSessionManagerServiceStubOnRemoteRequest(GET_CAST_SESSION, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }
    auto object = reply.ReadRemoteObject();
    if (object == nullptr) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCastEngineTokenFuzzer::AddCastEngineToken();
    UnregisterListenerFuzzTest(data, size);
    ReleaseFuzzTest(data, size);
    SetSinkSessionCapacityFuzzTest(data, size);
    StartDiscoveryFuzzTest(data, size);
    SetDiscoverableFuzzTest(data, size);
    StopDiscoveryFuzzTest(data, size);
    GetCastSessionFuzzTest(data, size);
    return 0;
}
