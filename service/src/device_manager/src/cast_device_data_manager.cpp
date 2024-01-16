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
 * Description: cast device data management
 * Author: zhangge
 * Create: 2022-10-15
 */

#include "cast_device_data_manager.h"

#include "cast_engine_log.h"
#include "cast_service_common.h"
#include "dm_constants.h"
#include "securec.h"
#include "json.hpp"

using nlohmann::json;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-DeviceDataManager");
namespace {
using namespace OHOS::DistributedHardware;
constexpr int SESSION_KEY_LENGTH = 16;
}
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
    std::lock_guard<std::mutex> lock(mutex_);
    DeviceInfoCollection data{};
    for (auto it : devices_) {
        if (device.deviceId == it.device.deviceId) {
            data = it;
            break;
        }
    }
    if (data.device.deviceId.empty()) {
        data.state = RemoteDeviceState::FOUND;
    }

    json extraJson = json::parse(dmDeviceInfo.extraData, nullptr, false);
    if (extraJson.is_discarded()) {
        CLOGI("extrajson is discarded");
        data.wifiDeviceInfo = data.wifiDeviceInfo.extraData.size() > 0 ? data.wifiDeviceInfo : dmDeviceInfo;
    } else if (extraJson.contains(PARAM_KEY_BLE_MAC) && extraJson[PARAM_KEY_BLE_MAC].is_string()) {
        data.bleDeviceInfo = dmDeviceInfo;
    } else if (extraJson.contains(PARAM_KEY_WIFI_IP) && extraJson[PARAM_KEY_WIFI_IP].is_string()) {
        data.wifiDeviceInfo = dmDeviceInfo;
    } else {
        data.wifiDeviceInfo = dmDeviceInfo;
    }

    int sessionId = device.sessionId != INVALID_ID ? device.sessionId : data.device.sessionId;
    data.device = device;
    data.device.sessionId = sessionId;
    data.networkId = strlen(dmDeviceInfo.networkId) > 0 ? dmDeviceInfo.networkId : data.networkId;
    CLOGI("sessionId is %d", sessionId);
    RemoveDeivceLocked(device.deviceId);

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
    return strlen(it->wifiDeviceInfo.deviceId) > 0 ? it->wifiDeviceInfo : it->bleDeviceInfo;
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

bool CastDeviceDataManager::RemoveDeivceLocked(const std::string &deviceId)
{
    CLOGI("RemoveDeivceLocked in");
    if (deviceId.empty()) {
        CLOGE("Empty device id!");
        return false;
    }

    auto sessionId = GetSessionIdByDeviceId(deviceId);
    if (sessionId != INVALID_ID) {
        for (auto it = devices_.begin(); it != devices_.end(); it++) {
            if (it->device.sessionId == sessionId) {
                devices_.erase(it);
                return true;
            }
        }
    }

    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->device.deviceId == deviceId) {
            devices_.erase(it);
            return true;
        }
    }
    return false;
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

bool CastDeviceDataManager::UpdateDeivceByDeviceId(const std::string &deviceId)
{
    CLOGI("ClearDeivceByDeviceId in");
    if (deviceId.empty()) {
        CLOGE("Empty device id!");
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->device.deviceId == deviceId) {
            it->state = RemoteDeviceState::UNKNOWN;
            it->localSessionId = INVALID_ID;
            it->transportId = INVALID_ID;
            it->isActiveAuth = false;
            return true;
        }
    }
    return false;
}

bool CastDeviceDataManager::IsDoubleFrameDevice(const std::string &deviceId)
{
    CLOGI("IsDoubleFrameDevice in");
    if (deviceId.empty()) {
        CLOGE("Empty device id!");
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = devices_.begin(); it != devices_.end(); it++) {
        if (it->device.deviceId == deviceId) {
            return !it->device.customData.empty() ? true : false;
        }
    }
    return false;
}

bool CastDeviceDataManager::SetDeviceSessionKey(const std::string &deviceId, const uint8_t *sessionKey)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }
    if (memcpy_s(it->device.sessionKey, SESSION_KEY_LENGTH, sessionKey, SESSION_KEY_LENGTH) != 0) {
        return false;
    }
    it->device.sessionKeyLength = SESSION_KEY_LENGTH;
    return true;
}

bool CastDeviceDataManager::SetDeviceIp(const std::string &deviceId, const std::string &localIp,
    const std::string &remoteIp)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }

    it->device.localIp = localIp;
    it->device.remoteIp = remoteIp;
    return true;
}

bool CastDeviceDataManager::SetDeviceChannleType(const std::string &deviceId, const ChannelType &channelType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        return false;
    }

    it->device.channelType = channelType;
    return true;
}


std::optional<std::string> CastDeviceDataManager::GetDeviceNameByDeviceId(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = GetDeviceLocked(deviceId);
    if (it == devices_.end()) {
        CLOGE("No device found");
        return std::nullopt;
    }

    if (strlen(it->wifiDeviceInfo.deviceName) > 0) {
        CLOGI("WIFI device name is not empty %s", it->wifiDeviceInfo.deviceName);
        return std::string(it->wifiDeviceInfo.deviceName);
    } else if (strlen(it->bleDeviceInfo.deviceName) > 0) {
        CLOGI("BLE device name is not empty %s", it->bleDeviceInfo.deviceName);
        return std::string(it->bleDeviceInfo.deviceName);
    }
    CLOGW("Device name is empty");
    return std::nullopt;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
