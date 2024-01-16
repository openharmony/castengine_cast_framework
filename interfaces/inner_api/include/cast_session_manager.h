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
 * Description: cast session manager apis.
 * Author: zhangge
 * Create: 2022-06-15
 */

#ifndef CAST_SESSION_MANAGER_H
#define CAST_SESSION_MANAGER_H

#include <memory>
#include <mutex>
#include "cast_engine_common.h"
#include "i_cast_session.h"
#include "i_cast_session_manager_adaptor.h"
#include "i_cast_session_manager_listener.h"
#include "iremote_object.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class EXPORT CastSessionManager {
public:
    static CastSessionManager &GetInstance();

    CastSessionManager(const CastSessionManager &) = delete;
    CastSessionManager &operator=(const CastSessionManager &) = delete;
    CastSessionManager(CastSessionManager &&) = delete;
    CastSessionManager &operator=(CastSessionManager &&) = delete;
    ~CastSessionManager() = default;

    int32_t RegisterListener(std::shared_ptr<ICastSessionManagerListener> listener);
    int32_t UnregisterListener();
    int32_t Release();
    int32_t SetLocalDevice(const CastLocalDevice &localDevice);
    int32_t CreateCastSession(const CastSessionProperty &property, std::shared_ptr<ICastSession> &castSession);
    int32_t SetSinkSessionCapacity(int sessionCapacity);
    int32_t StartDiscovery(int protocols);
    int32_t SetDiscoverable(bool enable);
    int32_t StopDiscovery();
    void ReleaseClientResources();

    int32_t GetCastSession(std::string sessionId, std::shared_ptr<ICastSession> &castSession);
private:
    class CastEngineServiceDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        CastEngineServiceDeathRecipient(std::shared_ptr<ICastSessionManagerListener> listener)
            : listener_(listener) {};
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
        std::shared_ptr<ICastSessionManagerListener> GetListener()
        {
            return listener_;
        }

    private:
        std::shared_ptr<ICastSessionManagerListener> listener_;
    };

    CastSessionManager();
    void ReleaseServiceDeathRecipient();
    std::shared_ptr<ICastSessionManagerAdaptor> GetAdaptor();

    std::mutex mutex_;
    std::shared_ptr<ICastSessionManagerAdaptor> adaptor_;
    sptr<CastEngineServiceDeathRecipient> deathRecipient_{ nullptr };
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS

#endif