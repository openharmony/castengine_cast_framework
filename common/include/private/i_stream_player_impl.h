/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: supply stream player implement interface.
 * Author: huangchanggui
 * Create: 2023-01-12
 */

#ifndef I_STREAM_PLAYER_IMPL_H
#define I_STREAM_PLAYER_IMPL_H

#include <string>
#include "surface_utils.h"
#include "cast_engine_common.h"
#include "cast_service_common.h"
#include "i_stream_player_listener_impl.h"
#include "iremote_broker.h"

namespace OHOS {
namespace CastEngine {
class IStreamPlayerImpl : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.CastEngine.IStreamPlayerImpl");

    IStreamPlayerImpl() = default;
    IStreamPlayerImpl(const IStreamPlayerImpl &) = delete;
    IStreamPlayerImpl &operator=(const IStreamPlayerImpl &) = delete;
    IStreamPlayerImpl(IStreamPlayerImpl &&) = delete;
    IStreamPlayerImpl &operator=(IStreamPlayerImpl &&) = delete;
    virtual ~IStreamPlayerImpl() override = default;

    virtual int32_t RegisterListener(sptr<IStreamPlayerListenerImpl> listener) = 0;
    virtual int32_t UnregisterListener() = 0;
    virtual int32_t SetSurface(sptr<IBufferProducer> producer) = 0;
    virtual int32_t Load(const MediaInfo &mediaInfo) = 0;
    virtual int32_t Play(const MediaInfo &mediaInfo) = 0;
    virtual int32_t Play(int index) = 0;
    virtual int32_t Play() = 0;
    virtual int32_t Pause() = 0;
    virtual int32_t Stop() = 0;
    virtual int32_t Next() = 0;
    virtual int32_t Previous() = 0;
    virtual int32_t Seek(int position) = 0;
    virtual int32_t SetVolume(int volume) = 0;
    virtual int32_t SetLoopMode(const LoopMode mode) = 0;
    virtual int32_t SetSpeed(const PlaybackSpeed speed) = 0;
    virtual int32_t GetPlayerStatus(PlayerStates &playerStates) = 0;
    virtual int32_t GetPosition(int &position) = 0;
    virtual int32_t GetDuration(int &duration) = 0;
    virtual int32_t GetVolume(int &volume, int &maxVolume) = 0;
    virtual int32_t GetLoopMode(LoopMode &loopMode) = 0;
    virtual int32_t GetPlaySpeed(PlaybackSpeed &playbackSpeed) = 0;
    virtual int32_t GetMediaInfoHolder(MediaInfoHolder &mediaInfoHolder) = 0;
    virtual int32_t Release() = 0;

protected:
    enum {
        REGISTER_LISTENER = 1,
        UNREGISTER_LISTENER,
        SET_SURFACE,
        PLAY_INDEX,
        LOAD,
        START,
        PAUSE,
        PLAY,
        STOP,
        NEXT,
        PREVIOUS,
        SEEK,
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
    static const size_t MAX_PLAY_LIST_SIZE = 100;
};
} // namespace CastEngine
} // namespace OHOS

#endif
