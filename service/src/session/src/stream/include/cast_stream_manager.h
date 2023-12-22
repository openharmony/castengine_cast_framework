/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Cast stream manager class.
 * Author: huangchanggui
 * Create: 2023-02-01
 */

#ifndef CAST_STREAM_MANAGER_H
#define CAST_STREAM_MANAGER_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "json.hpp"
#include "i_cast_stream_manager.h"
#include "i_cast_stream_manager_calback.h"
#include "cast_stream_common.h"
#include "i_cast_local_file_channel.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using nlohmann::json;

class CastStreamManager : public ICastStreamManager,
    public ICastStreamManagerCallback,
    public std::enable_shared_from_this<CastStreamManager> {
public:
    CastStreamManager(std::shared_ptr<ICastStreamListener> listener, EndType &endType);
    ~CastStreamManager()  override;

    sptr<IStreamPlayerImpl> CreateStreamPlayer(const std::function<void(void)>& releaseCallback) override;
    void ProcessActionsEvent(int event, const std::string &param) override;
    std::shared_ptr<IChannelListener> GetChannelListener() override;
    void AddChannel(std::shared_ptr<Channel> channel) override;
    void RemoveChannel(std::shared_ptr<Channel> channel) override;

    bool RegisterListener(sptr<IStreamPlayerListenerImpl> listener) override;
    bool UnregisterListener() override;
    bool NotifyPeerLoad(const MediaInfo &mediaInfo) override;
    bool NotifyPeerPlay(const MediaInfo &mediaInfo) override;
    bool NotifyPeerPause() override;
    bool NotifyPeerResume() override;
    bool NotifyPeerStop() override;
    bool NotifyPeerNext() override;
    bool NotifyPeerPrevious() override;
    bool NotifyPeerSeek(int position) override;
    bool NotifyPeerSetVolume(int volume) override;
    bool NotifyPeerSetRepeatMode(int mode) override;
    bool NotifyPeerSetSpeed(int speed) override;
    PlayerStates GetPlayerStatus() override;
    int GetPosition() override;
    int GetDuration() override;
    int GetVolume() override;
    int GetMaxVolume() override;
    LoopMode GetLoopMode() override;
    PlaybackSpeed GetPlaySpeed() override;

    bool NotifyPeerPlayerStatusChanged(const PlayerStates playbackState, bool isPlayWhenReady) override;
    bool NotifyPeerPositionChanged(int position, int bufferPosition, int duration) override;
    bool NotifyPeerMediaItemChanged(const MediaInfo &mediaInfo) override;
    bool NotifyPeerVolumeChanged(int volume, int maxVolume) override;
    bool NotifyPeerRepeatModeChanged(const LoopMode loopMode) override;
    bool NotifyPeerPlaySpeedChanged(const PlaybackSpeed speed) override;
    bool NotifyPeerPlayerError(int errorCode, const std::string &errorMsg) override;
    bool NotifyPeerNextRequest() override;
    bool NotifyPeerPreviousRequest() override;
    bool NotifyPeerSeekDone(int position) override;
    bool NotifyPeerEndOfStream(int isLooping) override;
    bool NotifyPeerCreateChannel() override;
    void OnEvent(EventId eventId, const std::string &data) override;
    void OnRenderReady(bool isReady) override;

private:
    const std::string KEY_ACTION = "ACTION";
    const std::string KEY_CALLBACK_ACTION = "CALLBACK_ACTION";
    const std::string KEY_DATA = "DATA";
    const std::string KEY_CURRENT_INDEX = "CURRENT_INDEX";
    const std::string KEY_PROGRESS_INTERVAL = "PROGRESS_INTERVAL";
    const std::string KEY_LIST = "LIST";
    const std::string KEY_MEDIA_ID = "MEDIA_ID";
    const std::string KEY_MEDIA_NAME = "MEDIA_NAME";
    const std::string KEY_MEDIA_URL = "MEDIA_URL";
    const std::string KEY_MEDIA_TYPE = "MEDIA_TYPE";
    const std::string KEY_MEDIA_SIZE = "MEDIA_SIZE";
    const std::string KEY_START_POSITION = "START_POSITION";
    const std::string KEY_DURATION = "DURATION";
    const std::string KEY_CLOSING_CREDITS_POSITION = "CLOSING_CREDITS_POSITION";
    const std::string KEY_ALBUM_COVER_URL = "ALBUM_COVER_URL";
    const std::string KEY_ALBUM_TITLE = "ALBUM_TITLE";
    const std::string KEY_MEDIA_ARTIST = "MEDIA_ARTIST";
    const std::string KEY_LRC_URL = "LRC_URL";
    const std::string KEY_LRC_CONTENT = "LRC_CONTENT";
    const std::string KEY_APP_ICON_URL = "APP_ICON_URL";
    const std::string KEY_APP_NAME = "APP_NAME";
    const std::string KEY_VOLUME = "VOLUME";
    const std::string KEY_MAX_VOLUME = "MAX_VOLUME";
    const std::string KEY_MODE = "MODE";
    const std::string KEY_SPEED = "SPEED";
    const std::string KEY_POSITION = "POSITION";
    const std::string KEY_BUFFER_POSITION = "BUFFER_POSITION";
    const std::string KEY_PLAY_INFO = "PLAY_INFO";
    const std::string KEY_ERROR_CODE = "ERROR_CODE";
    const std::string KEY_ERROR_MSG = "ERROR_MSG";
    const std::string KEY_PLAY_BACK_STATE = "PLAYBACK_STATE";
    const std::string KEY_IS_PLAY_WHEN_READY = "IS_PLAY_WHEN_READY";
    const std::string KEY_IS_LOOPING = "IS_LOOPING";

    const std::string ACTION_PLAY = "play";
    const std::string ACTION_LOAD = "load";
    const std::string ACTION_PAUSE = "pause";
    const std::string ACTION_RESUME = "resume";
    const std::string ACTION_STOP = "stop";
    const std::string ACTION_NEXT = "next";
    const std::string ACTION_PREVIOUS = "previous";
    const std::string ACTION_SEEK = "seek";
    const std::string ACTION_SET_VOLUME = "setVolume";
    const std::string ACTION_SET_REPEAT_MODE = "setRepeatMode";
    const std::string ACTION_SET_SPEED = "setSpeed";
    const std::string ACTION_PLAYER_STATUS_CHANGED = "onPlayerStatusChanged";
    const std::string ACTION_POSITION_CHANGED = "onPositionChanged";
    const std::string ACTION_MEDIA_ITEM_CHANGED = "onMediaItemChanged";
    const std::string ACTION_VOLUME_CHANGED = "onVolumeChanged";
    const std::string ACTION_REPEAT_MODE_CHANGED = "onRepeatModeChanged";
    const std::string ACTION_SPEED_CHANGED = "onPlaySpeedChanged";
    const std::string ACTION_PLAYER_ERROR = "onPlayerError";
    const std::string ACTION_NEXT_REQUEST = "onNextRequest";
    const std::string ACTION_PREVIOUS_REQUEST = "onPreviousRequest";
    const std::string ACTION_SEEK_DONE = "onSeekDone";
    const std::string ACTION_END_OF_STREAM = "onEndOfStream";

    bool ProcessActionLoad(const json &data);
    bool ProcessActionPlay(const json &data);
    bool ProcessActionPause(const json &data);
    bool ProcessActionResume(const json &data);
    bool ProcessActionStop(const json &data);
    bool ProcessActionNext(const json &data);
    bool ProcessActionPrevious(const json &data);
    bool ProcessActionSeek(const json &data);
    bool ProcessActionSetVolume(const json &data);
    bool ProcessActionSetRepeatMode(const json &data);
    bool ProcessActionSetSpeed(const json &data);
    bool ProcessActionPlayerStatusChanged(const json &data);
    bool ProcessActionPositionChanged(const json &data);
    bool ProcessActionMediaItemChanged(const json &data);
    bool ProcessActionVolumeChanged(const json &data);
    bool ProcessActionRepeatModeChanged(const json &data);
    bool ProcessActionSpeedChanged(const json &data);
    bool ProcessActionPlayerError(const json &data);
    bool ProcessActionNextRequest(const json &data);
    bool ProcessActionPreviousRequest(const json &data);
    bool ProcessActionSeekDone(const json &data);
    bool ProcessActionEndOfStream(const json &data);

    bool SendControlAction(const std::string &action, const json &dataBody = {});
    bool SendCallbackAction(const std::string &action, const json &dataBody = {});
    bool ParseMediaInfo(const json &data, MediaInfo &MediaInfo);
    void EncapMediaInfo(const MediaInfo &mediaInfo, json &data);
    sptr<IStreamPlayerListenerImpl> PlayerListenerGetter();
    sptr<IStreamPlayerImpl> PlayerGetter();
    void Handle();

    using StreamActionProcessor = bool (CastStreamManager::*)(const json &data);
    std::map<std::string, StreamActionProcessor> streamActionProcessor_{
        { ACTION_LOAD, &CastStreamManager::ProcessActionLoad },
        { ACTION_PLAY, &CastStreamManager::ProcessActionPlay },
        { ACTION_PAUSE, &CastStreamManager::ProcessActionPause },
        { ACTION_RESUME, &CastStreamManager::ProcessActionResume },
        { ACTION_STOP, &CastStreamManager::ProcessActionStop },
        { ACTION_NEXT, &CastStreamManager::ProcessActionNext },
        { ACTION_PREVIOUS, &CastStreamManager::ProcessActionPrevious },
        { ACTION_SEEK, &CastStreamManager::ProcessActionSeek },
        { ACTION_SET_VOLUME, &CastStreamManager::ProcessActionSetVolume },
        { ACTION_SET_REPEAT_MODE, &CastStreamManager::ProcessActionSetRepeatMode },
        { ACTION_SET_SPEED, &CastStreamManager::ProcessActionSetSpeed },
        { ACTION_PLAYER_STATUS_CHANGED, &CastStreamManager::ProcessActionPlayerStatusChanged },
        { ACTION_POSITION_CHANGED, &CastStreamManager::ProcessActionPositionChanged },
        { ACTION_MEDIA_ITEM_CHANGED, &CastStreamManager::ProcessActionMediaItemChanged },
        { ACTION_VOLUME_CHANGED, &CastStreamManager::ProcessActionVolumeChanged },
        { ACTION_REPEAT_MODE_CHANGED, &CastStreamManager::ProcessActionRepeatModeChanged },
        { ACTION_SPEED_CHANGED, &CastStreamManager::ProcessActionSpeedChanged },
        { ACTION_PLAYER_ERROR, &CastStreamManager::ProcessActionPlayerError },
        { ACTION_NEXT_REQUEST, &CastStreamManager::ProcessActionNextRequest },
        { ACTION_PREVIOUS_REQUEST, &CastStreamManager::ProcessActionPreviousRequest },
        { ACTION_SEEK_DONE, &CastStreamManager::ProcessActionSeekDone },
        { ACTION_END_OF_STREAM, &CastStreamManager::ProcessActionEndOfStream },
    };
    std::queue<std::pair<json, StreamActionProcessor>> workQueue_;
    std::thread handleThread_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> isRunning_{ false };

    std::shared_ptr<ICastLocalFileChannel> localFileChannel_;
    std::shared_ptr<ICastStreamListener> streamListener_;
    sptr<IStreamPlayerListenerImpl> playerListener_;
    sptr<IStreamPlayerImpl> player_;
    std::mutex dataMutex_;
    std::mutex listnerMutex_;
    std::mutex eventMutex_;
    EndType endType_;
    PlayerStates currentState_ = PlayerStates::PLAYER_IDLE;
    int currentPosition_{ CAST_STREAM_INT_INVALID };
    int currentDuration_{ CAST_STREAM_INT_INVALID };
    int currentVolume_{ CAST_STREAM_INT_INVALID };
    int maxVolume_{ CAST_STREAM_INT_INVALID };
    LoopMode currentMode_ = LoopMode::LOOP_MODE_LIST;
    PlaybackSpeed currentSpeed_ = PlaybackSpeed::SPEED_FORWARD_1_00_X;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H