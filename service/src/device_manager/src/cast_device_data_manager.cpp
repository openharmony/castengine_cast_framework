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

#include "cast_device_data_manager.h"

#include "cast_engine_log.h"
#include "cast_service_common.h"
#include "securec.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-DeviceDataManager");

CastDeviceDataManager &CastDeviceDataManager::GetInstance()
{
    static CastDeviceDataManager manager{};
    return manager;
}

bool CastDeviceDataManager::AddDevice(const CastInnerRemoteDevice &device, const DmDeviceInfo &dmDeviceInfo)
{
    if (device.deviceId.empty() || device.deviceId != dmDeviceInfo.deviceId) {
        CLOGE("Invalid device id<%s-%s>", device.deviceId.c_str(), dmDeviceInfo.deviceId);
        return false;
    }

    DeviceInfoCollection data = {
        .device = device,
        .dmDeviceInfo = dmDeviceInfo,
        .state = RemoteDeviceState::FOUND,
        .networkId = dmDeviceInfo.networkId,
    };

    std::lock_guard<std::mutex> lock(mutex_);
    if (HasDeviceLocked(device.deviceId)) {
        CLOGW("Has a device with the same device id(%s)!", device.deviceId.c_str());
        data.state = GetDeviceStateLocked(device.deviceId);
        RemoveDeivceLocked(device.deviceId);
    }

    data.device.sessionKey = nullptr;
    devices_.push_back(data);
    return true;
}

bool CastDeviceDataManager::HasDevice(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return HasDeviceLocked(deviceId);
}

std::optional<CastInnerRemoteDevice> CastDeviceDataManager::GetDeviceByDeviceId(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return std::nullopt;
    }
    return it->device;
}

std::optional<CastInnerRemoteDevice> CastDeviceDataManager::GetDeviceByTransId(int transportId)
{
    if (transportId <= INVALID_ID) {
        CLOGE("Invalid session id, %d", transportId);
        return std::nullopt;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->transportId == transportId) {
            return it->device;
        }
    }
    return std::nullopt;
}

std::optional<DmDeviceInfo> CastDeviceDataManager::GetDmDevice(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return std::nullopt;
    }
    return it->dmDeviceInfo;
}

bool CastDeviceDataManager::UpdateDevice(const CastInnerRemoteDevice &device)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(device.deviceId);
    if (it == devices_.end()) {
        return false;
    }
    if (it->device.deviceName != device.deviceName) {
        CLOGW("Different devices name: old:%s, new:%s", it->device.deviceName.c_str(), device.deviceName.c_str());
    }

    it->device = device;
    it->device.sessionKey = nullptr;

    return true;
}

void CastDeviceDataManager::RemoveDevice(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RemoveDeivceLocked(deviceId);
}

bool CastDeviceDataManager::SetDeviceTransId(const std::string &deviceId, int transportId)
{
    if (transportId <= INVALID_ID) {
        CLOGE("Invalid params: id(%d)", transportId);
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }

    if (it->transportId != INVALID_ID) {
        CLOGE("Device(%s) has matched a session id(%d) in the DB", deviceId.c_str(), it->transportId);
        return false;
    }
    it->transportId = transportId;
    return true;
}

int CastDeviceDataManager::GetDeviceTransId(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return INVALID_ID;
    }
    return it->transportId;
}

int CastDeviceDataManager::ResetDeviceTransId(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return INVALID_ID;
    }

    int tmp = it->transportId;
    it->transportId = INVALID_ID;
    return tmp;
}

bool CastDeviceDataManager::SetDeviceRole(const std::string &deviceId, bool isSink)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }

    it->isSink = isSink;
    return true;
}

std::optional<bool> CastDeviceDataManager::GetDeviceRole(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return std::nullopt;
    }

    return it->isSink;
}

bool CastDeviceDataManager::SetDeviceNetworkId(const std::string &deviceId, const std::string &networkId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }

    it->networkId = networkId;
    return true;
}

std::optional<std::string> CastDeviceDataManager::GetDeviceNetworkId(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return std::nullopt;
    }

    return it->networkId;
}

bool CastDeviceDataManager::SetDeviceIsActiveAuth(const std::string &deviceId, bool isActiveAuth)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }
    it->isActiveAuth = isActiveAuth;
    return true;
}

std::optional<bool> CastDeviceDataManager::GetDeviceIsActiveAuth(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return std::nullopt;
    }
    return it->isActiveAuth;
}

bool CastDeviceDataManager::SetDeviceState(const std::string &deviceId, RemoteDeviceState state)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }
    it->state = state;
    return true;
}

RemoteDeviceState CastDeviceDataManager::GetDeviceState(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return GetDeviceStateLocked(deviceId);
}

bool CastDeviceDataManager::IsDeviceConnecting(const std::string &deviceId)
{
    return GetDeviceState(deviceId) == RemoteDeviceState::CONNECTING;
}

bool CastDeviceDataManager::IsDeviceConnected(const std::string &deviceId)
{
    return GetDeviceState(deviceId) == RemoteDeviceState::CONNECTED;
}

bool CastDeviceDataManager::IsDeviceUsed(const std::string &deviceId)
{
    auto state = GetDeviceState(deviceId);
    return state == RemoteDeviceState::CONNECTING || state == RemoteDeviceState::CONNECTED;
}

bool CastDeviceDataManager::HasDeviceLocked(const std::string &deviceId)
{
    if (deviceId.empty()) {
        CLOGE("Empty device id!");
        return false;
    }

    for (const auto &device : devices_) {
        if (device.device.deviceId == deviceId) {
            return true;
        }
    }

    return false;
}

std::vector<CastDeviceDataManager::DeviceInfoCollection>::iterator CastDeviceDataManager::GetDeviceLocked(
    const std::string &deviceId)
{
    if (deviceId.empty()) {
        CLOGE("Empty device id!");
        return devices_.end();
    }

    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->device.deviceId == deviceId) {
            return it;
        }
    }
    CLOGW("Can't find the device(%s)!", deviceId.c_str());
    return devices_.end();
}

void CastDeviceDataManager::RemoveDeivceLocked(const std::string &deviceId)
{
    if (deviceId.empty()) {
        CLOGE("Empty device id!");
        return;
    }

    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->device.deviceId == deviceId) {
            devices_.erase(it);
            return;
        }
    }
}

RemoteDeviceState CastDeviceDataManager::GetDeviceStateLocked(const std::string &deviceId)
{
    auto it = GetDeviceLocked(deviceId);
    return (it != devices_.end()) ? it->state : RemoteDeviceState::UNKNOWN;
}

int CastDeviceDataManager::GetSessionIdByDeviceId(const std::string &deviceId)
{
    if (deviceId.empty()) {
        CLOGE("Empty device id!");
        return INVALID_ID;
    }

    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->device.deviceId == deviceId) {
            return it->device.sessionId;
        }
    }
    return INVALID_ID;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
