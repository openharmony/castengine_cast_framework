/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Cast session listener implement interface.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef I_CAST_SESSION_LISTENER_IMPL_H
#define I_CAST_SESSION_LISTENER_IMPL_H

#include "cast_engine_common.h"
#include "i_cast_session.h"
#include "iremote_broker.h"

namespace OHOS {
namespace CastEngine {
class ICastSessionListenerImpl : public IRemoteBroker, public ICastSessionListener {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.CastEngine.ICastSessionListenerImpl");
    ICastSessionListenerImpl() = default;
    ICastSessionListenerImpl(const ICastSessionListenerImpl &) = delete;
    ICastSessionListenerImpl &operator=(const ICastSessionListenerImpl &) = delete;
    ICastSessionListenerImpl(ICastSessionListenerImpl &&) = delete;
    ICastSessionListenerImpl &operator=(ICastSessionListenerImpl &&) = delete;
    ~ICastSessionListenerImpl() override = default;

protected:
    enum {
        ON_DEVICE_STATE = 1,
        ON_EVENT,
        ON_REMOTE_CTRL_EVENT
    };
};
} // namespace CastEngine
} // namespace OHOS

#endif