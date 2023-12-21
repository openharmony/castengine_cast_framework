/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: define cast source discovery.
 * Author: zhangge
 * Create: 2022-08-06
 */

#ifndef DISCOVERY_MANAGER_H
#define DISCOVERY_MANAGER_H

#include <map>
#include <mutex>
#include <string>

#include "cast_engine_common.h"
#include "cast_service_common.h"
#include "device_manager.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using OHOS::DistributedHardware::DiscoveryCallback;
using OHOS::DistributedHardware::DmDeviceInfo;
using OHOS::DistributedHardware::DmInitCallback;

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

class DiscoveryManager {
public:
    static DiscoveryManager &GetInstance();

    void Init(std::shared_ptr<IDiscoveryManagerListener> listener);
    void Deinit();

    void StartDiscovery();
    void StopDiscovery();

    void OnDeviceInfoFound(uint16_t subscribeId, const DmDeviceInfo &dmDeviceInfo);
    void NotifyDeviceIsFound(const std::vector<DmDeviceInfo> &dmDevices, bool isTrusted, bool isOnline);

private:
    void StartDmDiscovery();
    void GetAndReportTrustedDevices();

    void SetListener(std::shared_ptr<IDiscoveryManagerListener> listener);
    bool HasListener();
    void ResetListener();

    std::mutex mutex_;
    std::shared_ptr<IDiscoveryManagerListener> listener_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // DISCOVERY_MANAGER_H
