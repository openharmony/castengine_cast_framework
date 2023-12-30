/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast session listener implement proxy class.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef CAST_SESSION_LISTENER_IMPL_PROXY_H
#define CAST_SESSION_LISTENER_IMPL_PROXY_H

#include "i_cast_session_listener_impl.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastSessionListenerImplProxy : public IRemoteProxy<ICastSessionListenerImpl> {
public:
    explicit CastSessionListenerImplProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<ICastSessionListenerImpl>(impl)
    {}

    void OnDeviceState(const DeviceStateInfo &stateInfo) override;
    void OnEvent(const EventId &eventId, const std::string &jsonParam) override;
    void OnRemoteCtrlEvent(int eventType, const uint8_t *data, uint32_t len) override;

private:
    static inline BrokerDelegator<CastSessionListenerImplProxy> delegator_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif