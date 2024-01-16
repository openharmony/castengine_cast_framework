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
 * Description: supply cast session manager service base class.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef I_CAST_SESSION_MANAGER_SERVICE_H
#define I_CAST_SESSION_MANAGER_SERVICE_H

#include "cast_engine_common.h"
#include "i_cast_session_manager_listener.h"
#include "i_cast_session_impl.h"
#include "i_cast_service_listener_impl.h"
#include "iremote_broker.h"

namespace OHOS {
namespace CastEngine {
class ICastSessionManagerService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.CastEngine.ICastSessionManagerService");

    ICastSessionManagerService() = default;
    ICastSessionManagerService(const ICastSessionManagerService &) = delete;
    ICastSessionManagerService &operator=(const ICastSessionManagerService &) = delete;
    ICastSessionManagerService(ICastSessionManagerService &&) = delete;
    ICastSessionManagerService &operator=(ICastSessionManagerService &&) = delete;
    ~ICastSessionManagerService() override = default;

    virtual int32_t RegisterListener(sptr<ICastServiceListenerImpl> listener) = 0;
    virtual int32_t UnregisterListener() = 0;
    virtual int32_t Release() = 0;
    virtual int32_t SetLocalDevice(const CastLocalDevice &localDevice) = 0;
    virtual int32_t CreateCastSession(const CastSessionProperty &property, sptr<ICastSessionImpl> &castSession) = 0;
    virtual int32_t SetSinkSessionCapacity(int sessionCapacity) = 0;
    virtual int32_t StartDiscovery(int protocols) = 0;
    virtual int32_t SetDiscoverable(bool enable) = 0;
    virtual int32_t StopDiscovery() = 0;
    virtual int32_t GetCastSession(std::string sessionId, sptr<ICastSessionImpl> &castSession) = 0;

protected:
    enum {
        REGISTER_LISTENER = 0,
        UNREGISTER_LISTENER,
        RELEASE,
        SET_LOCAL_DEVICE,
        CREATE_CAST_SESSION,
        SET_SINK_SESSION_CAPACITY,
        START_DISCOVERY,
        SET_DISCOVERABLE,
        STOP_DISCOVERY,
        GET_CAST_SESSION
    };
};
} // namespace CastEngine
} // namespace OHOS
#endif