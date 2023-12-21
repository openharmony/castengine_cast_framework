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

#include "connection_manager.h"

#include <thread>

#include "dm_constants.h"
#include "json.hpp"
#include "securec.h"

#include "cast_engine_dfx.h"
#include "cast_engine_log.h"
#include "discovery_manager.h"
#include "session.h"

using nlohmann::json;
using OHOS::DistributedHardware::DeviceManager;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Connection-Manager");
namespace {
constexpr char SESSION_NAME[] = "CastPlusSessionName";

constexpr int SOFTBUS_OK = 0;
constexpr int DM_OK = 0;

constexpr int AUTH_WITH_PIN = 1;

const std::string VERSION_KEY = "version";
const std::string OPERATION_TYPE_KEY = "operType";
const std::string SEQUENCE_NUMBER = "sequenceNumber";
const std::string DATA_KEY = "data";

const std::string DEVICE_ID_KEY = "deviceId";
const std::string DEVICE_NAME_KEY = "deviceName";
const std::string KEY_SESSION_ID = "sessionId";

const std::string VERSION = "OH1.0";
constexpr int OPERATION_CONSULT = 3;

constexpr int FIRST_PRIO_INDEX = 0;
constexpr int SECOND_PRIO_INDEX = 1;
constexpr int THIRD_PRIO_INDEX = 2;
constexpr int MAX_LINK_TYPE_NUM = 3;

DmDeviceInfo GetDmDeviceInfo(const std::string &deviceId)
{
    std::vector<DmDeviceInfo> trustedDevices;
    if (DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", trustedDevices) != DM_OK) {
        return {};
    }

    for (const auto &device : trustedDevices) {
        if (device.deviceId == deviceId) {
            return device;
        }
    }

    return {};
}
} // namespace

namespace CastSink {
int OnSessionOpened(int sessionId, int result)
{
    CLOGD("OnSessionOpened, session id = %{public}d, result is %{public}d", sessionId, result);
    if (sessionId <= INVALID_ID || result != SOFTBUS_OK) {
        return result;
    }

    ConnectionManager::GetInstance().OnConsultSessionOpened(sessionId, false);
    return SOFTBUS_OK;
}

void OnSessionClosed(int sessionId)
{
    CLOGD("OnSessionClosed, session id = %{public}d", sessionId);
    if (sessionId <= INVALID_ID) {
        return;
    }
}

void OnBytesReceived(int sessionId, const void *data, unsigned int dataLen)
{
    CLOGD("OnBytesReceived,session id = %{public}d, len = %{public}u", sessionId, dataLen);
    if (sessionId <= INVALID_ID || data == nullptr || dataLen == 0) {
        return;
    }

    ConnectionManager::GetInstance().OnConsultDataReceived(sessionId, data, dataLen);
}

ISessionListener g_sinkSessionListener = {
    OnSessionOpened, OnSessionClosed, OnBytesReceived, nullptr, nullptr, nullptr
};
}

namespace CastSource {
int OnSessionOpened(int sessionId, int result)
{
    CLOGI("OnSessionOpened, session id = %{public}d, result is %{public}d", sessionId, result);
    if (sessionId <= INVALID_ID || result != SOFTBUS_OK) {
        return result;
    }

    ConnectionManager::GetInstance().OnConsultSessionOpened(sessionId, true);
    return SOFTBUS_OK;
}

void OnSessionClosed(int sessionId)
{
    CLOGI("OnSessionClosed, session id = %{public}d", sessionId);
    if (sessionId <= INVALID_ID) {
        return;
    }
    std::thread([]() { RemoveSessionServer(PKG_NAME, SESSION_NAME); }).detach();
}

ISessionListener g_sourceSessionListener = { OnSessionOpened, OnSessionClosed, nullptr, nullptr, nullptr, nullptr };
} // CastSource

ConnectionManager &ConnectionManager::GetInstance()
{
    static ConnectionManager instance{};
    return instance;
}

void ConnectionManager::Init(std::shared_ptr<IConnectionManagerListener> listener)
{
    CLOGD("ConnectionManager init start");
    if (listener == nullptr) {
        CLOGE("The input listener is null!");
        return;
    }

    if (HasListener()) {
        CLOGE("Already inited");
        return;
    }

    if (DeviceManager::GetInstance().RegisterDevStateCallback(PKG_NAME, "",
        std::make_shared<CastDeviceStateCallback>()) != DM_OK) {
        CLOGE("Failed to register device state callback");
        return;
    }

    SetListener(listener);
    CLOGD("ConnectionManager init done");
}

