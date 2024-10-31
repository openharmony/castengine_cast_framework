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
 * Description: Cast mirror player interface.
 * Author: zhangjingnan
 * Create: 2023-05-27
 */

#ifndef I_CAST_MIRROR_PLAYER_H
#define I_CAST_MIRROR_PLAYER_H

#include "cast_engine_common.h"
#include "oh_remote_control_event.h"
#include "pixel_map.h"

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
    virtual int32_t SetDpi(uint32_t dpi) { return 0; }
    virtual int32_t SetFps(uint32_t fps) { return 0; }
    virtual int32_t SetBitrate(uint32_t minBitrate, uint32_t defaultBitrate, uint32_t maxBitrate) { return 0; }
    virtual int32_t SetCastRoute(const bool &remote) { return 0; }
    virtual int32_t GetScreenshot(std::shared_ptr<Media::PixelMap> &screenShot) { return 0; }
};
} // namespace CastEngine
} // namespace OHOS

#endif
