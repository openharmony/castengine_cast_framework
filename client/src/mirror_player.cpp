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
 * Description: Cast mirror player function realization.
 * Author: zhangjingnan
 * Create: 2023-05-27
 */

#include "mirror_player.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "surface_utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-MirrorPlayer");

MirrorPlayer::~MirrorPlayer()
{
    CLOGI("Stop the client mirror player.");
}

int32_t MirrorPlayer::Play(const std::string &deviceId)
{
    if (deviceId.empty()) {
        CLOGE("The device id is null");
        return ERR_INVALID_PARAM;
    }
    return proxy_ ? proxy_->Play(deviceId) : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::Pause(const std::string &deviceId)
{
    if (deviceId.empty()) {
        CLOGE("The device id is null");
        return ERR_INVALID_PARAM;
    }
    return proxy_ ? proxy_->Pause(deviceId) : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::SetSurface(const std::string &surfaceId)
{
    errno = 0;
    uint64_t surfaceUniqueId = static_cast<uint64_t>(std::strtoll(surfaceId.c_str(), nullptr, 10));
    if (errno == ERANGE) {
        return ERR_INVALID_PARAM;
    }

    sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(surfaceUniqueId);
    if (!surface) {
        return CAST_ENGINE_ERROR;
    }
    sptr<IBufferProducer> producer = surface->GetProducer();
    if (!producer) {
        CLOGE("producer is null");
        return CAST_ENGINE_ERROR;
    }
    return proxy_ ? proxy_->SetSurface(producer) : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::SetAppInfo(const AppInfo &appInfo)
{
    return proxy_ ? proxy_->SetAppInfo(appInfo) : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::DeliverInputEvent(OHRemoteControlEvent event)
{
    return proxy_ ? proxy_->DeliverInputEvent(event) : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::InjectEvent(const OHRemoteControlEvent &event)
{
    return proxy_ ? proxy_->InjectEvent(event) : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::Release()
{
    return proxy_ ? proxy_->Release() : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::GetDisplayId(std::string &displayId)
{
    return proxy_ ? proxy_->GetDisplayId(displayId) : CAST_ENGINE_ERROR;
}

int32_t MirrorPlayer::ResizeVirtualScreen(uint32_t width, uint32_t height)
{
    return proxy_ ? proxy_->ResizeVirtualScreen(width, height) : CAST_ENGINE_ERROR;
}

} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS