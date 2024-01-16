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
 * Description: supply mirror player implement proxy
 * Author: zhangjingnan
 * Create: 2023-5-27
 */

#ifndef MIRROR_PLAYER_IMPL_PROXY_H
#define MIRROR_PLAYER_IMPL_PROXY_H

#include "cast_engine_common.h"
#include "i_mirror_player_impl.h"
#include "iremote_proxy.h"
#include "oh_remote_control_event.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class MirrorPlayerImplProxy : public IRemoteProxy<IMirrorPlayerImpl> {
public:
    explicit MirrorPlayerImplProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IMirrorPlayerImpl>(impl) {}
    ~MirrorPlayerImplProxy() override;

    int32_t Play(const std::string &deviceId) override;
    int32_t Pause(const std::string &deviceId) override;
    int32_t SetAppInfo(const AppInfo &appInfo) override;
    int32_t SetSurface(sptr<IBufferProducer> producer) override;
    int32_t DeliverInputEvent(const OHRemoteControlEvent &event) override;
    int32_t InjectEvent(const OHRemoteControlEvent &event) override;
    int32_t Release() override;
    int32_t GetDisplayId(std::string &displayId) override;
    int32_t ResizeVirtualScreen(uint32_t width, uint32_t height) override;

private:
    static inline BrokerDelegator<MirrorPlayerImplProxy> delegator_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_ENGINE_PROXY_H