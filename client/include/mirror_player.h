/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply Cast mirror player definition.
 * Author: zhangjingnan
 * Create: 2023-05-27
 */

#ifndef MIRROR_PLAYER_H
#define MIRROR_PLAYER_H

#include "cast_engine_common.h"
#include "i_mirror_player.h"
#include "i_mirror_player_impl.h"
#include "oh_remote_control_event.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class MirrorPlayer : public IMirrorPlayer {
public:
    MirrorPlayer(sptr<IMirrorPlayerImpl> proxy) : proxy_(proxy) {};
    ~MirrorPlayer() override;

    int32_t Play(const std::string &deviceId) override;
    int32_t Pause(const std::string &deviceId) override;
    int32_t SetAppInfo(const AppInfo &appInfo) override;
    int32_t SetSurface(const std::string &surfaceId) override;
    int32_t DeliverInputEvent(OHRemoteControlEvent event) override;
    int32_t InjectEvent(const OHRemoteControlEvent &event) override;
    int32_t Release() override;
    int32_t GetDisplayId(std::string &displayId) override;
    int32_t ResizeVirtualScreen(uint32_t width, uint32_t height) override;

private:
    sptr<IMirrorPlayerImpl> proxy_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif