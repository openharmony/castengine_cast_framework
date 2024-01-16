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
 * Description: Cast stream manager server class.
 * Author: zhangjingnan
 * Create: 2023-08-30
 */

#ifndef CAST_STREAM_MANAGER_SERVER_H
#define CAST_STREAM_MANAGER_SERVER_H

#include <mutex>
#include "json.hpp"
#include "cast_stream_player_manager.h"
#include "i_cast_stream_manager.h"
#include "i_cast_stream_manager_server.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using nlohmann::json;

class CastStreamManagerServer : public ICastStreamManager,
    public ICastStreamManagerServer,
    public std::enable_shared_from_this<CastStreamManagerServer> {
public:
    explicit CastStreamManagerServer(std::shared_ptr<ICastStreamListener> listener);
    ~CastStreamManagerServer() override;

    sptr<IStreamPlayerIpc> CreateStreamPlayer(const std::function<void(void)> &releaseCallback) override;

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
    bool NotifyPeerPlayRequest(const MediaInfo &mediaInfo) override;
    bool NotifyPeerCreateChannel() override;
    void OnEvent(EventId eventId, const std::string &data) override;
    void OnRenderReady(bool isReady) override;

private:
    bool ProcessActionLoad(const json &data);
    bool ProcessActionPlay(const json &data);
    bool ProcessActionPause(const json &data);
    bool ProcessActionResume(const json &data);
    bool ProcessActionStop(const json &data);
    bool ProcessActionNext(const json &data);
    bool ProcessActionPrevious(const json &data);
    bool ProcessActionSeek(const json &data);
    bool ProcessActionFastForward(const json &data);
    bool ProcessActionFastRewind(const json &data);
    bool ProcessActionSetVolume(const json &data);
    bool ProcessActionSetMute(const json &data);
    bool ProcessActionSetRepeatMode(const json &data);
    bool ProcessActionSetSpeed(const json &data);

    std::shared_ptr<CastStreamPlayerManager> PlayerGetter();
    bool ParseMediaInfoHolder(const json &data, MediaInfoHolder &mediaInfoHolder);

    std::shared_ptr<CastStreamPlayerManager> player_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H