/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
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

private:
    static inline BrokerDelegator<CastSessionListenerImplProxy> delegator_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif