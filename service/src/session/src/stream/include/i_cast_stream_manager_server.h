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
 * Description: cast stream manager server interface.
 * Author: zhangjingnan
 * Create: 2023-09-01
 */

#ifndef I_CAST_STREAM_MANAGER_SERVER_H
#define I_CAST_STREAM_MANAGER_SERVER_H

#include "i_stream_player_impl.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class ICastStreamManagerServer {
public:
    virtual ~ICastStreamManagerServer() = default;

    virtual bool NotifyPeerPlayerStatusChanged(const PlayerStates playbackState, bool isPlayWhenReady) = 0;
    virtual bool NotifyPeerPositionChanged(int position, int bufferPosition, int duration) = 0;
    virtual bool NotifyPeerMediaItemChanged(const MediaInfo &mediaInfo) = 0;
    virtual bool NotifyPeerVolumeChanged(int volume, int maxVolume) = 0;
    virtual bool NotifyPeerRepeatModeChanged(const LoopMode loopMode) = 0;
    virtual bool NotifyPeerPlaySpeedChanged(const PlaybackSpeed speed) = 0;
    virtual bool NotifyPeerPlayerError(int errorCode, const std::string &errorMsg) = 0;
    virtual bool NotifyPeerNextRequest() = 0;
    virtual bool NotifyPeerPreviousRequest() = 0;
    virtual bool NotifyPeerSeekDone(int position) = 0;
    virtual bool NotifyPeerEndOfStream(int isLooping) = 0;
    virtual bool NotifyPeerPlayRequest(const MediaInfo &mediaInfo) = 0;
    virtual bool NotifyPeerCreateChannel() = 0;
    virtual void OnEvent(EventId eventId, const std::string &data) = 0;
    virtual void OnRenderReady(bool isReady) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // I_CAST_STREAM_MANAGER_SERVER_H