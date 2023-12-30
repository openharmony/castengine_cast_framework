/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply Stream Player definition.
 * Author: huangchanggui
 * Create: 2023-01-12
 */

#ifndef STREAM_PLAYER_H
#define STREAM_PLAYER_H

#include "i_stream_player.h"
#include "i_stream_player_ipc.h"
#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class StreamPlayer : public IStreamPlayer {
public:
    explicit StreamPlayer(sptr<IStreamPlayerIpc> proxy) : proxy_(proxy) {}
    ~StreamPlayer() override;

    int32_t RegisterListener(std::shared_ptr<IStreamPlayerListener> listener) override;
    int32_t UnregisterListener() override;
    int32_t SetSurface(const std::string &surfaceId) override;
    int32_t Load(const MediaInfo &mediaInfo) override;
    int32_t Play(const MediaInfo &mediaInfo) override;
    int32_t Play(int index) override;
    int32_t Play() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Next() override;
    int32_t Previous() override;
    int32_t Seek(int position) override;
    int32_t FastForward(int delta) override;
    int32_t FastRewind(int delta) override;
    int32_t SetVolume(int volume) override;
    int32_t SetMute(bool mute) override;
    int32_t SetLoopMode(const LoopMode mode) override;
    int32_t SetSpeed(const PlaybackSpeed speed) override;
    int32_t GetPlayerStatus(PlayerStates &playerStates) override;
    int32_t GetPosition(int &position) override;
    int32_t GetDuration(int &duration) override;
    int32_t GetVolume(int &volume, int &maxVolume) override;
    int32_t GetMute(bool &mute) override;
    int32_t GetLoopMode(LoopMode &loopMode) override;
    int32_t GetPlaySpeed(PlaybackSpeed &playbackSpeed) override;
    int32_t GetMediaInfoHolder(MediaInfoHolder &mediaInfoHolder) override;
    int32_t Release() override;

private:
    static const int GET_FAILED = -1;
    sptr<IStreamPlayerIpc> proxy_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS

#endif
