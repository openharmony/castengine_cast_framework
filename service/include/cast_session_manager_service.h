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
 * Description: cast session manager service class
 * Author: zhangge
 * Create: 2022-06-15
 */

#ifndef CAST_SESSION_MANAGER_SERVICE_H
#define CAST_SESSION_MANAGER_SERVICE_H

#include <atomic>
#include <map>
#include <mutex>
#include <shared_mutex>

#include "cast_session_manager_service_stub.h"
#include "session.h"
#include "system_ability.h"

using SharedRLock = std::shared_lock<std::shared_mutex>;
using SharedWLock = std::lock_guard<std::shared_mutex>;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastSessionManagerService : public SystemAbility, public CastSessionManagerServiceStub {
    DECLARE_SYSTEM_ABILITY(CastSessionManagerService);

public:
    CastSessionManagerService(int32_t saId, bool runOnCreate);
    ~CastSessionManagerService() override;

    void OnStart() override;
    void OnStop() override;

    int32_t RegisterListener(sptr<ICastServiceListenerImpl> listener) override;
    int32_t UnregisterListener() override;
    int32_t Release() override;
    int32_t SetLocalDevice(const CastLocalDevice &localDevice) override;
    int32_t CreateCastSession(const CastSessionProperty &property, sptr<ICastSessionImpl> &castSession) override;
    int32_t SetSinkSessionCapacity(int sessionCapacity) override;
    int32_t StartDiscovery(int protocols) override;
    int32_t SetDiscoverable(bool enable) override;
    int32_t StopDiscovery() override;
    void ReleaseServiceResources(pid_t pid);
    int32_t GetCastSession(std::string sessionId, sptr<ICastSessionImpl> &castSession) override;

    bool DestroyCastSession(int32_t sessionId);
    void ReportServiceDieLocked();
    void ReportDeviceFound(const std::vector<CastRemoteDevice> &deviceList);
    void ReportSessionCreate(const sptr<ICastSessionImpl> &castSession);
    void ReportDeviceOffline(const std::string &deviceId);
    sptr<ICastSessionImpl> GetCastSessionInner(std::string sessionId);

private:
    class CastEngineClientDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        CastEngineClientDeathRecipient(wptr<CastSessionManagerService> service, pid_t pid)
            : service_(service), pid_(pid) {};
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        wptr<CastSessionManagerService> service_;
        pid_t pid_;
    };

    pid_t myPid_;
    std::shared_mutex mutex_;
    std::vector<std::pair<pid_t, sptr<ICastServiceListenerImpl>>> listeners_;
    CastLocalDevice localDevice_{};
    ServiceStatus serviceStatus_{ ServiceStatus::DISCONNECTED };
    int sessionCapacity_{ 0 };
    std::map<int32_t, sptr<ICastSessionImpl>> sessionMap_;
    std::atomic<int> sessionIndex_{ 0 };
    std::unordered_map<pid_t, sptr<IRemoteObject::DeathRecipient>> deathRecipientMap_;
    std::atomic<bool> hasServer_{ false };

    void AddClientDeathRecipientLocked(pid_t pid, sptr<ICastServiceListenerImpl> listener);
    void RemoveClientDeathRecipientLocked(pid_t pid, sptr<ICastServiceListenerImpl> listener);
    bool AddListenerLocked(sptr<ICastServiceListenerImpl> listener);
    int32_t ReleaseLocked();
    int32_t RemoveListenerLocked(pid_t pid);
    void ClearListenersLocked();
    bool HasListenerLocked();
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif