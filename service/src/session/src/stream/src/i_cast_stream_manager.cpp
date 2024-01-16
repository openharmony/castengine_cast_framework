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
 * Description: I Cast stream manager definition, which send instructions to the peer and
 * recv instructions from peer, bridges the session and player client.
 * Author: zhangjingnan
 * Create: 2023-08-30
 */

#include "i_cast_stream_manager.h"
#include "cast_engine_log.h"
#include "remote_player_controller.h"
#include "cast_local_file_channel_client.h"
#include "cast_local_file_channel_server.h"
#include "cast_stream_common.h"
#include "cast_stream_player_utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Stream-Manager");

ICastStreamManager::ICastStreamManager()
{
    CLOGD("ICastStreamManager in");
    isRunning_.store(true);
    handleThread_ = std::thread(&ICastStreamManager::Handle, this);
}

ICastStreamManager::~ICastStreamManager()
{
    CLOGD("~ICastStreamManager in");
    streamListener_ = nullptr;
    playerListener_ = nullptr;
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

void ICastStreamManager::Handle()
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
        (work.second)(work.first);
    }
    CLOGD("out");
}

void ICastStreamManager::ProcessActionsEvent(int event, const std::string &param)
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

std::shared_ptr<IChannelListener> ICastStreamManager::GetChannelListener()
{
    CLOGD("GetChannelListener in");
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!localFileChannel_) {
        CLOGE("localFileChannel_ is null");
        return nullptr;
    }
    return localFileChannel_->GetChannelListener();
}

void ICastStreamManager::AddChannel(std::shared_ptr<Channel> channel)
{
    CLOGD("AddChannel in");
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!localFileChannel_) {
        CLOGE("localFileChannel_ is null");
        return;
    }
    localFileChannel_->AddChannel(channel);
}

void ICastStreamManager::RemoveChannel(std::shared_ptr<Channel> channel)
{
    CLOGD("RemoveChannel in");
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!localFileChannel_) {
        CLOGE("localFileChannel_ is null");
        return;
    }
    localFileChannel_->RemoveChannel(channel);
}

void ICastStreamManager::EncapMediaInfo(const MediaInfo &mediaInfo, json &data, bool isDoubleFrame)
{
    data[KEY_MEDIA_ID] = mediaInfo.mediaId;
    data[KEY_MEDIA_NAME] = mediaInfo.mediaName;
    data[KEY_MEDIA_URL] = mediaInfo.mediaUrl;
    data[KEY_MEDIA_TYPE] = mediaInfo.mediaType;
    data[KEY_MEDIA_SIZE] = mediaInfo.mediaSize;
    data[KEY_START_POSITION] = mediaInfo.startPosition;
    data[KEY_DURATION] = mediaInfo.duration;
    data[KEY_CLOSING_CREDITS_POSITION] = mediaInfo.closingCreditsPosition;
    data[KEY_ALBUM_TITLE] = mediaInfo.albumTitle;
    data[KEY_MEDIA_ARTIST] = mediaInfo.mediaArtist;
    data[KEY_APP_NAME] = mediaInfo.appName;
    if (!isDoubleFrame) {
        data[KEY_ALBUM_COVER_URL] = mediaInfo.albumCoverUrl;
        data[KEY_LRC_URL] = mediaInfo.lrcUrl;
        data[KEY_LRC_CONTENT] = mediaInfo.lrcContent;
        data[KEY_APP_ICON_URL] = mediaInfo.appIconUrl;
    }
}

bool ICastStreamManager::ParseMediaInfo(const json &data, MediaInfo &mediaInfo, bool isDoubleFrame)
{
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaId, data, KEY_MEDIA_ID);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaName, data, KEY_MEDIA_NAME);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaType, data, KEY_MEDIA_TYPE);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.albumTitle, data, KEY_ALBUM_TITLE);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaArtist, data, KEY_MEDIA_ARTIST);
    RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.appName, data, KEY_APP_NAME);

    if (!isDoubleFrame) {
        RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.mediaUrl, data, KEY_MEDIA_URL);
        RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.mediaSize, data, KEY_MEDIA_SIZE);
        RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.startPosition, data, KEY_START_POSITION);
        RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.duration, data, KEY_DURATION);
        RETURN_FALSE_IF_PARSE_NUMBER_WRONG(mediaInfo.closingCreditsPosition, data, KEY_CLOSING_CREDITS_POSITION);
        RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.albumCoverUrl, data, KEY_ALBUM_COVER_URL);
        RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.lrcContent, data, KEY_LRC_CONTENT);
        RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.appIconUrl, data, KEY_APP_ICON_URL);
        RETURN_FALSE_IF_PARSE_STRING_WRONG(mediaInfo.lrcUrl, data, KEY_LRC_URL);
    } else {
        mediaInfo.mediaUrl = "DOUBLE_FRAME";
    }
    return true;
}

bool ICastStreamManager::SendControlAction(const std::string &action, const json &dataBody)
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

bool ICastStreamManager::SendCallbackAction(const std::string &action, const json &dataBody)
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

std::string ICastStreamManager::GetStreamPlayerCapability()
{
    CLOGD("GetStreamPlayerCapability in");
    json data;
    data[KEY_PARAMS_STREAM_VOLUME] = CastStreamPlayerUtils::GetVolume();
    data[KEY_MAX_VOLUME] = CastStreamPlayerUtils::GetMaxVolume();
    data[KEY_PARAMS_PLAYER_VERSION_CODE] = CAST_STREAM_INT_INVALID;
    data[KEY_CAPABILITY_SUPPORT_4K] = CAST_STREAM_INT_INVALID;
    data[KEY_CAPABILITY_SUPPORT_DRM] = CAST_STREAM_INT_INVALID;
    data[KEY_CAPABILITY_DRM_PROPERTIES] = CAST_STREAM_INT_INVALID;

    return data.dump();
}

std::string ICastStreamManager::HandleCustomNegotiationParams(const std::string &playerParams)
{
    CLOGI("handleCustomNegotiationParams in, %{public}s", playerParams.c_str());
    json data;
    if (!data.accept(playerParams)) {
        CLOGE("something wrong for the json data!");
        return "";
    }
    data = json::parse(playerParams, nullptr, false);
    std::lock_guard<std::mutex> lock(dataMutex_);
    RETURN_IF_PARSE_WRONG(currentVolume_, data, KEY_PARAMS_STREAM_VOLUME, "", number);
    RETURN_IF_PARSE_WRONG(maxVolume_, data, KEY_MAX_VOLUME, "", number);
    CLOGI("handleCustomNegotiationParams out. currentVolume: %{public}d, maxVolume: %{public}d.", currentVolume_,
        maxVolume_);
    return "";
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS