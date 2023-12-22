/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Cast stream manager client definition, which processes and sends client's instructions.
 * Author: zhangjingnan
 * Create: 2023-08-30
 */

#include "cast_stream_manager_client.h"
#include "cast_engine_log.h"
#include "remote_player_controller.h"
#include "cast_local_file_channel_server.h"
#include "i_stream_player_ipc.h"
#include "stream_player_impl_stub.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Stream-Manager-Client");

CastStreamManagerClient::CastStreamManagerClient(std::shared_ptr<ICastStreamListener> listener)
{
    CLOGD("CastStreamManagerClient in");
    streamActionProcessor_ = {
        { ACTION_PLAYER_STATUS_CHANGED, [this](const json &data) { return ProcessActionPlayerStatusChanged(data); } },
        { ACTION_POSITION_CHANGED, [this](const json &data) { return ProcessActionPositionChanged(data); } },
        { ACTION_MEDIA_ITEM_CHANGED, [this](const json &data) { return ProcessActionMediaItemChanged(data); } },
        { ACTION_VOLUME_CHANGED, [this](const json &data) { return ProcessActionVolumeChanged(data); } },
        { ACTION_REPEAT_MODE_CHANGED, [this](const json &data) { return ProcessActionRepeatModeChanged(data); } },
        { ACTION_SPEED_CHANGED, [this](const json &data) { return ProcessActionSpeedChanged(data); } },
        { ACTION_PLAYER_ERROR, [this](const json &data) { return ProcessActionPlayerError(data); } },
        { ACTION_NEXT_REQUEST, [this](const json &data) { return ProcessActionNextRequest(data); } },
        { ACTION_PREVIOUS_REQUEST, [this](const json &data) { return ProcessActionPreviousRequest(data); } },
        { ACTION_SEEK_DONE, [this](const json &data) { return ProcessActionSeekDone(data); } },
        { ACTION_END_OF_STREAM, [this](const json &data) { return ProcessActionEndOfStream(data); } },
        { ACTION_PLAY_REQUEST, [this](const json &data) { return ProcessActionPlayRequest(data); } }
    };
    streamListener_ = listener;
}

CastStreamManagerClient::~CastStreamManagerClient()
{
    CLOGD("~CastStreamManagerClient in");
    player_ = nullptr;
}

sptr<IStreamPlayerIpc> CastStreamManagerClient::CreateStreamPlayer(const std::function<void(void)> &releaseCallback)
{
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (playerIpc_) {
        return playerIpc_;
    }
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto player = std::make_shared<RemotePlayerController>(shared_from_this(), fileChannel);
    if (player == nullptr) {
        CLOGE("streamController is null");
        return nullptr;
    }
    player->SetSessionCallbackForRelease(releaseCallback);
    auto streamPlayer = new StreamPlayerImplStub(player);
    if (streamPlayer == nullptr) {
        CLOGE("streamPlayer is null");
        return nullptr;
    }
    localFileChannel_ = fileChannel;
    player_ = player;
    playerIpc_ = streamPlayer;
    return streamPlayer;
}

bool CastStreamManagerClient::RegisterListener(sptr<IStreamPlayerListenerImpl> listener)
{
    std::lock_guard<std::mutex> lock(listenerMutex_);
    playerListener_ = listener;
    return true;
}

bool CastStreamManagerClient::UnregisterListener()
{
    std::lock_guard<std::mutex> lock(listenerMutex_);
    playerListener_ = nullptr;
    return true;
}

bool CastStreamManagerClient::NotifyPeerLoad(const MediaInfo &mediaInfo)
{
    CLOGD("NotifyPeerLoad in");
    json body;
    json list;
    body[KEY_CURRENT_INDEX] = 0;
    body[KEY_PROGRESS_INTERVAL] = 0;
    size_t mediaInfoListSize = 1;
    for (size_t i = 0; i < mediaInfoListSize; i++) {
        json info;
        EncapMediaInfo(mediaInfo, info);
        list[i] = info;
    }
    body[KEY_LIST] = list;
    CLOGD("list size:%{public}zu ", list.size());
    return SendControlAction(ACTION_LOAD, body);
}

