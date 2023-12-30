/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
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