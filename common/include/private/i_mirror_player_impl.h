/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply cast mirror player interface.
 * Author: zhangjingnan
 * Create: 2023-05-27
 */

#ifndef I_CAST_MIRROR_PLAYER_IMPL_H
#define I_CAST_MIRROR_PLAYER_IMPL_H

#include <string>

#include "iremote_broker.h"
#include "oh_remote_control_event.h"
#include "surface_utils.h"

namespace OHOS {
namespace CastEngine {
class IMirrorPlayerImpl : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.CastEngine.IMirrorPlayerImpl");

    IMirrorPlayerImpl() = default;
    IMirrorPlayerImpl(const IMirrorPlayerImpl &) = delete;
    IMirrorPlayerImpl &operator=(const IMirrorPlayerImpl &) = delete;
    IMirrorPlayerImpl(IMirrorPlayerImpl &&) = delete;
    IMirrorPlayerImpl &operator=(IMirrorPlayerImpl &&) = delete;
    ~IMirrorPlayerImpl() override = default;

    virtual int32_t Play(const std::string &deviceId) = 0;
    virtual int32_t Pause(const std::string &deviceId) = 0;
    virtual int32_t SetSurface(sptr<IBufferProducer> producer) = 0;
    virtual int32_t SetAppInfo(const AppInfo &appInfo) = 0;
    virtual int32_t DeliverInputEvent(const OHRemoteControlEvent &event) = 0;
    virtual int32_t InjectEvent(const OHRemoteControlEvent &event) = 0;
    virtual int32_t Release() = 0;
    virtual int32_t GetDisplayId(std::string &displayId) = 0;
    virtual int32_t ResizeVirtualScreen(uint32_t width, uint32_t height) = 0;

protected:
    enum {
        PLAY = 1,
        PAUSE,
        SET_SURFACE,
        DELIVER_INPUT_EVENT,
        INJECT_EVENT,
        RELEASE,
        SET_APP_INFO,
        GET_DISPLAYID,
        RESIZE_VIRTUAL_SCREEN,
    };
};
} // namespace CastEngine
} // namespace OHOS
#endif