bool CastStreamManagerClient::NotifyPeerPlay(const MediaInfo &mediaInfo)
{
    CLOGD("NotifyPeerPlay in");
    json body;
    json list;
    body[KEY_CURRENT_INDEX] = 0;
    body[KEY_PROGRESS_INTERVAL] = 0;
    size_t mediaInfoListSize = 1;
    for (size_t i = 0; i < mediaInfoListSize; i++) {
        json info;
        EncapMediaInfo(mediaInfo, info);
        list[i] = info;
    }
    body[KEY_LIST] = list;
    CLOGD("list size:%{public}zu ", list.size());
    return SendControlAction(ACTION_PLAY, body);
}

bool CastStreamManagerClient::NotifyPeerPause()
{
    CLOGD("NotifyPeerPause in");
    return SendControlAction(ACTION_PAUSE);
}

bool CastStreamManagerClient::NotifyPeerResume()
{
    CLOGD("NotifyPeerResume in");
    return SendControlAction(ACTION_RESUME);
}

bool CastStreamManagerClient::NotifyPeerStop()
{
    CLOGD("NotifyPeerStop in");
    return SendControlAction(ACTION_STOP);
}

bool CastStreamManagerClient::NotifyPeerNext()
{
    CLOGD("NotifyPeerNext in");
    return SendControlAction(ACTION_NEXT);
}

bool CastStreamManagerClient::NotifyPeerPrevious()
{
    CLOGD("NotifyPeerPrevious in");
    return SendControlAction(ACTION_PREVIOUS);
}

bool CastStreamManagerClient::NotifyPeerSeek(int position)
{
    CLOGD("NotifyPeerSeek in");
    json body;
    body[KEY_POSITION] = position;
    return SendControlAction(ACTION_SEEK, body);
}

bool CastStreamManagerClient::NotifyPeerFastForward(int delta)
{
    CLOGD("NotifyPeerFastForward in");
    json body;
    body[KEY_DELTA] = delta;
    return SendControlAction(ACTION_FAST_FORWARD, body);
}

bool CastStreamManagerClient::NotifyPeerFastRewind(int delta)
{
    CLOGD("NotifyPeerFastRewind in");
    json body;
    body[KEY_DELTA] = delta;
    return SendControlAction(ACTION_FAST_REWIND, body);
}

bool CastStreamManagerClient::NotifyPeerSetVolume(int volume)
{
    CLOGD("NotifyPeerSetVolume in");
    json body;
    body[KEY_VOLUME] = volume;
    return SendControlAction(ACTION_SET_VOLUME, body);
}

bool CastStreamManagerClient::NotifyPeerSetRepeatMode(int mode)
{
    CLOGD("NotifyPeerSetRepeatMode in");
    json body;
    body[KEY_MODE] = mode;
    return SendControlAction(ACTION_SET_REPEAT_MODE, body);
}

bool CastStreamManagerClient::NotifyPeerSetSpeed(int speed)
{
    CLOGD("NotifyPeerSetSpeed in");
    json body;
    body[KEY_SPEED] = speed;
    return SendControlAction(ACTION_SET_SPEED, body);
}

