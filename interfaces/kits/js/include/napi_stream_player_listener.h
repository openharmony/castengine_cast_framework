/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: supply stream player listener for napi interface.
 * Author: huangchanggui
 * Create: 2023-1-11
 */

#ifndef NAPI_STREAM_PLAYER_LISTENER_H
#define NAPI_STREAM_PLAYER_LISTENER_H

#include <memory>
#include <list>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_common.h"
#include "napi_callback.h"
#include "napi_castengine_utils.h"
#include "i_stream_player.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class NapiStreamPlayerListener : public IStreamPlayerListener {
public:
    using NapiArgsGetter = std::function<void(napi_env env, int &argc, napi_value *argv)>;
    enum {
        EVENT_PLAYER_STATUS_CHANGED,
        EVENT_POSITION_CHANGED,
        EVENT_MEDIA_ITEM_CHANGED,
        EVENT_VOLUME_CHANGED,
        EVENT_VIDEO_SIZE_CHANGED,
        EVENT_LOOP_MODE_CHANGED,
        EVENT_PLAY_SPEED_CHANGED,
        EVENT_PLAYER_ERROR,
        EVENT_NEXT_REQUEST,
        EVENT_PREVIOUS_REQUEST,
        EVENT_SEEK_DONE,
        EVENT_END_OF_STREAM,
        EVENT_PLAY_REQUEST,
        EVENT_TYPE_MAX
    };

    NapiStreamPlayerListener() {};
    ~NapiStreamPlayerListener() override;

    void OnStateChanged(const PlayerStates playbackState, bool isPlayWhenReady) override;
    void OnPositionChanged(int position, int bufferPosition, int duration) override;
    void OnMediaItemChanged(const MediaInfo &mediaInfo) override;
    void OnVolumeChanged(int volume, int maxVolume) override;
    void OnVideoSizeChanged(int width, int height) override;
    void OnLoopModeChanged(const LoopMode loopMode) override;
    void OnPlaySpeedChanged(const PlaybackSpeed speed) override;
    void OnPlayerError(int errorCode, const std::string &errorMsg) override;
    void OnNextRequest() override;
    void OnPreviousRequest() override;
    void OnSeekDone(int position) override;
    void OnEndOfStream(int isLooping) override;
    void OnPlayRequest(const MediaInfo &mediaInfo) override;

    napi_status AddCallback(napi_env env, int32_t event, napi_value callback);
    napi_status RemoveCallback(napi_env env, int32_t event, napi_value callback);

private:
    void HandleEvent(int32_t event, NapiArgsGetter getter);
    napi_status ClearCallback(napi_env env);

    std::mutex lock_;
    std::shared_ptr<NapiCallback> callback_;
    std::list<napi_ref> callbacks_[EVENT_TYPE_MAX] {};
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif