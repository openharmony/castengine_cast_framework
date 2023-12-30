/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast session service listener implement apis.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef I_CAST_SERVICE_LISTENER_IMPL_H
#define I_CAST_SERVICE_LISTENER_IMPL_H

#include "i_cast_session_manager_listener.h"
#include "iremote_broker.h"
#include "i_cast_session_impl.h"

namespace OHOS {
namespace CastEngine {
class ICastServiceListenerImpl : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.CastEngine.ICastServiceListenerImpl");

    ICastServiceListenerImpl() = default;
    ICastServiceListenerImpl(const ICastServiceListenerImpl &) = delete;
    ICastServiceListenerImpl &operator=(const ICastServiceListenerImpl &) = delete;
    ICastServiceListenerImpl(ICastServiceListenerImpl &&) = delete;
    ICastServiceListenerImpl &operator=(ICastServiceListenerImpl &&) = delete;
    ~ICastServiceListenerImpl() override = default;

    virtual void OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList) = 0;
    virtual void OnDeviceOffline(const std::string &deviceId) = 0;
    virtual void OnSessionCreated(const sptr<ICastSessionImpl> &castSession) = 0;
    virtual void OnServiceDied() = 0;

protected:
    enum {
        ON_DEVICE_FOUND = 1,
        ON_DEVICE_OFFLINE,
        ON_SESSION_CREATE,
        ON_SERVICE_DIE
    };
};
} // namespace CastEngine
} // namespace OHOS
#endif