PlayerStates CastStreamManagerClient::GetPlayerStatus()
{
    CLOGD("GetPlayerStatus in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentState_;
}

int CastStreamManagerClient::GetPosition()
{
    CLOGD("GetPosition in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentPosition_;
}

int CastStreamManagerClient::GetDuration()
{
    CLOGD("GetDuration in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentDuration_;
}

int CastStreamManagerClient::GetVolume()
{
    CLOGD("GetVolume in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentVolume_;
}

int CastStreamManagerClient::GetMaxVolume()
{
    CLOGD("GetMaxVolume in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return maxVolume_;
}

LoopMode CastStreamManagerClient::GetLoopMode()
{
    CLOGD("GetLoopMode in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentMode_;
}

PlaybackSpeed CastStreamManagerClient::GetPlaySpeed()
{
    CLOGD("GetPlaySpeed in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentSpeed_;
}

void CastStreamManagerClient::OnEvent(EventId eventId, const std::string &data)
{
    CLOGD("OnEvent in");
    if (!streamListener_) {
        CLOGE("streamListener_ is nullptr");
        return;
    }
    streamListener_->OnEvent(eventId, data);
}

sptr<IStreamPlayerListenerImpl> CastStreamManagerClient::PlayerListenerGetter()
{
    std::lock_guard<std::mutex> lock(listenerMutex_);
    return playerListener_;
}

bool CastStreamManagerClient::ProcessActionPlayerStatusChanged(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int state;
    bool isPlayWhenReady = false;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(state, data, KEY_PLAY_BACK_STATE);
    RETURN_FALSE_IF_PARSE_BOOL_WRONG(isPlayWhenReady, data, KEY_IS_PLAY_WHEN_READY);
    auto playbackState = static_cast<PlayerStates>(state);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentState_ = playbackState;
    }
    CLOGI("playbackState:%{public}d  isPlayWhenReady:%{public}d", state, isPlayWhenReady);
    playerListener->OnStateChanged(playbackState, isPlayWhenReady);
    return true;
}

bool CastStreamManagerClient::ProcessActionPositionChanged(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int position;
    int bufferPosition;
    int duration;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(position, data, KEY_POSITION);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(bufferPosition, data, KEY_BUFFER_POSITION);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(duration, data, KEY_DURATION);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentPosition_ = position;
        currentDuration_ = duration;
    }
    CLOGI("position:%{public}d  bufferPosition:%{public}d duration:%{public}d", position, bufferPosition, duration);
    playerListener->OnPositionChanged(position, bufferPosition, duration);
    return true;
}

bool CastStreamManagerClient::ProcessActionMediaItemChanged(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    MediaInfo mediaInfo = MediaInfo();
    if (!ParseMediaInfo(data, mediaInfo)) {
        return false;
    }
    playerListener->OnMediaItemChanged(mediaInfo);
    return true;
}

bool CastStreamManagerClient::ProcessActionVolumeChanged(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int volume;
    int maxVolume;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(volume, data, KEY_VOLUME);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(maxVolume, data, KEY_MAX_VOLUME);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentVolume_ = volume;
        maxVolume_ = maxVolume;
    }
    CLOGI("volume:%{public}d, maxVolume:%{public}d", volume, maxVolume);
    playerListener->OnVolumeChanged(volume, maxVolume);
    return true;
}

bool CastStreamManagerClient::ProcessActionRepeatModeChanged(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int mode;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mode, data, KEY_MODE);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentMode_ = static_cast<LoopMode>(mode);
    }
    CLOGI("mode:%{public}d", mode);
    playerListener->OnLoopModeChanged(static_cast<LoopMode>(mode));
    return true;
}

bool CastStreamManagerClient::ProcessActionSpeedChanged(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int speed;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(speed, data, KEY_SPEED);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        currentSpeed_ = static_cast<PlaybackSpeed>(speed);
    }
    CLOGI("speed:%{public}d", speed);
    playerListener->OnPlaySpeedChanged(static_cast<PlaybackSpeed>(speed));
    return true;
}

bool CastStreamManagerClient::ProcessActionPlayerError(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int errorCode;
    std::string errorMsg;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(errorCode, data, KEY_ERROR_CODE);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(errorMsg, data, KEY_ERROR_MSG);
    CLOGI("errorCode:%{public}d errorMsg:%{public}s", errorCode, errorMsg.c_str());
    playerListener->OnPlayerError(errorCode, errorMsg);
    return true;
}

bool CastStreamManagerClient::ProcessActionNextRequest(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    playerListener->OnNextRequest();
    return true;
}

bool CastStreamManagerClient::ProcessActionPreviousRequest(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    playerListener->OnPreviousRequest();
    return true;
}

bool CastStreamManagerClient::ProcessActionSeekDone(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int position;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(position, data, KEY_POSITION);
    CLOGI("position:%{public}d", position);
    playerListener->OnSeekDone(position);
    return true;
}

bool CastStreamManagerClient::ProcessActionEndOfStream(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    int isLooping;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(isLooping, data, KEY_IS_LOOPING);
    CLOGI("isLooping:%{public}d", isLooping);
    playerListener->OnEndOfStream(isLooping);
    return true;
}

bool CastStreamManagerClient::ProcessActionPlayRequest(const json &data)
{
    MediaInfoHolder mediaInfoHolder = MediaInfoHolder();
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfoHolder.currentIndex, data, KEY_CURRENT_INDEX);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfoHolder.progressRefreshInterval, data, KEY_PROGRESS_INTERVAL);
    if (!data.contains(KEY_LIST)) {
        CLOGE("json object have no mediaInfo list");
        return false;
    }
    json list = data[KEY_LIST];
    for (size_t i = 0; i < list.size(); i++) {
        MediaInfo mediaInfo = MediaInfo();
        json info = list[i];
        if (!ParseMediaInfo(info, mediaInfo)) {
            return false;
        }
        mediaInfoHolder.mediaInfoList.push_back(mediaInfo);
    }

    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    playerListener->OnPlayRequest(mediaInfoHolder.mediaInfoList.front());
    return true;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS