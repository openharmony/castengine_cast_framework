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