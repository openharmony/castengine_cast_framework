/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: cast device data management
 * Author: zhangge
 * Create: 2022-10-15
 */

#ifndef CAST_DEVICE_DATA_MANAGE_H
#define CAST_DEVICE_DATA_MANAGE_H

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "cast_service_common.h"
#include "device_manager.h"

using OHOS::DistributedHardware::DmDeviceInfo;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
enum class RemoteDeviceState {
    UNKNOWN,
    FOUND,
    CONNECTING,
    CONNECTED,
};

class CastDeviceDataManager final {
public:
    static CastDeviceDataManager &GetInstance();

    ~CastDeviceDataManager() = default;

    bool AddDevice(const CastInnerRemoteDevice &device, const DmDeviceInfo &dmDeviceInfo);
    bool HasDevice(const std::string &deviceId);
    bool UpdateDevice(const CastInnerRemoteDevice &device);
    void RemoveDevice(const std::string &deviceId);

    std::optional<CastInnerRemoteDevice> GetDeviceByDeviceId(const std::string &deviceId);
    std::optional<CastInnerRemoteDevice> GetDeviceByTransId(int sessionId);
    std::optional<DmDeviceInfo> GetDmDevice(const std::string &deviceId);

    bool SetDeviceTransId(const std::string &deviceId, int transportId);
    int GetDeviceTransId(const std::string &deviceId);
    int ResetDeviceTransId(const std::string &deviceId);

    bool SetDeviceRole(const std::string &deviceId, bool isSink);
    std::optional<bool> GetDeviceRole(const std::string &deviceId);

    bool SetDeviceNetworkId(const std::string &deviceId, const std::string &networkId);
    std::optional<std::string> GetDeviceNetworkId(const std::string &deviceId);

    bool SetDeviceIsActiveAuth(const std::string &deviceId, bool isActiveAuth);
    std::optional<bool> GetDeviceIsActiveAuth(const std::string &deviceId);

    bool SetDeviceState(const std::string &deviceId, RemoteDeviceState state);
    RemoteDeviceState GetDeviceState(const std::string &deviceId);
    bool IsDeviceConnected(const std::string &deviceId);
    bool IsDeviceConnecting(const std::string &deviceId);
    bool IsDeviceUsed(const std::string &deviceId);
    int GetSessionIdByDeviceId(const std::string &deviceId);

private:
    struct DeviceInfoCollection {
        CastInnerRemoteDevice device;
        DmDeviceInfo dmDeviceInfo;
        RemoteDeviceState state{ RemoteDeviceState::UNKNOWN };
        std::string networkId;
        int localSessionId{ INVALID_ID };
        int transportId{ INVALID_ID };
        bool isActiveAuth{ false };
        bool isSink{ false };
    };

    CastDeviceDataManager() = default;

    std::vector<DeviceInfoCollection>::iterator GetDeviceLocked(const std::string &deviceId);
    void RemoveDeivceLocked(const std::string &deviceId);
    RemoteDeviceState GetDeviceStateLocked(const std::string &deviceId);
    bool HasDeviceLocked(const std::string &deviceId);

    std::mutex mutex_;
    std::vector<DeviceInfoCollection> devices_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif