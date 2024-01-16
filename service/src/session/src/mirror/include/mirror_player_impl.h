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
 * Description: Mirror player implement class.
 * Author: zhangjingnan
 * Create: 2023-05-29
 */

#ifndef MIRROR_PLAYER_IMPL_H
#define MIRROR_PLAYER_IMPL_H

#include <mutex>
#include "permission.h"
#include "cast_session_impl.h"
#include "oh_remote_control_event.h"
#include "mirror_player_impl_stub.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class MirrorPlayerImpl : public MirrorPlayerImplStub {
public:
    MirrorPlayerImpl(sptr<CastSessionImpl> session) : session_(session) {}
    ~MirrorPlayerImpl() override;
    int32_t Play(const std::string &deviceId) override;
    int32_t Pause(const std::string &deviceId) override;
    int32_t SetSurface(sptr<IBufferProducer> producer) override;
    int32_t SetAppInfo(const AppInfo &appInfo) override;
    int32_t DeliverInputEvent(const OHRemoteControlEvent &event) override;
    int32_t InjectEvent(const OHRemoteControlEvent &event) override;
    int32_t Release() override;
    int32_t GetDisplayId(std::string &displayId) override;
    int32_t ResizeVirtualScreen(uint32_t width, uint32_t height) override;
private:
    wptr<CastSessionImpl> session_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif