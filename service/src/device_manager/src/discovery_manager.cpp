/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: implement the cast source discovery
 * Author: zhangge
 * Create: 2022-08-23
 */

#include "discovery_manager.h"
#include "connection_manager.h"
#include "cast_device_data_manager.h"
#include "cast_engine_dfx.h"
#include "cast_engine_log.h"
#include "dm_constants.h"
#include "securec.h"

using namespace OHOS::DistributedHardware;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Discovery-Manager");

namespace {
constexpr int WIFI_SUBSCRIBE_ID = 0;

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
    DiscoveryManager::GetInstance().Deinit();
}

DiscoveryManager &DiscoveryManager::GetInstance()
{
    static DiscoveryManager instance{};
    return instance;
}

void DiscoveryManager::Init(std::shared_ptr<IDiscoveryManagerListener> listener)
{
    CLOGD("init start");
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
}

void DiscoveryManager::GetAndReportTrustedDevices()
{
    std::vector<DmDeviceInfo> dmDevices;
    auto result = DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", dmDevices);
    if (result != DM_OK || dmDevices.size() == 0) {
        CLOGW("No trusted devices, result:%d", result);
        return;
    }

    NotifyDeviceIsFound(dmDevices, true, false);
}

void DiscoveryManager::StartDmDiscovery()
{
    OHOS::DistributedHardware::DmSubscribeInfo subInfo = {
        .subscribeId = WIFI_SUBSCRIBE_ID,
        .mode = DM_DISCOVER_MODE_ACTIVE,
        .medium = DM_AUTO,
        .freq = DM_SUPER_HIGH,
        .isSameAccount = false,
        .isWakeRemote = false,
        .capability = "ddmpCapability",
    };
    int32_t ret = DeviceManager::GetInstance().StartDeviceDiscovery(PKG_NAME, subInfo, "",
        std::make_shared<CastDiscoveryCallback>());
    if (ret != DM_OK && ret != ERR_DM_DISCOVERY_REPEATED) {
        CLOGE("Failed to start discovery, ret:%d", ret);
        CastEngineDfx::WriteErrorEvent(START_DISCOVERY_FAIL);
    }
}

void DiscoveryManager::StartDiscovery()
{
    CLOGI("StartDiscovery in");
    StartDmDiscovery();
    GetAndReportTrustedDevices();
}

void DiscoveryManager::StopDiscovery()
{
    CLOGD("StopDiscovery in");
    DeviceManager::GetInstance().StopDeviceDiscovery(PKG_NAME, WIFI_SUBSCRIBE_ID);
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

void DiscoveryManager::OnDeviceInfoFound(uint16_t subscribeId, const DmDeviceInfo &dmDeviceInfo)
{
    std::string networkId = dmDeviceInfo.networkId;
    CLOGD("OnDeviceInfoFound deviceName is %{public}s", dmDeviceInfo.deviceName);
    bool ret = ConnectionManager::GetInstance().IsDeviceTrusted(dmDeviceInfo.deviceId, networkId);
    if (!ret) {
        NotifyDeviceIsFound({ dmDeviceInfo }, false, false);
        return;
    }
    NotifyDeviceIsFound({ dmDeviceInfo }, true, false);
}

void DiscoveryManager::NotifyDeviceIsFound(const std::vector<DmDeviceInfo> &dmDevices, bool isTrusted, bool isOnline)
{
    std::vector<CastInnerRemoteDevice> devices;
    for (const auto &dmDevice : dmDevices) {
        CastInnerRemoteDevice newDevice = {
            .deviceId = dmDevice.deviceId,
            .deviceName = dmDevice.deviceName,
            .deviceType = ConvertDeviceType(dmDevice.deviceTypeId),
            .subDeviceType = SubDeviceType::SUB_DEVICE_DEFAULT,
            .channelType = ChannelType::SOFT_BUS
        };
        if (!isTrusted) {
            newDevice.subDeviceType = SubDeviceType::SUB_DEVICE_MATEBOOK_PAD;
        }
        if (!isOnline && !CastDeviceDataManager::GetInstance().AddDevice(newDevice, dmDevice)) {
            return;
        }
        devices.push_back(newDevice);
    }
    std::lock_guard<std::mutex> lock(mutex_);
    listener_->OnDeviceFound(devices);
}

void CastDiscoveryCallback::OnDiscoverySuccess(uint16_t subscribeId)
{
    CLOGI("OnDiscoverySuccess, subscribe id:%{public}u", subscribeId);
}

void CastDiscoveryCallback::OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason)
{
    CLOGI("OnDiscoveryFailed, subscribe id:%{public}u, reason:%{public}d", subscribeId, failedReason);
}

void CastDiscoveryCallback::OnDeviceFound(uint16_t subscribeId, const DmDeviceInfo &deviceInfo)
{
    CLOGI("OnDeviceInfoFound in, subscribe id:%{public}u, device id:%s, device name:%s", subscribeId,
        deviceInfo.deviceId, deviceInfo.deviceName);
    DiscoveryManager::GetInstance().OnDeviceInfoFound(subscribeId, deviceInfo);
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
