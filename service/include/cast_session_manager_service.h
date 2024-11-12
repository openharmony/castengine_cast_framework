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
#include "connection_manager_listener.h"
#include "session.h"
#include "system_ability_load_callback_stub.h"
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
    void OnActive(const SystemAbilityOnDemandReason& activeReason) override;

    int32_t RegisterListener(sptr<ICastServiceListenerImpl> listener) override;
    int32_t UnregisterListener() override;
    int32_t Release() override;
    int32_t SetLocalDevice(const CastLocalDevice &localDevice) override;
    int32_t CreateCastSession(const CastSessionProperty &property, sptr<ICastSessionImpl> &castSession) override;
    int32_t SetSinkSessionCapacity(int sessionCapacity) override;
    int32_t StartDiscovery(int protocols, std::vector<std::string> drmSchemes) override;
    int32_t SetDiscoverable(bool enable) override;
    int32_t StopDiscovery() override;
    int32_t StartDeviceLogging(int32_t fd, uint32_t maxSize) override;
    void ReleaseServiceResources(pid_t pid, uid_t uid);
    int32_t GetCastSession(std::string sessionId, sptr<ICastSessionImpl> &castSession) override;

    bool DestroyCastSession(int32_t sessionId);
    void ReportServiceDieLocked();
    void ReportDeviceFound(const std::vector<std::pair<CastRemoteDevice, bool>> &deviceList);
    void ReportSessionCreate(const sptr<ICastSessionImpl> &castSession);
    void ReportDeviceOffline(const std::string &deviceId);
    bool LoadSinkSA(const std::string &networkId);
    sptr<ICastSessionImpl> GetCastSessionInner(std::string sessionId);

    class CastServiceLoadCallback : public SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;
    };

private:
    class CastEngineClientDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        CastEngineClientDeathRecipient(wptr<CastSessionManagerService> service, pid_t pid, uid_t uid)
            : service_(service), pid_(pid), uid_(uid) {};
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        wptr<CastSessionManagerService> service_;
        pid_t pid_;
        uid_t uid_;
    };

    bool CheckAndWaitDevieManagerServiceInit();

    pid_t myPid_;
    std::shared_mutex mutex_;
    std::map<pid_t, std::pair<sptr<ICastServiceListenerImpl>, uid_t>> listeners_;
    CastLocalDevice localDevice_{};
    ServiceStatus serviceStatus_{ ServiceStatus::DISCONNECTED };
    int sessionCapacity_{ 0 };
    std::map<int32_t, sptr<ICastSessionImpl>> sessionMap_;
    std::atomic<int> sessionIndex_{ 0 };
    std::unordered_map<pid_t, sptr<IRemoteObject::DeathRecipient>> deathRecipientMap_;
    std::atomic<bool> hasServer_{ false };
    std::atomic<bool> isUnloading_{ false };

    void AddClientDeathRecipientLocked(pid_t pid, uid_t uid, sptr<ICastServiceListenerImpl> listener);
    void RemoveClientDeathRecipientLocked(pid_t pid, sptr<ICastServiceListenerImpl> listener);
    bool AddListenerLocked(sptr<ICastServiceListenerImpl> listener);
    int32_t ReleaseLocked();
    int32_t RemoveListenerLocked(pid_t pid);
    void ClearListenersLocked();
    bool HasListenerLocked();
};

class ConnectionManagerListener : public IConnectionManagerListener {
public:
    ConnectionManagerListener(sptr<CastSessionManagerService> service) : service_(service) {}
    int NotifySessionIsReady() override;
    void ReportSessionCreate(int castSessionId) override;
    bool NotifyRemoteDeviceIsReady(int castSessionId, const CastInnerRemoteDevice &device) override;
    void NotifyDeviceIsOffline(const std::string &deviceId) override;
    void GrabDevice(int32_t sessionId) override;
    int32_t GetSessionProtocolType(int sessionId, ProtocolType &protocolType) override;
    int32_t SetSessionProtocolType(int sessionId, ProtocolType protocolType) override;
    bool LoadSinkSA(const std::string &networkId) override;

private:
    wptr<CastSessionManagerService> service_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif