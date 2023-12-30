/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
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