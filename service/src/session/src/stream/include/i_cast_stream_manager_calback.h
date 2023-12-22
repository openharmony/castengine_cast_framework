/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: cast stream manager callback interface.
 * Author: huangchanggui
 * Create: 2023-02-01
 */

#ifndef I_CAST_STREAM_MANAGER_CALLBACK_H
#define I_CAST_STREAM_MANAGER_CALLBACK_H

#include "i_stream_player_impl.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class ICastStreamManagerCallback {
public:
    virtual ~ICastStreamManagerCallback() = default;

    virtual bool RegisterListener(sptr<IStreamPlayerListenerImpl> listener) = 0;
    virtual bool UnregisterListener() = 0;
    virtual bool NotifyPeerLoad(const MediaInfo &mediaInfo) = 0;
    virtual bool NotifyPeerPlay(const MediaInfo &mediaInfo) = 0;
    virtual bool NotifyPeerPause() = 0;
    virtual bool NotifyPeerResume() = 0;
    virtual bool NotifyPeerStop() = 0;
    virtual bool NotifyPeerNext() = 0;
    virtual bool NotifyPeerPrevious() = 0;
    virtual bool NotifyPeerSeek(int position) = 0;
    virtual bool NotifyPeerSetVolume(int volume) = 0;
    virtual bool NotifyPeerSetRepeatMode(int mode) = 0;
    virtual bool NotifyPeerSetSpeed(int speed) = 0;
    virtual PlayerStates GetPlayerStatus() = 0;
    virtual int GetPosition() = 0;
    virtual int GetDuration() = 0;
    virtual int GetVolume() = 0;
    virtual int GetMaxVolume() = 0;
    virtual LoopMode GetLoopMode() = 0;
    virtual PlaybackSpeed GetPlaySpeed() = 0;

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
    virtual bool NotifyPeerCreateChannel() = 0;
    virtual void OnEvent(EventId eventId, const std::string &data) = 0;
    virtual void OnRenderReady(bool isReady) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H