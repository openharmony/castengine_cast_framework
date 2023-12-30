/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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