void ConnectionManager::Deinit()
{
    CLOGD("Deinit start");
    ResetListener();
    DisableDiscoverable();
    DeviceManager::GetInstance().UnRegisterDevStateCallback(PKG_NAME);
}

bool ConnectionManager::IsDeviceTrusted(const std::string &deviceId, std::string &networkId)
{
    std::vector<DmDeviceInfo> trustedDevices;
    if (DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", trustedDevices) != DM_OK) {
        return false;
    }

    for (const auto &device : trustedDevices) {
        CLOGV("Trusted device id(%s)", device.deviceId);
        if (device.deviceId == deviceId) {
            networkId = device.networkId;
            return true;
        }
    }

    return false;
}

bool ConnectionManager::ConnectDevice(const CastInnerRemoteDevice &dev)
{
    auto &deviceId = dev.deviceId;
    CLOGI("ConnectDevice in, %s", deviceId.c_str());

    if (CastDeviceDataManager::GetInstance().IsDeviceUsed(deviceId)) {
        return true;
    }

    if (!UpdateDeviceState(deviceId, RemoteDeviceState::CONNECTING)) {
        CLOGE("device(%s) is missing", deviceId.c_str());
        return false;
    }

    DiscoveryManager::GetInstance().StopDiscovery();
    std::string networkId;
    if (IsDeviceTrusted(dev.deviceId, networkId)) {
        if (!CastDeviceDataManager::GetInstance().SetDeviceNetworkId(deviceId, networkId) ||
            !OpenConsultSession(deviceId)) {
            RemoveSessionServer(PKG_NAME, SESSION_NAME);
            (void)UpdateDeviceState(deviceId, RemoteDeviceState::FOUND);
            return false;
        }

        (void)UpdateDeviceState(deviceId, RemoteDeviceState::CONNECTED);
        return true;
    }

    if (!AuthenticateDevice(dev.deviceId)) {
        (void)UpdateDeviceState(deviceId, RemoteDeviceState::FOUND);
        return false;
    }

    return true;
}

bool ConnectionManager::AuthenticateDevice(const std::string &deviceId)
{
    auto dmDeviceInfo = CastDeviceDataManager::GetInstance().GetDmDevice(deviceId);
    if (dmDeviceInfo == std::nullopt) {
        CLOGE("The device(%s) is missing", deviceId.c_str());
        return false;
    }

    std::string extraInfo = R"({
        "targetPkgName": "CastEngineService",
        "appName": "CastEngineService",
        "appDescription": "Cast engine service",
        "business": '0'
    })";
    int ret = DeviceManager::GetInstance().AuthenticateDevice(PKG_NAME, AUTH_WITH_PIN, *dmDeviceInfo, extraInfo,
        std::make_shared<CastAuthenticateCallback>());
    if (ret != DM_OK) {
        CLOGE("ConnectDevice AuthenticateDevice fail, ret = %{public}d)", ret);
        CastEngineDfx::WriteErrorEvent(AUTHENTICATE_DEVICE_FAIL);
        return false;
    }

    CastDeviceDataManager::GetInstance().SetDeviceIsActiveAuth(deviceId, true);
    CLOGI("Finish to authenticate device!");
    return true;
}

bool ConnectionManager::OpenConsultSession(const std::string &deviceId)
{
    // Only the source end enters this path, so the softbus server should be created firstly.
    int32_t ret = CreateSessionServer(PKG_NAME, SESSION_NAME, &CastSource::g_sourceSessionListener);
    if (ret != SOFTBUS_OK) {
        CLOGE("CreateSessionServer failed, ret:%d", ret);
        CastEngineDfx::WriteErrorEvent(SOURCE_CREATE_SESSION_SERVER_FAIL);
        return false;
    }

    // The session can only be opened using a network ID instead of a UDID in OH system
    auto networkId = CastDeviceDataManager::GetInstance().GetDeviceNetworkId(deviceId);
    if (networkId == std::nullopt) {
        return false;
    }

    SessionAttribute attr {};
    attr.dataType = TYPE_BYTES;
    attr.linkTypeNum = MAX_LINK_TYPE_NUM;
    attr.linkType[FIRST_PRIO_INDEX] = LINK_TYPE_WIFI_P2P;
    attr.linkType[SECOND_PRIO_INDEX] = LINK_TYPE_WIFI_WLAN_5G;
    attr.linkType[THIRD_PRIO_INDEX] = LINK_TYPE_WIFI_WLAN_2G;
    auto transportId = OpenSession(SESSION_NAME, SESSION_NAME, networkId->c_str(), "", &attr);
    if (transportId <= INVALID_ID) {
        CLOGW("Failed to open session, and try again, id:%{public}d", transportId);
        transportId = OpenSession(SESSION_NAME, SESSION_NAME, networkId->c_str(), "", &attr);
        if (transportId <= INVALID_ID) {
            CLOGE("Failed to open session finally, id:%{public}d", transportId);
            CastEngineDfx::WriteErrorEvent(OPEN_SESSION_FAIL);
            RemoveSessionServer(PKG_NAME, SESSION_NAME);
            return false;
        }
    }
    if (!CastDeviceDataManager::GetInstance().SetDeviceTransId(deviceId, transportId)) {
        CloseSession(transportId);
        RemoveSessionServer(PKG_NAME, SESSION_NAME);
        return false;
    }

    UpdateDeviceState(deviceId, RemoteDeviceState::CONNECTED);
    CLOGD("Out, sessionId = %{public}d", transportId);
    return true;
}

std::unique_ptr<CastLocalDevice> ConnectionManager::GetLocalDeviceInfo()
{
    CLOGD("GetLocalDeviceInfo in");
    DmDeviceInfo local;
    if (DeviceManager::GetInstance().GetLocalDeviceInfo(PKG_NAME, local) != DM_OK) {
        CLOGE("Cannot get the local device info from DM");
        return nullptr;
    };
    auto device = std::make_unique<CastLocalDevice>();
    device->deviceId = local.deviceId;
    device->deviceType = DeviceType::DEVICE_CAST_PLUS;
    device->deviceName = local.deviceName;
    CLOGD("GetLocalDeviceInfo out");
    return device;
}

void ConnectionManager::SendConsultData(const CastInnerRemoteDevice &device)
{
    CLOGD("In");

    int transportId = CastDeviceDataManager::GetInstance().GetDeviceTransId(device.deviceId);
    if (transportId == INVALID_ID) {
        return;
    }

    auto local = GetLocalDeviceInfo();
    if (local == nullptr) {
        return;
    }
    json body;
    body[DEVICE_ID_KEY] = local->deviceId;
    body[DEVICE_NAME_KEY] = local->deviceName;
    body[KEY_SESSION_ID] = device.sessionId;

    json data;
    data[VERSION_KEY] = VERSION;
    data[OPERATION_TYPE_KEY] = OPERATION_CONSULT;
    data[SEQUENCE_NUMBER] = rand();
    data[DATA_KEY] = body;

    std::string dataStr = data.dump();
    int ret = SendBytes(transportId, dataStr.c_str(), dataStr.size());
    if (ret != SOFTBUS_OK) {
        CLOGE("failed to send consultion data, return:%{public}d", ret);
        CastEngineDfx::WriteErrorEvent(SEND_CONSULTION_DATA_FAIL);
        return;
    }
    CLOGD("return:%{public}d, data:%s", ret, dataStr.c_str());
}

void ConnectionManager::OnConsultSessionOpened(int transportId, bool isSource)
{
    std::thread([transportId, isSource]() {
        if (isSource) {
            auto device = CastDeviceDataManager::GetInstance().GetDeviceByTransId(transportId);
            if (device == std::nullopt) {
                return;
            }
            ConnectionManager::GetInstance().SendConsultData(*device);
            return;
        }
        ConnectionManager::GetInstance().NotifySessionIsReady(transportId);
    }).detach();
    return;
}

void ConnectionManager::NotifySessionIsReady(int transportId)
{
    int castSessionId = listener_->NotifySessionIsReady();
    if (castSessionId == INVALID_ID) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    transIdToCastSessionIdMap_.insert({ transportId, castSessionId });
}

int ConnectionManager::GetCastSessionId(int transportId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &element : transIdToCastSessionIdMap_) {
        if (element.first == transportId) {
            return element.second;
        }
    }
    CLOGE("Invalid transport id:%{public}d", transportId);
    return INVALID_ID;
}

std::unique_ptr<CastInnerRemoteDevice> ConnectionManager::GetRemoteFromJsonData(const std::string &data)
{
    json jsonObject;
    if (!jsonObject.accept(data)) {
        CLOGE("something wrong for the json data!");
        return nullptr;
    }
    jsonObject = json::parse(data, nullptr, false);
    if (!jsonObject.contains(DATA_KEY)) {
        CLOGE("json object have no data!");
        return nullptr;
    }

    json remote = jsonObject[DATA_KEY];
    if (remote.is_discarded()) {
        CLOGE("json object discarded!");
        return nullptr;
    }

    if (remote.contains(DEVICE_ID_KEY) && !remote[DEVICE_ID_KEY].is_string()) {
        CLOGE("DEVICE_ID_KEY json data is not string");
        return nullptr;
    }

    if (remote.contains(DEVICE_NAME_KEY) && !remote[DEVICE_NAME_KEY].is_string()) {
        CLOGE("DEVICE_NAME_KEY json data is not string");
        return nullptr;
    }

    if (remote.contains(KEY_SESSION_ID) && !remote[KEY_SESSION_ID].is_number()) {
        CLOGE("KEY_SESSION_ID json data is not number");
        return nullptr;
    }

    auto device = std::make_unique<CastInnerRemoteDevice>();
    device->deviceId = remote.contains(DEVICE_ID_KEY) ? remote[DEVICE_ID_KEY] : "";
    device->deviceName = remote.contains(DEVICE_NAME_KEY) ? remote[DEVICE_NAME_KEY] : "";
    device->deviceType = DeviceType::DEVICE_CAST_PLUS;
    if (remote.contains(KEY_SESSION_ID)) {
        device->sessionId = remote[KEY_SESSION_ID];
    }

    return device;
}

void ConnectionManager::OnConsultDataReceived(int transportId, const void *data, unsigned int dataLen)
{
    std::string dataStr(static_cast<const char *>(data), dataLen);
    CLOGD("Received data: len:%{public}u, data:%s", dataLen, dataStr.c_str());

    auto device = GetRemoteFromJsonData(dataStr);
    if (device == nullptr) {
        return;
    }
    const std::string &deviceId = device->deviceId;
    auto dmDevice = GetDmDeviceInfo(deviceId);
    if (deviceId.compare(dmDevice.deviceId) != 0) {
        CLOGE("Failed to get DmDeviceInfo");
        return;
    }
    int castSessionId = GetCastSessionId(transportId);
    if (castSessionId == INVALID_ID) {
        return;
    }

    if (!CastDeviceDataManager::GetInstance().AddDevice(*device, dmDevice)) {
        return;
    }
    if (!CastDeviceDataManager::GetInstance().SetDeviceRole(deviceId, true) ||
        !UpdateDeviceState(deviceId, RemoteDeviceState::CONNECTED)) {
        CastDeviceDataManager::GetInstance().RemoveDevice(deviceId);
        return;
    }
    if (!listener_->NotifyRemoteDeviceIsReady(castSessionId, *device)) {
        CastDeviceDataManager::GetInstance().RemoveDevice(deviceId);
    }

    DestroyConsulationSession(deviceId);
}

void ConnectionManager::DestroyConsulationSession(const std::string &deviceId)
{
    int transportId = CastDeviceDataManager::GetInstance().ResetDeviceTransId(deviceId);
    if (transportId != INVALID_ID) {
        CloseSession(transportId);
    }

    auto isSink = CastDeviceDataManager::GetInstance().GetDeviceRole(deviceId);
    if (isSink == std::nullopt || (*isSink)) {
        // The sink's Server is only removed when DisableDiscoverable or Deinit is performed.
        return;
    }

    RemoveSessionServer(PKG_NAME, SESSION_NAME);
}

void ConnectionManager::DisconnectDevice(const std::string &deviceId)
{
    DiscoveryManager::GetInstance().StopDiscovery();
    if (!CastDeviceDataManager::GetInstance().IsDeviceUsed(deviceId)) {
        CLOGE("Device(%s) is not used, remove it", deviceId.c_str());
        CastDeviceDataManager::GetInstance().RemoveDevice(deviceId);
        return;
    }

    DestroyConsulationSession(deviceId);
    auto isActiveAuth = CastDeviceDataManager::GetInstance().GetDeviceIsActiveAuth(deviceId);
    if (isActiveAuth == std::nullopt) {
        return;
    }
    auto dmDeviceInfo = CastDeviceDataManager::GetInstance().GetDmDevice(deviceId);
    if (dmDeviceInfo == std::nullopt) {
        return;
    }

    if (*isActiveAuth) {
        DeviceManager::GetInstance().UnAuthenticateDevice(PKG_NAME, *dmDeviceInfo);
    }
    CastDeviceDataManager::GetInstance().RemoveDevice(deviceId);
}

bool ConnectionManager::EnableDiscoverable()
{
    std::lock_guard lock(mutex_);
    if (isDiscoverable_) {
        CLOGW("service has been set discoverable");
        return true;
    }

    int ret = CreateSessionServer(PKG_NAME, SESSION_NAME, &CastSink::g_sinkSessionListener);
    if (ret != 0) {
        CLOGE("CreateSessionServer failed, ret = %{public}d", ret);
        CastEngineDfx::WriteErrorEvent(SINK_CREATE_SESSION_SERVER_FAIL);
        return false;
    }

    isDiscoverable_ = true;
    return true;
}

bool ConnectionManager::DisableDiscoverable()
{
    std::lock_guard lock(mutex_);
    if (!isDiscoverable_) {
        return true;
    }

    RemoveSessionServer(PKG_NAME, SESSION_NAME);
    isDiscoverable_ = false;
    return true;
}

void ConnectionManager::SetListener(std::shared_ptr<IConnectionManagerListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    listener_ = listener;
}

bool ConnectionManager::HasListener()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return listener_ != nullptr;
}

void ConnectionManager::ResetListener()
{
    SetListener(nullptr);
}

bool ConnectionManager::UpdateDeviceState(const std::string &deviceId, RemoteDeviceState state)
{
    return CastDeviceDataManager::GetInstance().SetDeviceState(deviceId, state);
}

void ConnectionManager::ReportErrorByListener(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!listener_) {
        return;
    }
    listener_->OnError(deviceId);
}

void ConnectionManager::NotifyDeviceIsOffline(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!listener_) {
        return;
    }
    listener_->NotifyDeviceIsOffline(deviceId);
}

void CastAuthenticateCallback::OnAuthResult(const std::string &deviceId, const std::string &token, int32_t status,
    int32_t reason)
{
    if (reason != DM_OK) {
        ConnectionManager::GetInstance().ReportErrorByListener(deviceId);
        CastEngineDfx::WriteErrorEvent(AUTHENTICATE_DEVICE_FAIL);
    }
    CLOGI("OnAuthResult, device id:%s, token:%s, status: %{public}d, %{public}d", deviceId.c_str(), token.c_str(),
        status, reason);
}

void CastDeviceStateCallback::OnDeviceOnline(const DmDeviceInfo &deviceInfo)
{
    CLOGI("device(%s) is online", deviceInfo.deviceId);
    std::vector<DmDeviceInfo> devices;
    devices.push_back(deviceInfo);
    DiscoveryManager::GetInstance().NotifyDeviceIsFound(devices, true, true);
}

void CastDeviceStateCallback::OnDeviceOffline(const DmDeviceInfo &deviceInfo)
{
    CLOGI("device(%s) is offline", deviceInfo.deviceId);
    ConnectionManager::GetInstance().NotifyDeviceIsOffline(deviceInfo.deviceId);
}

void CastDeviceStateCallback::OnDeviceChanged(const DmDeviceInfo &deviceInfo)
{
    CLOGI("device(%s) is changed", deviceInfo.deviceId);
}

void CastDeviceStateCallback::OnDeviceReady(const DmDeviceInfo &deviceInfo)
{
    std::string deviceId = deviceInfo.deviceId;
    CLOGI("device(%s) is ready, network:%s", deviceId.c_str(), deviceInfo.networkId);
    if (!CastDeviceDataManager::GetInstance().SetDeviceNetworkId(deviceId, deviceInfo.networkId)) {
        return;
    }
    auto isActiveAuth = CastDeviceDataManager::GetInstance().GetDeviceIsActiveAuth(deviceId);
    if (isActiveAuth == std::nullopt || !(*isActiveAuth)) {
        return;
    }
    ConnectionManager::GetInstance().OpenConsultSession(deviceId);
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
