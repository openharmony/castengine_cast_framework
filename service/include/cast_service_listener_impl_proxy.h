/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast service listener implement proxy class.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef CAST_SERVICE_LISTENER_IMPL_PROXY_H
#define CAST_SERVICE_LISTENER_IMPL_PROXY_H

#include "cast_engine_common.h"
#include "i_cast_service_listener_impl.h"
#include "i_cast_session_impl.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastServiceListenerImplProxy : public IRemoteProxy<ICastServiceListenerImpl> {
public:
    explicit CastServiceListenerImplProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<ICastServiceListenerImpl>(impl)
    {}

    void OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList) override;
    void OnDeviceOffline(const std::string &deviceId) override;
    void OnSessionCreated(const sptr<ICastSessionImpl> &castSession) override;
    void OnServiceDied() override;

private:
    static inline BrokerDelegator<CastServiceListenerImplProxy> delegator_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif