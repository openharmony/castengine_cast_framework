/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: StreamPlayerImplStub OnRemoteRequest Fuzz.
 * Author: renshuang
 * Create: 2023-06-20
 */

#include "stream_player_impl_stub_fuzzer.h"

#include <string>

#include "add_cast_engine_token_fuzzer.h"
#include "cast_engine_common.h"
#include "cast_engine_common_helper.h"
#include "cast_engine_errors.h"
#include "i_cast_local_file_channel.h"
#include "remote_player_controller.h"
#include "securec.h"
#include "stream_player_impl_stub.h"

using namespace OHOS::CastEngine::CastEngineService;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-StreamPlayerImplStubFuzzer");
namespace {
enum {
    REGISTER_LISTENER = 1,
    UNREGISTER_LISTENER,
    SET_SURFACE,
    PLAY_INDEX,
    START,
    PAUSE,
    PLAY,
    STOP,
    NEXT,
    PREVIOUS,
    SEEK,
    FAST_FORWARD,
    FAST_REWIND,
    SET_VOLUME,
    SET_LOOP_MODE,
    SET_SPEED,
    GET_PLAYER_STATUS,
    GET_POSITION,
    GET_DURATION,
    GET_VOLUME,
    GET_LOOP_MODE,
    GET_PLAY_SPEED,
    GET_MEDIA_INFO_HOLDER,
    RELEASE
};
} // namespace

int32_t StreamPlayerOnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    if (!fileChannel) {
        CLOGE("fileChannel is null");
        return CAST_ENGINE_ERROR;
    }
    auto remotePlayer = std::make_shared<RemotePlayerController>(nullptr, fileChannel);
    if (remotePlayer == nullptr) {
        CLOGE("streamController is null");
        return CAST_ENGINE_ERROR;
    }
    sptr<StreamPlayerImplStub> streamPlayerImplStub = new (std::nothrow) StreamPlayerImplStub(remotePlayer);
    if (!streamPlayerImplStub) {
        CLOGE("streamPlayerImplStub is null");
        return CAST_ENGINE_ERROR;
    }
    MessageOption option;
    CLOGE("StreamPlayerOnRemoteRequest, code:%{public}d", code);
    return streamPlayerImplStub->OnRemoteRequest(code, data, reply, option);
}

int32_t UnregisterListenerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return CAST_ENGINE_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(UNREGISTER_LISTENER, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t PlayIndexFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t index = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;

    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(index)) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(PLAY_INDEX, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t PlayFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;

    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(PLAY, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t PauseFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(PAUSE, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t StopFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(STOP, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t NextFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(NEXT, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t PreviousFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(PREVIOUS, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t SeekFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t position = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(position)) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(SEEK, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t FastForwardFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t delta = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(delta)) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(FAST_FORWARD, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t FastRewindFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t delta = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(delta)) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(FAST_REWIND, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t SetVolumeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t volume = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(volume)) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(SET_VOLUME, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t SetLoopModeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t mode = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(mode)) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(SET_LOOP_MODE, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t SetSpeedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return CAST_ENGINE_ERROR;
    }
    int32_t speed = *reinterpret_cast<const int32_t *>(data);
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (!dataParcel.WriteInt32(speed)) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(SET_SPEED, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetPlayerStatusFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(GET_PLAYER_STATUS, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetPositionFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(GET_POSITION, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetDurationFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(GET_DURATION, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetVolumeFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(GET_VOLUME, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetLoopModeFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(GET_LOOP_MODE, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetPlaySpeedFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(GET_PLAY_SPEED, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t GetMediaInfoHolderFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(GET_MEDIA_INFO_HOLDER, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }
    auto mediaInfos = ReadMediaInfoHolder(reply);
    if (mediaInfos == nullptr) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}

int32_t ReleaseFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel reply;
    if (!dataParcel.WriteInterfaceToken(StreamPlayerImplStub::GetDescriptor())) {
        return CAST_ENGINE_ERROR;
    }
    if (StreamPlayerOnRemoteRequest(RELEASE, dataParcel, reply) != ERR_NONE) {
        return CAST_ENGINE_ERROR;
    }

    return reply.ReadInt32();
}
} // CastEngineService
} // CastEngine
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCastEngineTokenFuzzer::AddCastEngineToken();
    UnregisterListenerFuzzTest(data, size);
    PlayIndexFuzzTest(data, size);
    PlayFuzzTest(data, size);
    PauseFuzzTest(data, size);
    StopFuzzTest(data, size);
    NextFuzzTest(data, size);
    PreviousFuzzTest(data, size);
    SeekFuzzTest(data, size);
    SetVolumeFuzzTest(data, size);
    SetLoopModeFuzzTest(data, size);
    SetSpeedFuzzTest(data, size);
    GetPlayerStatusFuzzTest(data, size);
    GetPositionFuzzTest(data, size);
    GetDurationFuzzTest(data, size);
    GetVolumeFuzzTest(data, size);
    GetLoopModeFuzzTest(data, size);
    GetPlaySpeedFuzzTest(data, size);
    GetMediaInfoHolderFuzzTest(data, size);
    ReleaseFuzzTest(data, size);
    return 0;
}
