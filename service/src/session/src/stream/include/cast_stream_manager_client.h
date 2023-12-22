/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Cast stream manager client class.
 * Author: zhangjingnan
 * Create: 2023-08-30
 */

#ifndef CAST_STREAM_MANAGER_CLIENT_H
#define CAST_STREAM_MANAGER_CLIENT_H


#include <mutex>
#include "json.hpp"
#include "cast_stream_common.h"
#include "i_cast_stream_manager.h"
#include "i_cast_stream_manager_client.h"
#include "remote_player_controller.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using nlohmann::json;

class CastStreamManagerClient : public ICastStreamManager,
    public ICastStreamManagerClient,
    public std::enable_shared_from_this<CastStreamManagerClient> {
public:
    explicit CastStreamManagerClient(std::shared_ptr<ICastStreamListener> listener);
    ~CastStreamManagerClient() override;

    sptr<IStreamPlayerIpc> CreateStreamPlayer(const std::function<void(void)> &releaseCallback) override;

    bool RegisterListener(sptr<IStreamPlayerListenerImpl> listener) override;
    bool UnregisterListener() override;
    void OnEvent(EventId eventId, const std::string &data) override;
    bool NotifyPeerLoad(const MediaInfo &mediaInfo) override;
    bool NotifyPeerPlay(const MediaInfo &mediaInfo) override;
    bool NotifyPeerPause() override;
    bool NotifyPeerResume() override;
    bool NotifyPeerStop() override;
    bool NotifyPeerNext() override;
    bool NotifyPeerPrevious() override;
    bool NotifyPeerSeek(int position) override;
    bool NotifyPeerFastForward(int delta) override;
    bool NotifyPeerFastRewind(int delta) override;
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

private:
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
    bool ProcessActionPlayRequest(const json &data);

    sptr<IStreamPlayerListenerImpl> PlayerListenerGetter();

    std::shared_ptr<RemotePlayerController> player_;
    std::mutex eventMutex_;
    PlayerStates currentState_ = PlayerStates::PLAYER_IDLE;
    int currentPosition_{ CAST_STREAM_INT_INVALID };
    int currentDuration_{ CAST_STREAM_INT_INVALID };
    LoopMode currentMode_ = LoopMode::LOOP_MODE_LIST;
    PlaybackSpeed currentSpeed_ = PlaybackSpeed::SPEED_FORWARD_1_00_X;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H