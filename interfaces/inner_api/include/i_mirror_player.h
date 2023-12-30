/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Cast mirror player interface.
 * Author: zhangjingnan
 * Create: 2023-05-27
 */

#ifndef I_CAST_MIRROR_PLAYER_H
#define I_CAST_MIRROR_PLAYER_H

#include "cast_engine_common.h"
#include "oh_remote_control_event.h"

namespace OHOS {
namespace CastEngine {
class EXPORT IMirrorPlayer {
public:
    IMirrorPlayer() = default;
    IMirrorPlayer(const IMirrorPlayer &) = delete;
    IMirrorPlayer &operator = (const IMirrorPlayer &) = delete;
    IMirrorPlayer(IMirrorPlayer &&) = delete;
    IMirrorPlayer &operator = (IMirrorPlayer &&) = delete;
    virtual ~IMirrorPlayer() = default;

    virtual int32_t Play(const std::string &deviceId) = 0;
    virtual int32_t Pause(const std::string &deviceId) = 0;
    virtual int32_t SetAppInfo(const AppInfo &appInfo) = 0;
    virtual int32_t SetSurface(const std::string &surfaceId) = 0;
    virtual int32_t DeliverInputEvent(OHRemoteControlEvent event) = 0;
    virtual int32_t InjectEvent(const OHRemoteControlEvent &event) = 0;
    virtual int32_t Release() = 0;
    virtual int32_t GetDisplayId(std::string &displayId) = 0;
    virtual int32_t ResizeVirtualScreen(uint32_t width, uint32_t height) = 0;
};
} // namespace CastEngine
} // namespace OHOS

#endif
