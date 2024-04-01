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
 * Description: implement the cast source discovery
 * Author: zhangge
 * Create: 2022-08-23
 */
#include <ipc_skeleton.h>
#include <algorithm>
#include <thread>

#include "discovery_manager.h"
#include "connection_manager.h"
#include "cast_device_data_manager.h"
#include "cast_engine_dfx.h"
#include "cast_engine_log.h"
#include "dm_constants.h"
#include "parameters.h"
#include "securec.h"
#include "softbus_common.h"
#include "utils.h"
#include "json.hpp"

using namespace OHOS::DistributedHardware;
using namespace OHOS::AppExecFwk;

using nlohmann::json;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Discovery-Manager");

namespace {
constexpr int AV_SESSION_UID = 6700;
constexpr int REMOTE_DIED_SLEEP = 4000;
constexpr int DISCOVERY_DELAY_TIME = 3000;
constexpr int TIMEOUT_COUNT = 40;
constexpr int EVENT_START_DISCOVERY = 1;
constexpr int EVENT_CONTINUE_DISCOVERY = 2;
const std::string DISCOVERY_TRUST_VALUE = R"({"filters": [{"type": "isTrusted", "value": 2}]})";

DeviceType ConvertDeviceType(uint16_t deviceTypeId)
{
    switch (deviceTypeId) {
        case DEVICE_TYPE_TV:
            return DeviceType::DEVICE_HW_TV;
        case DEVICE_TYPE_PAD:
            return DeviceType::DEVICE_PAD;
        case DEVICE_TYPE_CAR:
            return DeviceType::DEVICE_HICAR;
        case DEVICE_TYPE_2IN1:
            return DeviceType::DEVICE_TYPE_2IN1;
        default:
            return DeviceType::DEVICE_CAST_PLUS;
    }
}
} // namespace

void CastDmInitCallback::OnRemoteDied()
{
    CLOGE("DM is dead, deinit the DiscoveryManager");
    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(REMOTE_DIED_SLEEP));
        constexpr int sleepTime = 100;       // uint: ms
        constexpr int retryTimes = 10 * 10;  // total 10s
        for (int32_t retryTime = 0; retryTime < retryTimes; ++retryTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            if (DeviceManager::GetInstance().InitDeviceManager(PKG_NAME,
                std::make_shared<CastDmInitCallback>()) == DM_OK) {
                CLOGI("DeviceManager onDied, try to init success, retryTime = %d", retryTime);
                return;
            }
            CLOGI("DeviceManager onDied, try to init fail, retryTime = %d", retryTime);
        }
        CLOGI("DeviceManager onDied, try to init has reached the maximum, but still failed");
        DiscoveryManager::GetInstance().Deinit();
        return;
    }).detach();
}

DiscoveryManager &DiscoveryManager::GetInstance()
{
    static DiscoveryManager instance{};
    return instance;
}

void DiscoveryManager::Init(std::shared_ptr<IDiscoveryManagerListener> listener)
{
    CLOGD("init start");
    eventRunner_ = EventRunner::Create("cast-discovery-manager");
    eventHandler_ = std::make_shared<DiscoveryEventHandler>(eventRunner_);
    if (listener == nullptr) {
        CLOGE("The input listener is null!");
        return;
    }

    if (HasListener()) {
        CLOGE("Already inited");
        return;
    }

    if (DeviceManager::GetInstance().InitDeviceManager(PKG_NAME, std::make_shared<CastDmInitCallback>()) != DM_OK) {
        CLOGE("Failed to InitDeviceManager");
        return;
    }

    SetListener(listener);
    CLOGD("init done");
}

void DiscoveryManager::Deinit()
{
    if (!HasListener()) {
        return;
    }

    StopDiscovery();
    ResetListener();
    DeviceManager::GetInstance().UnInitDeviceManager(PKG_NAME);
    eventRunner_->Stop();
}

void DiscoveryManager::GetAndReportTrustedDevices()
{
    std::vector<DmDeviceInfo> dmDevices;
    auto result = DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", true, dmDevices);
    if (result != DM_OK || dmDevices.size() == 0) {
        CLOGW("No trusted devices, result:%d", result);
        return;
    }
    for (const auto &dmDevice : dmDevices) {
        CLOGI("GetAndReportTrustedDevices, device id is %s, device name is %s", dmDevice.deviceId, dmDevice.deviceName);
        CastInnerRemoteDevice newDevice = CreateRemoteDevice(dmDevice);
        NotifyDeviceIsFound(newDevice);
    }
}

void DiscoveryManager::UpdateDeviceState()
{
    auto it = std::find_if(remoteDeviceMap.begin(), remoteDeviceMap.end(), [&](const auto& pair) {
        return scanCount - pair.second >=1 ;
    });
    if (it != remoteDeviceMap.end()) {
        CastInnerRemoteDevice remoteDevice = it->first;
        CLOGE("StartDmDiscovery offline: %{public}s", remoteDevice.deviceName.c_str());
        ConnectionManager::GetInstance().NotifyDeviceIsOffline(remoteDevice.deviceId);
        CastDeviceDataManager::GetInstance().RemoveDevice(remoteDevice.deviceId);
        remoteDeviceMap.erase(it);
    }
}

void DiscoveryManager::StartDmDiscovery()
{
    CLOGI("StartDmDiscovery in");
    UpdateDeviceState();
    scanCount++;
    std::map<std::string, std::string> discoverParam{
        { PARAM_KEY_META_TYPE, std::to_string(5) } };
    std::map<std::string, std::string> filterOptions{
        { PARAM_KEY_FILTER_OPTIONS, DISCOVERY_TRUST_VALUE } };
    int32_t ret = DeviceManager::GetInstance().StartDiscovering(PKG_NAME, discoverParam, filterOptions,
        std::make_shared<CastDiscoveryCallback>());
    if (ret != DM_OK && ret != ERR_DM_DISCOVERY_REPEATED) {
        CLOGE("Failed to start discovery, ret:%d", ret);
        CastEngineDfx::WriteErrorEvent(START_DISCOVERY_FAIL);
    }
}

void DiscoveryManager::StopDmDiscovery()
{
    CLOGI("StopDmDiscovery in");
    std::map<std::string, std::string> discoverParam = {};
    DeviceManager::GetInstance().StopDiscovering(PKG_NAME, discoverParam);
}

bool DiscoveryManager::StartAdvertise()
{
    CLOGI("PublishDiscovery in");
    std::map<std::string, std::string> advertiseParam { { PARAM_KEY_DISC_CAPABILITY, DM_CAPABILITY_CASTPLUS } };
    int ret = DeviceManager::GetInstance().StartAdvertising(PKG_NAME, advertiseParam,
        std::make_shared<CastPublishDiscoveryCallback>());
    if (ret != DM_OK) {
        CLOGE("Failed to publish discovery, ret:%d", ret);
        return false;
    }
    return true;
}

bool DiscoveryManager::StopAdvertise()
{
    CLOGI("UnPublishDiscoveryv in");
    std::map<std::string, std::string> advertiseParam = {};
    int ret = DeviceManager::GetInstance().StopAdvertising(PKG_NAME, advertiseParam);
    if (ret != DM_OK) {
        CLOGE("Failed to unpublish discovery, ret:%d", ret);
        return false;
    }
    return true;
}

void DiscoveryManager::SetProtocolType(int protocols)
{
    protocolType_ = protocols;
}

int DiscoveryManager::GetProtocolType() const
{
    return protocolType_;
}

void DiscoveryManager::StartDiscovery()
{
    HiSysEventWriteWrap(__func__, {
            {"BIZ_SCENE", static_cast<int32_t>(BIZSceneType::DEVICE_DISCOVERY)},
            {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_BEGIN)},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::START_DISCOVERY)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_IDLE)},
            {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
            {"TO_CALL_PKG", ""},
            {"LOCAL_SESS_NAME", ""},
            {"PEER_SESS_NAME", ""},
            {"PEER_UDID", ""}});

    CLOGI("StartDiscovery in");
    scanCount = 0;
    remoteDeviceMap.clear();
    std::lock_guard<std::mutex> lock(mutex_);
    uid_ = IPCSkeleton::GetCallingUid();
    eventHandler_->SendEvent(EVENT_START_DISCOVERY);
    CLOGI("StartDiscovery out");
}

void DiscoveryManager::StopDiscovery()
{
    CLOGI("StopDiscovery in");
    StopDmDiscovery();
    eventHandler_->RemoveAllEvents();
}

void DiscoveryManager::SetListener(std::shared_ptr<IDiscoveryManagerListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    listener_ = listener;
}

bool DiscoveryManager::HasListener()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return listener_ != nullptr;
}

void DiscoveryManager::ResetListener()
{
    SetListener(nullptr);
}

void DiscoveryManager::RemoveSameDeviceLocked(const CastInnerRemoteDevice &newDevice)
{
    auto it = std::find_if(remoteDeviceMap.begin(), remoteDeviceMap.end(), [&](const auto& pair) {
        return pair.first.deviceId == newDevice.deviceId;
    });
    if (it != remoteDeviceMap.end()) {
        remoteDeviceMap.erase(it);
    }
}

CastInnerRemoteDevice DiscoveryManager::CreateRemoteDevice(const DmDeviceInfo &dmDeviceInfo)
{
    CastInnerRemoteDevice newDevice = {
        .deviceId = dmDeviceInfo.deviceId,
        .deviceName = dmDeviceInfo.deviceName,
        .deviceType = ConvertDeviceType(dmDeviceInfo.deviceTypeId),
        .deviceTypeId = dmDeviceInfo.deviceTypeId,
        .subDeviceType = SubDeviceType::SUB_DEVICE_DEFAULT,
        .channelType = ChannelType::SOFT_BUS,
        .networkId = dmDeviceInfo.networkId
    };

    ParseDeviceInfo(dmDeviceInfo, newDevice);

    return newDevice;
}

// {"BLE_MAC":"74:41:3d:f3:3d:cd","BLE_UDID_HASH":"xxxxxx","CONN_ADDR_TYPE":"BLE_TYPE","CUSTOM_DATA":""}
// {"CONN_ADDR_TYPE":"WLAN_IP_TYPE","CUSTOM_DATA":"{\"castPlus\":\"\"}","WIFI_IP":"192.168.135.204","WIFI_PORT":36767}
void DiscoveryManager::OnDeviceInfoFound(uint16_t subscribeId, const DmDeviceInfo &dmDeviceInfo)
{
    CLOGD("OnDeviceInfoFound in deviceName: %{public}s, deviceId: %{public}s, extra: %{public}s",
          dmDeviceInfo.deviceName, dmDeviceInfo.deviceId, dmDeviceInfo.extraData.c_str());
    CastInnerRemoteDevice newDevice = CreateRemoteDevice(dmDeviceInfo);

    // If the map does not exist, the notification is sent.
    auto it = remoteDeviceMap.find(newDevice);
    if (it == remoteDeviceMap.end()) {
        // If the deviceId are the same, delete the old device.
        RemoveSameDeviceLocked(newDevice);

        // Set subDeviceType based on isTrusted.
        std::string networkId = dmDeviceInfo.networkId;
        bool isTrusted = ConnectionManager::GetInstance().IsDeviceTrusted(dmDeviceInfo.deviceId, networkId);
        if (!isTrusted) {
            newDevice.subDeviceType = SubDeviceType::SUB_DEVICE_MATEBOOK_PAD;
        }

        if (CastDeviceDataManager::GetInstance().AddDevice(newDevice, dmDeviceInfo)) {
            NotifyDeviceIsFound(newDevice);
        }
    }
    remoteDeviceMap[newDevice] = scanCount;
}

void DiscoveryManager::NotifyDeviceIsFound(const CastInnerRemoteDevice &newDevice)
{
    std::vector<CastInnerRemoteDevice> devices;
    std::vector<CastInnerRemoteDevice> device2In1;

    if (newDevice.deviceType == DeviceType::DEVICE_TYPE_2IN1) {
        CLOGI("device type is 2IN1");
        device2In1.push_back(newDevice);
    }
    devices.push_back(newDevice);

    std::lock_guard<std::mutex> lock(mutex_);
    listener_->OnDeviceFound(devices);
    if (uid_ == AV_SESSION_UID) {
        listener_->OnDeviceFound(device2In1);
        return;
    }
}

void DiscoveryManager::NotifyDeviceIsOnline(const DmDeviceInfo &dmDeviceInfo)
{
    CastInnerRemoteDevice newDevice = CreateRemoteDevice(dmDeviceInfo);
    NotifyDeviceIsFound(newDevice);
}

void DiscoveryManager::ParseDeviceInfo(const DmDeviceInfo &dmDevice, CastInnerRemoteDevice &castDevice)
{
    CLOGD("parse castData extraData is %s", dmDevice.extraData.c_str());
    auto ret = CastDeviceDataManager::GetInstance().GetDeviceNameByDeviceId(dmDevice.deviceId);
    std::string deviceName = ret != std::nullopt ? *ret : "";
    json jsonObj = json::parse(dmDevice.extraData, nullptr, false);
    if (!jsonObj.is_discarded()) {
        if (jsonObj.contains(PARAM_KEY_WIFI_IP) && jsonObj[PARAM_KEY_WIFI_IP].is_string()) {
            castDevice.wifiIp = jsonObj[PARAM_KEY_WIFI_IP];
            castDevice.deviceName = !castDevice.deviceName.empty() ? castDevice.deviceName : deviceName;
        }
        if (jsonObj.contains(PARAM_KEY_WIFI_PORT) && jsonObj[PARAM_KEY_WIFI_PORT].is_number()) {
            castDevice.wifiPort = jsonObj[PARAM_KEY_WIFI_PORT];
            castDevice.deviceName = !castDevice.deviceName.empty() ? castDevice.deviceName : deviceName;
        }
        if (jsonObj.contains(PARAM_KEY_BLE_MAC) && jsonObj[PARAM_KEY_BLE_MAC].is_string()) {
            castDevice.bleMac = jsonObj[PARAM_KEY_BLE_MAC];
            castDevice.deviceName = !deviceName.empty() ? deviceName : castDevice.deviceName;
        }
        if (jsonObj.contains(PARAM_KEY_CUSTOM_DATA) && jsonObj[PARAM_KEY_CUSTOM_DATA].is_string()) {
            std::string customData = jsonObj[PARAM_KEY_CUSTOM_DATA];
            json softbusCustData = json::parse(customData, nullptr, false);
            if (softbusCustData.contains("castPlus") && softbusCustData["castPlus"].is_string()) {
                std::string castData = softbusCustData["castPlus"];
                castDevice.customData = castData;
            }
            if (softbusCustData.contains("castId") && softbusCustData["castId"].is_string()) {
                castDevice.udid = softbusCustData["castId"];
            }
        }
    } else {
        CLOGE("dm device extraData parse error");
    }
}

void CastDiscoveryCallback::OnDiscoverySuccess(uint16_t subscribeId)
{
    CLOGI("OnDiscoverySuccess, subscribe id:%{public}u", subscribeId);
}

void CastDiscoveryCallback::OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason)
{
    auto errorCode = GetErrorCode(CAST_ENGINE_SYSTEM_ID, CAST_ENGINE_CAST_PLUS_MODULE_ID, subscribeId);
    HiSysEventWriteWrap(__func__, {
            {"BIZ_SCENE", static_cast<int32_t>(BIZSceneType::DEVICE_DISCOVERY)},
            {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_END)},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::DEVICE_DISCOVERY)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_FAILED)},
            {"ERROR_CODE", errorCode}}, {
            {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
            {"LOCAL_SESS_NAME", ""},
            {"PEER_SESS_NAME", ""},
            {"PEER_UDID", ""}});

    CLOGI("OnDiscoveryFailed, subscribe id:%{public}u, reason:%{public}d", subscribeId, failedReason);
}

void CastDiscoveryCallback::OnDeviceFound(uint16_t subscribeId, const DmDeviceInfo &deviceInfo)
{
    CLOGI("OnDeviceInfoFound in, subscribe id:%{public}u, device id:%s, device name:%s", subscribeId,
        deviceInfo.deviceId, deviceInfo.deviceName);
    DiscoveryManager::GetInstance().OnDeviceInfoFound(subscribeId, deviceInfo);
}

void CastPublishDiscoveryCallback::OnPublishResult(int32_t publishId, int32_t publishResult)
{
    CLOGI("OnPublishResult publishId is %{public}d, publishResult is %{public}d", publishId, publishResult);
}

void DiscoveryEventHandler::ProcessEvent(const InnerEvent::Pointer &event)
{
    DiscoveryManager::GetInstance().StopDmDiscovery();
    auto eventId = event->GetInnerEventId();
    switch (eventId) {
        case EVENT_START_DISCOVERY:
            scanCount = 1;
            SendEvent(EVENT_CONTINUE_DISCOVERY, DISCOVERY_DELAY_TIME);
            break;
        case EVENT_CONTINUE_DISCOVERY:
            scanCount++;
            if (scanCount < TIMEOUT_COUNT) {
                SendEvent(EVENT_CONTINUE_DISCOVERY, DISCOVERY_DELAY_TIME);
            }
            break;
        default:
            break;
    }
    CLOGI("handler run, thread name: %{public}s, scanCount: %{public}d, eventId: %{public}d",
          GetEventRunner()->GetRunnerThreadName().c_str(), scanCount, eventId);
    DiscoveryManager::GetInstance().StartDmDiscovery();
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
