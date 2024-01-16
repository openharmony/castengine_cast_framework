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
 * Description: supply cast session manager service proxy class
 * Author: zhangge
 * Create: 2022-5-29
 */

#ifndef CAST_SESSION_MANAGER_SERVICE_PROXY_H
#define CAST_SESSION_MANAGER_SERVICE_PROXY_H

#include "cast_engine_common.h"
#include "i_cast_session_manager_service.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class CastSessionManagerServiceProxy : public IRemoteProxy<ICastSessionManagerService> {
public:
    explicit CastSessionManagerServiceProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<ICastSessionManagerService>(impl)
    {}

    int32_t RegisterListener(sptr<ICastServiceListenerImpl> listener) override;
    int32_t UnregisterListener() override;
    int32_t Release() override;
    int32_t SetLocalDevice(const CastLocalDevice &localDevice) override;
    int32_t CreateCastSession(const CastSessionProperty &property, sptr<ICastSessionImpl> &castSession) override;
    int32_t SetSinkSessionCapacity(int sessionCapacity) override;
    int32_t StartDiscovery(int protocols) override;
    int32_t SetDiscoverable(bool enable) override;
    int32_t StopDiscovery() override;
    sptr<IRemoteObject> GetSessionManagerService();
    int32_t GetCastSession(std::string sessionId, sptr<ICastSessionImpl> &castSession) override;

private:
    static inline BrokerDelegator<CastSessionManagerServiceProxy> delegator_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS


#endif // CAST_ENGINE_PROXY_H