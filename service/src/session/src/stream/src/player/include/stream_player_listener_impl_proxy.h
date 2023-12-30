/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply stream player listener implement proxy class.
 * Author: huangchanggui
 * Create: 2023-01-13
 */

#ifndef STREAM_PLAYER_LISTENER_IMPL_PROXY_H
#define STREAM_PLAYER_LISTENER_IMPL_PROXY_H

#include "i_stream_player_listener_impl.h"
#include "iremote_proxy.h"
#include "pixel_map.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class StreamPlayerListenerImplProxy : public IRemoteProxy<IStreamPlayerListenerImpl> {
public:
    explicit StreamPlayerListenerImplProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IStreamPlayerListenerImpl>(impl) {}

    void OnStateChanged(const PlayerStates playbackState, bool isPlayWhenReady) override;
    void OnPositionChanged(int position, int bufferPosition, int duration) override;
    void OnMediaItemChanged(const MediaInfo &mediaInfo) override;
    void OnVolumeChanged(int volume, int maxVolume) override;
    void OnPlayerError(int errorCode, const std::string &errorMsg) override;
    void OnVideoSizeChanged(int width, int height) override;
    void OnLoopModeChanged(const LoopMode loopMode) override;
    void OnPlaySpeedChanged(const PlaybackSpeed speed)  override;
    void OnNextRequest() override;
    void OnPreviousRequest() override;
    void OnSeekDone(int position) override;
    void OnEndOfStream(int isLooping) override;
    void OnPlayRequest(const MediaInfo &mediaInfo) override;
    void OnImageChanged(std::shared_ptr<Media::PixelMap> pixelMap) override;
    void OnAlbumCoverChanged(std::shared_ptr<Media::PixelMap> pixelMap) override;

private:
    static inline BrokerDelegator<StreamPlayerListenerImplProxy> delegator_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif