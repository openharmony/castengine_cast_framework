/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast session manager adaptor.
 * Author: zhangge
 * Create: 2022-5-29
 */

#ifndef CAST_SESSION_MANAGER_ADAPTOR_H
#define CAST_SESSION_MANAGER_ADAPTOR_H

#include <memory>
#include <mutex>
#include <set>

#include "cast_engine_common.h"
#include "cast_session_listener_impl_stub.h"
#include "cast_session_manager_service_proxy.h"
#include "i_cast_session_manager_adaptor.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class CastSessionManagerAdaptor : public ICastSessionManagerAdaptor,
    public std::enable_shared_from_this<CastSessionManagerAdaptor> {
public:
    CastSessionManagerAdaptor(sptr<CastSessionManagerServiceProxy> proxy) : proxy_(proxy) {}
    ~CastSessionManagerAdaptor() override;

    int32_t RegisterListener(std::shared_ptr<ICastSessionManagerListener> listener,
        sptr<IRemoteObject::DeathRecipient> deathRecipient) override;
    int32_t UnregisterListener() override;
    int32_t Release() override;
    int32_t SetLocalDevice(const CastLocalDevice &localDevice) override;
    int32_t CreateCastSession(const CastSessionProperty &property, std::shared_ptr<ICastSession> &castSession) override;
    int32_t SetSinkSessionCapacity(int sessionCapacity) override;
    int32_t StartDiscovery(int protocols) override;
    int32_t SetDiscoverable(bool enable) override;
    int32_t StopDiscovery() override;
    int32_t GetCastSession(std::string sessionId, std::shared_ptr<ICastSession> &castSession) override;

private:
    void UnsubscribeDeathRecipient();

    sptr<CastSessionManagerServiceProxy> proxy_;
    std::mutex mutex_;
    wptr<IRemoteObject> remote_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_{ nullptr };
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_SESSION_MANAGER_ADAPTOR_H