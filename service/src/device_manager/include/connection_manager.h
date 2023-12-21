/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: implement the cast source connect
 * Author: zhangge
 * Create: 2022-08-23
 */

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <map>
#include <mutex>
#include <optional>
#include <string>

#include "cast_device_data_manager.h"
#include "cast_engine_common.h"
#include "cast_engine_log.h"
#include "cast_service_common.h"
#include "device_manager.h"
#include "session.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using OHOS::DistributedHardware::AuthenticateCallback;
using OHOS::DistributedHardware::DeviceStateCallback;
using OHOS::DistributedHardware::DmDeviceInfo;

class CastAuthenticateCallback : public AuthenticateCallback {
public:
    void OnAuthResult(const std::string &deviceId, const std::string &token, int32_t status, int32_t reason) override;
};

class IConnectionManagerListener {
public:
    IConnectionManagerListener() = default;
    virtual ~IConnectionManagerListener() = default;

    virtual int NotifySessionIsReady() = 0;
    virtual void NotifyDeviceIsOffline(const std::string &deviceId) = 0;
    virtual bool NotifyRemoteDeviceIsReady(int castSessionId, const CastInnerRemoteDevice &device) = 0;
    virtual void OnError(const std::string &deviceId) = 0;
};

class ConnectionManager {
public:
    static ConnectionManager &GetInstance();

    void Init(std::shared_ptr<IConnectionManagerListener> listener);
    void Deinit();

    bool IsDeviceTrusted(const std::string &deviceId, std::string &networkId);
    bool EnableDiscoverable();
    bool DisableDiscoverable();

    bool OpenConsultSession(const std::string &deviceId);
    void OnConsultDataReceived(int transportId, const void *data, unsigned int dataLen);
    void OnConsultSessionOpened(int transportId, bool isSource);

    bool ConnectDevice(const CastInnerRemoteDevice &dev);
    void DisconnectDevice(const std::string &deviceId);

    bool UpdateDeviceState(const std::string &deviceId, RemoteDeviceState state);

    std::unique_ptr<CastLocalDevice> GetLocalDeviceInfo();
    void NotifySessionIsReady(int transportId);
    void NotifyDeviceIsOffline(const std::string &deviceId);
    void ReportErrorByListener(const std::string &deviceId);

private:
    bool AuthenticateDevice(const std::string &deviceId);
    void SendConsultData(const CastInnerRemoteDevice &device);
    void DestroyConsulationSession(const std::string &deviceId);
    int GetCastSessionId(int transportId);
    std::unique_ptr<CastInnerRemoteDevice> GetRemoteFromJsonData(const std::string &Data);

    void SetListener(std::shared_ptr<IConnectionManagerListener> listener);
    bool HasListener();
    void ResetListener();

    std::mutex mutex_;
    std::shared_ptr<IConnectionManagerListener> listener_;
    std::map<int, int> transIdToCastSessionIdMap_;
    bool isDiscoverable_{ false };
};

class CastDeviceStateCallback : public DeviceStateCallback {
public:
    void OnDeviceOnline(const DmDeviceInfo &deviceInfo) override;
    void OnDeviceOffline(const DmDeviceInfo &deviceInfo) override;
    void OnDeviceChanged(const DmDeviceInfo &deviceInfo) override;
    void OnDeviceReady(const DmDeviceInfo &deviceInfo) override;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CONNECTION_MANAGER_H