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
 * Description: define cast source discovery.
 * Author: zhangge
 * Create: 2022-08-06
 */

#ifndef DISCOVERY_MANAGER_H
#define DISCOVERY_MANAGER_H

#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <cstring>

#include "cast_engine_common.h"
#include "cast_service_common.h"
#include "device_manager.h"
#include "event_handler.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using OHOS::DistributedHardware::DiscoveryCallback;
using OHOS::DistributedHardware::DmDeviceInfo;
using OHOS::DistributedHardware::DmInitCallback;
using OHOS::DistributedHardware::PublishCallback;
using namespace OHOS::AppExecFwk;

class CastDmInitCallback : public DmInitCallback {
public:
    void OnRemoteDied() override;
};

class IDiscoveryManagerListener {
public:
    IDiscoveryManagerListener() = default;
    IDiscoveryManagerListener(const IDiscoveryManagerListener &) = delete;
    IDiscoveryManagerListener &operator=(const IDiscoveryManagerListener &) = delete;
    IDiscoveryManagerListener(IDiscoveryManagerListener &&) = delete;
    IDiscoveryManagerListener &operator=(IDiscoveryManagerListener &&) = delete;
    virtual ~IDiscoveryManagerListener() = default;

    virtual void OnDeviceFound(const std::vector<CastInnerRemoteDevice> &devices) = 0;
};

class CastDiscoveryCallback : public DiscoveryCallback {
public:
    virtual void OnDiscoverySuccess(uint16_t subscribeId) override;
    virtual void OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason) override;
    virtual void OnDeviceFound(uint16_t subscribeId, const DmDeviceInfo &deviceInfo) override;
};

class CastPublishDiscoveryCallback : public PublishCallback {
public:
    virtual void OnPublishResult(int32_t publishId, int32_t publishResult) override;
};

class DiscoveryManager;

class DiscoveryEventHandler : public OHOS::AppExecFwk::EventHandler {
public:
    explicit DiscoveryEventHandler(const std::shared_ptr<OHOS::AppExecFwk::EventRunner> &runner)
        : EventHandler(runner)
    {
        scanCount = 0;
    };
protected:
    void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer &event) override;

private:
    int scanCount;
};

class DiscoveryManager {
    friend class DiscoveryEventHandler;
public:
    static DiscoveryManager &GetInstance();

    void Init(std::shared_ptr<IDiscoveryManagerListener> listener);
    void Deinit();

    void StartDiscovery();
    void StopDiscovery();

    bool StartAdvertise();
    bool StopAdvertise();

    int GetProtocolType() const;
    void SetProtocolType(int protocols);

    void OnDeviceInfoFound(uint16_t subscribeId, const DmDeviceInfo &dmDeviceInfo);
    void NotifyDeviceIsFound(const CastInnerRemoteDevice &newDevice);
    void NotifyDeviceIsOnline(const DmDeviceInfo &dmDeviceInfo);

private:
    void StartDmDiscovery();
    void StopDmDiscovery();
    void GetAndReportTrustedDevices();
    void ParseDeviceInfo(const DmDeviceInfo &dmDevice, CastInnerRemoteDevice &castDevice);

    void SetListener(std::shared_ptr<IDiscoveryManagerListener> listener);
    bool HasListener();
    void ResetListener();
    void RemoveSameDeviceLocked(const CastInnerRemoteDevice &newDevice);
    CastInnerRemoteDevice CreateRemoteDevice(const DmDeviceInfo &dmDeviceInfo);
    void UpdateDeviceState();

    std::mutex mutex_;
    int protocolType_ = 0;
    int32_t uid_{ 0 };
    std::shared_ptr<IDiscoveryManagerListener> listener_;
    std::shared_ptr<EventRunner> eventRunner_;
    std::shared_ptr<DiscoveryEventHandler> eventHandler_;
    std::unordered_map<CastInnerRemoteDevice, int> remoteDeviceMap;
    int32_t scanCount;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // DISCOVERY_MANAGER_H
