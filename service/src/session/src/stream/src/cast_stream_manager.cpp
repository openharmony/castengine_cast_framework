/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Cast streeam manager definition, which send instructions to the peer and
 * recv instructions from peer, bridges the session and player stub.
 * Author: huangchanggui
 * Create: 2023-02-01
 */

#include "cast_stream_manager.h"
#include "cast_engine_log.h"
#include "cast_stream_player_manager.h"
#include "remote_player_controller.h"
#include "cast_local_file_channel_client.h"
#include "cast_local_file_channel_server.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Stream-Manager");

#define RETURN_IF_PARSE_WRONG(value, data, key, ret, jsonType)            \
    do {                                                                  \
        if (!(data).contains(key) || !(data)[key].is_##jsonType()) {      \
            CLOGE("json object does not contains key:%s", (key).c_str()); \
            return ret;                                                   \
        }                                                                 \
        (value) = (data)[key];                                            \
    } while (0)

#define RET_VOID

#define RETURN_FALSE_IF_PARSE_NUMBER_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, false, number)

#define RETURN_FALSE_IF_PARSE_STRING_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, false, string)

#define RETURN_VOID_IF_PARSE_STRING_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, RET_VOID, string)

#define RETURN_FALSE_IF_PARSE_BOOL_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, false, boolean)


std::shared_ptr<ICastStreamManager> ICastStreamManager::GetInstance(std::shared_ptr<ICastStreamListener> listener,
    EndType &endType)
{
    if (!listener) {
        CLOGE("listener is null");
        return nullptr;
    }
    return std::static_pointer_cast<ICastStreamManager>(std::make_shared<CastStreamManager>(listener, endType));
}

CastStreamManager::CastStreamManager(std::shared_ptr<ICastStreamListener> listener, EndType &endType)
    : streamListener_(listener), endType_(endType)
{
    CLOGD("CastStreamManager in");
    isRunning_.store(true);
    handleThread_ = std::thread(&CastStreamManager::Handle, this);
}

CastStreamManager::~CastStreamManager()
{
    CLOGD("~CastStreamManager in");
    streamListener_ = nullptr;
    playerListener_ = nullptr;
    player_ = nullptr;
    isRunning_.store(false);
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        while (!workQueue_.empty()) {
            workQueue_.pop();
        }
        condition_.notify_all();
    }
    if (handleThread_.joinable()) {
        handleThread_.join();
    }
    RemoveChannel(nullptr);
}

sptr<IStreamPlayerImpl> CastStreamManager::CreateStreamPlayer(const std::function<void(void)>& releaseCallback)
{
    auto player = PlayerGetter();
    if (player) {
        return player;
    }
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto callback = shared_from_this();
    if (endType_ == EndType::CAST_SINK) {
        auto fileChannel = std::make_shared<CastLocalFileChannelClient>(callback);
        auto streamPlayer = new CastStreamPlayerManager(callback, fileChannel);
        if (streamPlayer == nullptr) {
            CLOGE("streamPlayer is null");
            return nullptr;
        }
        streamPlayer->SetSessionCallbackForRelease(releaseCallback);
        player_ = streamPlayer;
        localFileChannel_ = fileChannel;
    } else {
        auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
        auto streamController = new RemotePlayerController(callback, fileChannel);
        if (streamController == nullptr) {
            CLOGE("streamController is null");
            return nullptr;
        }
        streamController->SetSessionCallbackForRelease(releaseCallback);
        player_ = streamController;
        localFileChannel_ = fileChannel;
    }
    if (!player_) {
        CLOGE("streamPlayer_ is null");
    }
    return player_;
}

void CastStreamManager::Handle()
{
    CLOGD("in");
    while (isRunning_.load()) {
        std::pair<json, StreamActionProcessor> work;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (workQueue_.empty()) {
                condition_.wait(lock, [this] { return (!this->workQueue_.empty() || !this->isRunning_.load()); });
                if (!isRunning_.load()) {
                    break;
                }
            }
            work = workQueue_.front();
            workQueue_.pop();
        }
        (this->*(work.second))(work.first);
    }
    CLOGD("out");
}

void CastStreamManager::ProcessActionsEvent(int event, const std::string &param)
{
    CLOGD("in");
    json data;
    std::string keyAction;
    if (!data.accept(param)) {
        CLOGE("something wrong for the json data!");
        return;
    }
    data = json::parse(param, nullptr, false);
    if (!data.contains(KEY_DATA)) {
        CLOGE("json object have no data");
        return;
    }
    if (event == MODULE_EVENT_ID_CONTROL_EVENT) {
        keyAction = KEY_ACTION;
    } else {
        keyAction = KEY_CALLBACK_ACTION;
    }
    std::string action;
    RETURN_VOID_IF_PARSE_STRING_WRONG(action, data, keyAction);
    auto iter = streamActionProcessor_.find(action);
    if (iter == streamActionProcessor_.end()) {
        CLOGE("unsupport action %{public}s", action.c_str());
        return;
    }
    std::lock_guard<std::mutex> lock(queueMutex_);
    CLOGI("enqueue action %{public}s", action.c_str());
    workQueue_.push(std::pair<json, StreamActionProcessor> { data[KEY_DATA], iter->second });
    condition_.notify_all();
}

std::shared_ptr<IChannelListener> CastStreamManager::GetChannelListener()
{
    CLOGD("GetChannelListener in");
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!localFileChannel_) {
        CLOGE("localFileChannel_ is null");
        return nullptr;
    }
    return localFileChannel_->GetChannelListener();
}

void CastStreamManager::AddChannel(std::shared_ptr<Channel> channel)
{
    CLOGD("AddChannel in");
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!localFileChannel_) {
        CLOGE("localFileChannel_ is null");
        return;
    }
    localFileChannel_->AddChannel(channel);
}

void CastStreamManager::RemoveChannel(std::shared_ptr<Channel> channel)
{
    CLOGD("RemoveChannel in");
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!localFileChannel_) {
        CLOGE("localFileChannel_ is null");
        return;
    }
    localFileChannel_->RemoveChannel(channel);
}

bool CastStreamManager::RegisterListener(sptr<IStreamPlayerListenerImpl> listener)
{
    std::lock_guard<std::mutex> lock(listnerMutex_);
    playerListener_ = listener;
    return true;
}

bool CastStreamManager::UnregisterListener()
{
    std::lock_guard<std::mutex> lock(listnerMutex_);
    playerListener_ = nullptr;
    return true;
}

bool CastStreamManager::NotifyPeerLoad(const MediaInfo &mediaInfo)
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

bool CastStreamManager::NotifyPeerPlay(const MediaInfo &mediaInfo)
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

bool CastStreamManager::NotifyPeerPause()
{
    CLOGD("NotifyPeerPause in");
    return SendControlAction(ACTION_PAUSE);
}

bool CastStreamManager::NotifyPeerResume()
{
    CLOGD("NotifyPeerResume in");
    return SendControlAction(ACTION_RESUME);
}

bool CastStreamManager::NotifyPeerStop()
{
    CLOGD("NotifyPeerStop in");
    return SendControlAction(ACTION_STOP);
}

bool CastStreamManager::NotifyPeerNext()
{
    CLOGD("NotifyPeerNext in");
    return SendControlAction(ACTION_NEXT);
}

bool CastStreamManager::NotifyPeerPrevious()
{
    CLOGD("NotifyPeerPrevious in");
    return SendControlAction(ACTION_PREVIOUS);
}

bool CastStreamManager::NotifyPeerSeek(int position)
{
    CLOGD("NotifyPeerSeek in");
    json body;
    body[KEY_POSITION] = position;
    return SendControlAction(ACTION_SEEK, body);
}

bool CastStreamManager::NotifyPeerSetVolume(int volume)
{
    CLOGD("NotifyPeerSetVolume in");
    json body;
    body[KEY_VOLUME] = volume;
    return SendControlAction(ACTION_SET_VOLUME, body);
}

bool CastStreamManager::NotifyPeerSetRepeatMode(int mode)
{
    CLOGD("NotifyPeerSetRepeatMode in");
    json body;
    body[KEY_MODE] = mode;
    return SendControlAction(ACTION_SET_REPEAT_MODE, body);
}

bool CastStreamManager::NotifyPeerSetSpeed(int speed)
{
    CLOGD("NotifyPeerSetSpeed in");
    json body;
    body[KEY_SPEED] = speed;
    return SendControlAction(ACTION_SET_SPEED, body);
}

bool CastStreamManager::NotifyPeerNextRequest()
{
    CLOGD("NotifyPeerNextRequest in");
    return SendControlAction(ACTION_NEXT_REQUEST);
}

bool CastStreamManager::NotifyPeerPreviousRequest()
{
    CLOGD("NotifyPeerPreviousRequest in");
    return SendControlAction(ACTION_PREVIOUS_REQUEST);
}

bool CastStreamManager::NotifyPeerSeekDone(int position)
{
    CLOGD("NotifyPeerSeekDone in");
    json body;
    body[KEY_POSITION] = position;
    return SendControlAction(ACTION_SEEK_DONE, body);
}

bool CastStreamManager::NotifyPeerEndOfStream(int isLooping)
{
    CLOGD("NotifyPeerEndOfStream in");
    json body;
    body[KEY_IS_LOOPING] = isLooping;
    return SendControlAction(ACTION_END_OF_STREAM, body);
}

PlayerStates CastStreamManager::GetPlayerStatus()
{
    CLOGD("GetPlayerStatus in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentState_;
}

int CastStreamManager::GetPosition()
{
    CLOGD("GetPosition in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentPosition_;
}

int CastStreamManager::GetDuration()
{
    CLOGD("GetDuration in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentDuration_;
}

int CastStreamManager::GetVolume()
{
    CLOGD("GetVolume in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentVolume_;
}

int CastStreamManager::GetMaxVolume()
{
    CLOGD("GetMaxVolume in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return maxVolume_;
}

LoopMode CastStreamManager::GetLoopMode()
{
    CLOGD("GetLoopMode in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentMode_;
}

PlaybackSpeed CastStreamManager::GetPlaySpeed()
{
    CLOGD("GetPlaySpeed in");
    std::lock_guard<std::mutex> lock(eventMutex_);
    return currentSpeed_;
}

bool CastStreamManager::NotifyPeerPlayerStatusChanged(const PlayerStates playbackState, bool isPlayWhenReady)
{
    CLOGD("NotifyPeerPlayerStatusChanged in");
    json body;
    body[KEY_PLAY_BACK_STATE] = static_cast<int>(playbackState);
    body[KEY_IS_PLAY_WHEN_READY] = isPlayWhenReady;
    return SendCallbackAction(ACTION_PLAYER_STATUS_CHANGED, body);
}

bool CastStreamManager::NotifyPeerPositionChanged(int position, int bufferPosition, int duration)
{
    CLOGD("NotifyPeerPositionChanged in");
    json body;
    body[KEY_POSITION] = position;
    body[KEY_BUFFER_POSITION] = bufferPosition;
    body[KEY_DURATION] = duration;
    return SendCallbackAction(ACTION_POSITION_CHANGED, body);
}

void CastStreamManager::EncapMediaInfo(const MediaInfo &mediaInfo, json &data)
{
    data[KEY_MEDIA_ID] = mediaInfo.mediaId;
    data[KEY_MEDIA_NAME] = mediaInfo.mediaName;
    data[KEY_MEDIA_URL] = mediaInfo.mediaUrl;
    data[KEY_MEDIA_TYPE] = mediaInfo.mediaType;
    data[KEY_MEDIA_SIZE] = mediaInfo.mediaSize;
    data[KEY_START_POSITION] = mediaInfo.startPosition;
    data[KEY_DURATION] = mediaInfo.duration;
    data[KEY_CLOSING_CREDITS_POSITION] = mediaInfo.closingCreditsPosition;
    data[KEY_ALBUM_COVER_URL] = mediaInfo.albumCoverUrl;
    data[KEY_ALBUM_TITLE] = mediaInfo.albumTitle;
    data[KEY_MEDIA_ARTIST] = mediaInfo.mediaArtist;
    data[KEY_LRC_URL] = mediaInfo.lrcUrl;
    data[KEY_LRC_CONTENT] = mediaInfo.lrcContent;
    data[KEY_APP_ICON_URL] = mediaInfo.appIconUrl;
    data[KEY_APP_NAME] = mediaInfo.appName;
}

bool CastStreamManager::ParseMediaInfo(const json &data, MediaInfo &mediaInfo)
{
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaId, data, KEY_MEDIA_ID);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaName, data, KEY_MEDIA_NAME);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaUrl, data, KEY_MEDIA_URL);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaType, data, KEY_MEDIA_TYPE);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.mediaSize, data, KEY_MEDIA_SIZE);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.startPosition, data, KEY_START_POSITION);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.duration, data, KEY_DURATION);
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.closingCreditsPosition, data, KEY_CLOSING_CREDITS_POSITION);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.albumCoverUrl, data, KEY_ALBUM_COVER_URL);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.albumTitle, data, KEY_ALBUM_TITLE);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaArtist, data, KEY_MEDIA_ARTIST);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.lrcUrl, data, KEY_LRC_URL);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.lrcContent, data, KEY_LRC_CONTENT);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.appIconUrl, data, KEY_APP_ICON_URL);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.appName, data, KEY_APP_NAME);
    return true;
}

bool CastStreamManager::NotifyPeerMediaItemChanged(const MediaInfo &mediaInfo)
{
    CLOGD("NotifyPeerMediaItemChanged in");
    json body;
    EncapMediaInfo(mediaInfo, body);
    return SendCallbackAction(ACTION_MEDIA_ITEM_CHANGED, body);
}

bool CastStreamManager::NotifyPeerVolumeChanged(int volume, int maxVolume)
{
    CLOGD("NotifyPeerVolumeChanged in");
    json body;
    body[KEY_VOLUME] = volume;
    body[KEY_MAX_VOLUME] = maxVolume;
    return SendCallbackAction(ACTION_VOLUME_CHANGED, body);
}

bool CastStreamManager::NotifyPeerRepeatModeChanged(const LoopMode mode)
{
    CLOGD("NotifyPeerRepeatModeChanged in");
    json body;
    body[KEY_MODE] = mode;
    return SendCallbackAction(ACTION_REPEAT_MODE_CHANGED, body);
}

bool CastStreamManager::NotifyPeerPlaySpeedChanged(const PlaybackSpeed speed)
{
    CLOGD("NotifyPeerPlaySpeedChanged in");
    json body;
    body[KEY_SPEED] = static_cast<int>(speed);
    return SendCallbackAction(ACTION_SPEED_CHANGED, body);
}

bool CastStreamManager::NotifyPeerPlayerError(int errorCode, const std::string &errorMsg)
{
    CLOGD("NotifyPeerPlayerError in");
    json body;
    body[KEY_ERROR_CODE] = errorCode;
    body[KEY_ERROR_MSG] = errorMsg;
    return SendCallbackAction(ACTION_PLAYER_ERROR, body);
}

bool CastStreamManager::NotifyPeerCreateChannel()
{
    CLOGD("NotifyPeerCreateChannel in");
    if (!streamListener_) {
        CLOGE("streamListener_ is nullptr");
        return false;
    }
    return streamListener_->SendActionToPeers(MODULE_EVENT_ID_CHANNEL_CREATE, "");
}

void CastStreamManager::OnEvent(EventId eventId, const std::string &data)
{
    CLOGD("OnEvent in");
    if (!streamListener_) {
        CLOGE("streamListener_ is nullptr");
        return;
    }
    streamListener_->OnEvent(eventId, data);
}

void CastStreamManager::OnRenderReady(bool isReady)
{
    CLOGD("OnRenderReady in, isReady:%{public}d", isReady);
    if (!streamListener_) {
        CLOGE("streamListener_ is nullptr");
        return;
    }
    streamListener_->OnRenderReady(isReady);
}

bool CastStreamManager::SendControlAction(const std::string &action, const json &dataBody)
{
    if (!streamListener_) {
        CLOGE("streamListener_ is nullptr");
        return false;
    }
    json data;
    data[KEY_ACTION] = action;
    data[KEY_DATA] = dataBody;
    std::string dataStr = data.dump();
    return streamListener_->SendActionToPeers(MODULE_EVENT_ID_CONTROL_EVENT, dataStr);
}

bool CastStreamManager::SendCallbackAction(const std::string &action, const json &dataBody)
{
    if (!streamListener_) {
        CLOGE("streamListener_ is nullptr");
        return false;
    }
    json data;
    data[KEY_CALLBACK_ACTION] = action;
    data[KEY_DATA] = dataBody;
    std::string dataStr = data.dump();
    return streamListener_->SendActionToPeers(MODULE_EVENT_ID_CALLBACK_EVENT, dataStr);
}

sptr<IStreamPlayerImpl> CastStreamManager::PlayerGetter()
{
    std::lock_guard<std::mutex> lock(dataMutex_);
    return player_;
}

sptr<IStreamPlayerListenerImpl> CastStreamManager::PlayerListenerGetter()
{
    std::lock_guard<std::mutex> lock(listnerMutex_);
    return playerListener_;
}

bool CastStreamManager::ProcessActionLoad(const json &data)
{
    CLOGI("in");
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    MediaInfoHolder mediaInfoHolder = MediaInfoHolder{};
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
    return player->Load(mediaInfoHolder.mediaInfoList.front());
}

bool CastStreamManager::ProcessActionPlay(const json &data)
{
    CLOGI("in");
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
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
    return player->Play(mediaInfoHolder.mediaInfoList.front());
}

bool CastStreamManager::ProcessActionPause(const json &data)
{
    CLOGI("in");
    static_cast<void>(data);
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    return player->Pause();
}

bool CastStreamManager::ProcessActionResume(const json &data)
{
    CLOGI("in");
    static_cast<void>(data);
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    return player->Play();
}

bool CastStreamManager::ProcessActionStop(const json &data)
{
    CLOGI("in");
    static_cast<void>(data);
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    return player->Stop();
}

bool CastStreamManager::ProcessActionNext(const json &data)
{
    CLOGI("in");
    static_cast<void>(data);
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    return player->Next();
}

bool CastStreamManager::ProcessActionPrevious(const json &data)
{
    CLOGI("in");
    static_cast<void>(data);
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    return player->Previous();
}

bool CastStreamManager::ProcessActionSeek(const json &data)
{
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    int position;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(position, data, KEY_POSITION);
    CLOGI("position:%{public}d", position);
    return player->Seek(position);
}

bool CastStreamManager::ProcessActionSetVolume(const json &data)
{
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    int volume;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(volume, data, KEY_VOLUME);
    CLOGI("volume:%{public}d", volume);
    return player->SetVolume(volume);
}

bool CastStreamManager::ProcessActionSetRepeatMode(const json &data)
{
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    int mode;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mode, data, KEY_MODE);
    CLOGI("mode:%{public}d", mode);
    return player->SetLoopMode(static_cast<LoopMode>(mode));
}

bool CastStreamManager::ProcessActionSetSpeed(const json &data)
{
    auto player = PlayerGetter();
    if (!player) {
        CLOGE("player is nullptr");
        return false;
    }
    int speed;
    RETURN_FALSE_IF_PARSE_NUMBER_WRONG(speed, data, KEY_SPEED);
    CLOGI("speed:%{public}d", speed);
    return player->SetSpeed(static_cast<PlaybackSpeed>(speed));
}

bool CastStreamManager::ProcessActionPlayerStatusChanged(const json &data)
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

bool CastStreamManager::ProcessActionPositionChanged(const json &data)
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

bool CastStreamManager::ProcessActionMediaItemChanged(const json &data)
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

bool CastStreamManager::ProcessActionVolumeChanged(const json &data)
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

bool CastStreamManager::ProcessActionRepeatModeChanged(const json &data)
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

bool CastStreamManager::ProcessActionSpeedChanged(const json &data)
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

bool CastStreamManager::ProcessActionPlayerError(const json &data)
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

bool CastStreamManager::ProcessActionNextRequest(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    playerListener->OnNextRequest();
    return true;
}

bool CastStreamManager::ProcessActionPreviousRequest(const json &data)
{
    auto playerListener = PlayerListenerGetter();
    if (!playerListener) {
        CLOGE("playerListener is nullptr");
        return false;
    }
    playerListener->OnPreviousRequest();
    return true;
}

bool CastStreamManager::ProcessActionSeekDone(const json &data)
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

bool CastStreamManager::ProcessActionEndOfStream(const json &data)
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
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
