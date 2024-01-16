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
 * Description: supply cast session manager base adaptor
 * Author: zhangge
 * Create: 2022-5-29
 */

#ifndef I_CAST_SESSION_MANAGER_ADAPTOR_H
#define I_CAST_SESSION_MANAGER_ADAPTOR_H

#include <memory>

#include "cast_engine_common.h"
#include "i_cast_session.h"
#include "i_cast_session_manager_listener.h"
#include "iremote_object.h"

namespace OHOS {
namespace CastEngine {
class EXPORT ICastSessionManagerAdaptor {
public:
    ICastSessionManagerAdaptor() = default;
    ICastSessionManagerAdaptor(const ICastSessionManagerAdaptor &) = delete;
    ICastSessionManagerAdaptor &operator = (const ICastSessionManagerAdaptor &) = delete;
    ICastSessionManagerAdaptor(ICastSessionManagerAdaptor &&) = delete;
    ICastSessionManagerAdaptor &operator = (ICastSessionManagerAdaptor &&) = delete;
    virtual ~ICastSessionManagerAdaptor() = default;

    virtual int32_t RegisterListener(std::shared_ptr<ICastSessionManagerListener> listener,
        sptr<IRemoteObject::DeathRecipient> deathRecipient) = 0;
    virtual int32_t UnregisterListener() = 0;
    virtual int32_t Release() = 0;
    virtual int32_t SetLocalDevice(const CastLocalDevice &localDevice) = 0;
    virtual int32_t CreateCastSession(const CastSessionProperty &property,
        std::shared_ptr<ICastSession> &castSession) = 0;
    virtual int32_t SetSinkSessionCapacity(int sessionCapacity) = 0;
    virtual int32_t StartDiscovery(int protocols) = 0;
    virtual int32_t SetDiscoverable(bool enable) = 0;
    virtual int32_t StopDiscovery() = 0;
    virtual int32_t GetCastSession(std::string sessionId, std::shared_ptr<ICastSession> &castSession) = 0;
};
} // namespace CastEngine
} // namespace OHOS

#endif // I_CAST_SESSION_MANAGER_ADAPTOR_H