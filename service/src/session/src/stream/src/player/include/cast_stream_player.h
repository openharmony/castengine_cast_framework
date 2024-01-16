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
 * Description: supply cast stream player.
 * Author: zhangjingnan
 * Create: 2023-1-11
 */

#ifndef CAST_STREAM_PLAYER_H
#define CAST_STREAM_PLAYER_H

#include <map>
#include <mutex>
#include <condition_variable>
#include "media_errors.h"
#include "audio_system_manager.h"
#include "avmetadatahelper.h"
#include "player.h"
#include "pixel_map.h"
#include "cast_engine_common.h"
#include "cast_stream_common.h"
#include "cast_stream_player_common.h"
#include "i_stream_player_listener_impl.h"
#include "i_cast_stream_manager_server.h"
#include "local_data_source.h"
#include "cast_local_file_channel_client.h"
#include "cast_timer.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastStreamPlayer;

class CastStreamPlayerCallback : public Media::PlayerCallback {
public:
    explicit CastStreamPlayerCallback(std::shared_ptr<ICastStreamManagerServer> callback);
    ~CastStreamPlayerCallback() override;

    bool RegisterListener(sptr<IStreamPlayerListenerImpl> listener);
    bool UnregisterListener();
    void OnError(int32_t errorCode, const std::string &errorMsg) override;
    void OnInfo(Media::PlayerOnInfoType type, int32_t extra, const Media::Format &infoBody) override;
    void SetPlayer(std::shared_ptr<CastStreamPlayer> player);
    void SetSpeedMode(Media::PlaybackRateMode speedMode);
    void SetSwitching();
    PlayerStates GetPlayerStatus();
    Media::PlaybackRateMode GetSpeedMode();
    bool GetSwitching();
    bool IsPaused();
    bool IsNeededToReset();
    void SetPosInfo(int startPos, int endPos);
    void OnMediaItemChanged(const MediaInfo &mediaInfo);
    void OnVolumeChanged(int volume, int maxVolume);
    void OnEvent(EventId eventId, const std::string &data);
    void OnRenderReady(bool isReady);
    void OnPositionChanged(int position, int bufferPosition, int duration);
    void OnPlayerError(int errorCode, const std::string &errorMsg);
    void OnNextRequest();
    void OnPreviousRequest();
    void OnSeekDone(int position);
    void OnLoopModeChanged(const LoopMode loopMode);
    void OnPlayRequest(const MediaInfo &mediaInfo);
    void OnImageChanged(std::shared_ptr<Media::PixelMap> pixelMap);
    void OnAlbumCoverChanged(std::shared_ptr<Media::PixelMap> pixelMap);

private:
    void OnStateChanged(const PlayerStates playbackState, bool isPlayWhenReady);
    void OnBufferChanged(const Media::Format &infoBody);
    void OnVideoSizeChanged(const Media::Format &infoBody);
    void OnPlaySpeedChanged();
    void OnEndOfStream(int isLooping);
    void SetState(PlayerStates state);
    void CheckPosInfo(int curPos);
    void HandleInterruptEvent(const Media::Format &infoBody);
    PlaybackSpeed ConvertMediaSpeedToPlaybackSpeed(Media::PlaybackRateMode speedMode);
    sptr<IStreamPlayerListenerImpl> ListenerGetter();
    bool IsNeedToSyncPos(int position);
    bool AutoSyncPosition();

    std::mutex mutex_;
    std::mutex posMutex_;
    std::mutex listenerMutex_;
    std::shared_ptr<CastTimer> timer_;
    sptr<IStreamPlayerListenerImpl> listener_ = nullptr;
    std::weak_ptr<ICastStreamManagerServer> callback_;
    std::weak_ptr<CastStreamPlayer> player_;
    PlayerStates state_ = PlayerStates::PLAYER_IDLE;
    int switchingCount_ = CAST_STREAM_INT_INIT;
    int curPosition_ = CAST_STREAM_INT_INIT;
    int endPosition_ = CAST_STREAM_INT_INIT;
    Media::PlaybackRateMode speedMode_ = Media::SPEED_FORWARD_1_00_X;
    std::atomic<bool> isRequestingResource_{ false };
};

class CastStreamVolumeCallback : public AudioStandard::VolumeKeyEventCallback,
    public std::enable_shared_from_this<CastStreamVolumeCallback> {
public:
    explicit CastStreamVolumeCallback(std::shared_ptr<CastStreamPlayerCallback> callback) : callback_(callback) {}
    ~CastStreamVolumeCallback() override;

    void SetMaxVolume(int maxVolume);
    void OnVolumeKeyEvent(AudioStandard::VolumeEvent volumeEvent) override;

private:
    std::shared_ptr<CastStreamPlayerCallback> callback_ = nullptr;
    std::mutex mutex_;
    int maxVolume_ = 0;
};

class CastStreamPlayer {
public:
    explicit CastStreamPlayer(std::shared_ptr<CastStreamPlayerCallback> callback,
        std::shared_ptr<CastLocalFileChannelClient> fileChannel);
    ~CastStreamPlayer();

    bool RegisterListener(sptr<IStreamPlayerListenerImpl> listener);
    bool UnregisterListener();
    bool SetSource(const MediaInfo &mediaInfo);
    bool Prepare();
    bool PrepareAsync();
    bool Play();
    bool Pause();
    bool Stop();
    bool Reset();
    bool Seek(int32_t mseconds, Media::PlayerSeekMode mode);
    bool SetVolume(int32_t volume);
    bool SetMute(bool mute);
    bool SetLooping(bool loop);
    bool SetLoopMode(const LoopMode mode);
    bool SetPlaybackSpeed(Media::PlaybackRateMode mode);
    bool SetParameter(const Media::Format &param);
    bool SetVideoSurface(sptr<Surface> surface);
    bool GetVolume(int &volume, int &curMaxVolume);
    bool GetMute();
    bool GetCurrentTime(int32_t &currentTime);
    bool GetVideoTrackInfo(std::vector<Media::Format> &videoTrack);
    bool GetAudioTrackInfo(std::vector<Media::Format> &audioTrack);
    bool GetVideoWidth();
    bool GetVideoHeight();
    int GetDuration();
    LoopMode GetLoopMode();
    bool GetPlaybackSpeed(Media::PlaybackRateMode &mode);
    bool SelectBitRate(uint32_t bitRate);
    bool IsPlaying();
    bool IsLooping();
    void NotifyPlayComplete();

private:
    bool SeekPrepare(int32_t &mseconds, Media::PlayerSeekMode &mode);
    bool Release();
    bool SendInitSysVolume();
    bool GetImageResource();
    bool ProcessAlbumCover(std::shared_ptr<Media::AVSharedMemory> albumCoverMem);
    std::mutex mutex_;
    std::shared_ptr<Media::Player> player_ = nullptr;
    std::shared_ptr<Media::AVMetadataHelper> avMetadataHelper_ = nullptr;
    std::shared_ptr<CastStreamPlayerCallback> callback_ = nullptr;
    std::shared_ptr<CastStreamVolumeCallback> castStreamVolumeCallback_ = nullptr;
    std::shared_ptr<LocalDataSource> dataSource_;
    std::shared_ptr<CastLocalFileChannelClient> fileChannelClient_;
    AudioStandard::AudioSystemManager *audioSystemMgr_ = nullptr;
    LoopMode loopMode_ = LoopMode::LOOP_MODE_LIST;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H