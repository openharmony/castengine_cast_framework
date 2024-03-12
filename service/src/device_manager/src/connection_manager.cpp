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
 * Description: implement the cast source connect
 * Author: zhangge
 * Create: 2022-08-23
 */

#include "connection_manager.h"

#include <thread>

#include "dm_device_info.h"
#include "dm_constants.h"
#include "json.hpp"
#include "securec.h"

#include "cast_engine_dfx.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "discovery_manager.h"
#include "session.h"
#include "softbus_common.h"
#include "utils.h"
#include "openssl/rand.h"
#include "encrypt_decrypt.h"
#include "utils.h"
#include <iconv.h>

using nlohmann::json;
using namespace OHOS::DistributedHardware;
using OHOS::DistributedHardware::DeviceManager;
using OHOS::DistributedHardware::PeerTargetId;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Connection-Manager");
namespace {
using namespace OHOS::DistributedHardware;
constexpr char SESSION_NAME[] = "CastPlusSessionName";

constexpr int SOFTBUS_OK = 0;

const std::string AUTH_WITH_PIN = "1";

const std::string VERSION_KEY = "version";
const std::string OPERATION_TYPE_KEY = "operType";
const std::string SEQUENCE_NUMBER = "sequenceNumber";
const std::string DATA_KEY = "data";

const std::string DEVICE_ID_KEY = "deviceId";
const std::string DEVICE_NAME_KEY = "deviceName";
const std::string KEY_SESSION_ID = "sessionId";
const std::string PROTOCOL_TYPE_KEY = "protocolType";
const std::string KEY_TRANSFER_MODE = "transferMode";
const std::string DEVICE_CAST_SOURCE = "deviceCastSource";
const std::string PORT_KEY = "port";
const std::string SOURCE_IP_KEY = "sourceIp";
const std::string SINK_IP_KEY = "sinkIp";
const std::string TYPE_SESSION_KEY = "sessionKey";

constexpr int TRANSFER_MODE_SOFTBUS_SINGLE = 2;
constexpr int SESSION_KEY_LENGTH = 16;

const std::string VERSION = "OH1.0";
constexpr int OPERATION_CONSULT = 3;

constexpr int FIRST_PRIO_INDEX = 0;
constexpr int SECOND_PRIO_INDEX = 1;
constexpr int THIRD_PRIO_INDEX = 2;
constexpr int MAX_LINK_TYPE_NUM = 3;

/*
 * send to json key auth version, hichain 1.0 or 2.0
 */
const std::string AUTH_VERSION_KEY = "authVersion";
const std::string AUTH_VERSION_1 = "1.0";
const std::string AUTH_VERSION_2 = "2.0";

constexpr int THIRD_TV = 0x2E;

const std::string KEY_BIND_TARGET_ACTION = "action";
constexpr int ACTION_CONNECT_DEVICE = 0;
constexpr int ACTION_QUERY_P2P_IP = 1;
constexpr int ACTION_SEND_MESSAGE = 2;

const std::string KEY_LOCAL_P2P_IP = "localP2PIp";
const std::string KEY_REMOTE_P2P_IP = "remoteP2PIp";
const std::string NETWORK_ID = "networkId";

constexpr static int SECOND_BYTE_OFFSET = 8;
constexpr static int THIRD_BYTE_OFFSET = 16;
constexpr static int FOURTH_BYTE_OFFSET = 24;

constexpr static int INT_FOUR = 4;

void DeviceDiscoveryWriteWrap(const std::string& funcName, const std::string& puid)
{
    HiSysEventWriteWrap(funcName, {
        {"BIZ_SCENE", static_cast<int32_t>(BIZSceneType::DEVICE_DISCOVERY)},
        {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_END)},
        {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::DEVICE_DISCOVERY)},
        {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_SUCCESS)},
        {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
        {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
        {"LOCAL_SESS_NAME", ""},
        {"PEER_SESS_NAME", ""},
        {"PEER_UDID", puid}});
}

void EstablishConsultWriteWrap(const std::string& funcName, int sceneType, const std::string& puid)
{
    HiSysEventWriteWrap(funcName, {
            {"BIZ_SCENE", sceneType},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::ESTABLISH_CONSULT_SESSION)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_IDLE)},
            {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
            {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
            {"LOCAL_SESS_NAME", ""},
            {"PEER_SESS_NAME", ""},
            {"PEER_UDID", puid}});
}

void DeviceAuthWriteWrap(const std::string& funcName, int sceneType, const std::string& puid)
{
    HiSysEventWriteWrap(funcName, {
            {"BIZ_SCENE", sceneType},
            {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_BEGIN)},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::DEVICE_AUTHENTICATION)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_SUCCESS)},
            {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
            {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
            {"LOCAL_SESS_NAME", ""},
            {"PEER_SESS_NAME", ""},
            {"PEER_UDID", puid}});
}

void OnBindResultFailedWriteWrap(const std::string& funcName, int32_t result, const std::string& puid)
{
    auto errorCode = GetErrorCode(CAST_ENGINE_SYSTEM_ID, CAST_ENGINE_CAST_PLUS_MODULE_ID,
        static_cast<uint16_t>(result));
    HiSysEventWriteWrap(funcName, {
            {"BIZ_SCENE", static_cast<int32_t>(GetBIZSceneType(
                ConnectionManager::GetInstance().GetProtocolType()))},
            {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_END)},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::DEVICE_AUTHENTICATION)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_FAILED)},
            {"ERROR_CODE", errorCode}}, {
            {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
            {"LOCAL_SESS_NAME", ""},
            {"PEER_SESS_NAME", ""},
            {"PEER_UDID", puid}});
}
} // namespace

/*
* User's unusual action or other event scenarios could cause changing of STATE or RESULT which delivered
* by DM.
*/
const std::map<int, EventCode> CastBindTargetCallback::EVENT_CODE_MAP = {
    // ----- ON RESULT != 0 -----
    // SINK peer click distrust button during 3-state authentication.
    { ERR_DM_AUTH_PEER_REJECT, EventCode::ERR_DISTRUST_BY_SINK },
    // SINK peer click cancel button during pin code inputting.
    { ERR_DM_BIND_USER_CANCEL_PIN_CODE_DISPLAY, EventCode::ERR_CANCEL_BY_SINK },
    // SOURCE peer input wrong pin code up to 3 times
    { ERR_DM_BIND_PIN_CODE_ERROR, EventCode::ERR_PIN_CODE_RETRY_COUNT_EXCEEDED },
    // SOURCE peer click cancel button during pin code inputting.
    { ERR_DM_BIND_USER_CANCEL_ERROR, EventCode::EVT_CANCEL_BY_SOURCE },
    // SINK peer PIN code window closed.
    { ERR_DM_TIME_OUT, EventCode::ERR_SINK_TIMEOUT },
    // ----- ON RESULT == 0 -----
    // DEFAULT event
    { DmAuthStatus::STATUS_DM_AUTH_DEFAULT, EventCode::DEFAULT_EVENT },
    // Sink peer click trust during 3-state authentication.
    { DmAuthStatus::STATUS_DM_SHOW_PIN_INPUT_UI, EventCode::EVT_TRUST_BY_SINK },
    // Build connection successfully.
    { DmAuthStatus::STATUS_DM_AUTH_FINISH, EventCode::EVT_AUTHENTICATION_COMPLETED },
    // Waiting for user to click confirm
    { DmAuthStatus::STATUS_DM_SHOW_AUTHORIZE_UI, EventCode::EVT_SHOW_AUTHORIZE_UI }
};

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

DmDeviceInfo ConnectionManager::GetDmDeviceInfo(const std::string &deviceId)
{
    std::vector<DmDeviceInfo> trustedDevices;
    if (DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", trustedDevices) != DM_OK) {
        CLOGE("GetTrustedDeviceList fail");
        return {};
    }

    for (const auto &device : trustedDevices) {
        if (device.deviceId == deviceId) {
            return device;
        }
    }
    CLOGW("Can't find device");
    return {};
}

bool ConnectionManager::IsDeviceTrusted(const std::string &deviceId, std::string &networkId)
{
    std::vector<DmDeviceInfo> trustedDevices;
    if (DeviceManager::GetInstance().GetTrustedDeviceList(PKG_NAME, "", trustedDevices) != DM_OK) {
        CLOGE("GetTrustedDeviceList fail");
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

void ConnectionManager::SetProtocolType(int protocols)
{
    protocolType_ = protocols;
}

int ConnectionManager::GetProtocolType() const
{
    return protocolType_;
}

bool ConnectionManager::ConnectDevice(const CastInnerRemoteDevice &dev)
{
    DeviceDiscoveryWriteWrap(__func__, GetAnonymousDeviceID(dev.deviceId));

    auto &deviceId = dev.deviceId;
    CLOGI("ConnectDevice in, %s", deviceId.c_str());

    if (CastDeviceDataManager::GetInstance().IsDeviceUsed(deviceId)) {
        CLOGD("Device: %s is used.", deviceId.c_str());
        return true;
    }

    if (!UpdateDeviceState(deviceId, RemoteDeviceState::CONNECTING)) {
        CLOGE("Device(%s) is missing", deviceId.c_str());
        return false;
    }

    DiscoveryManager::GetInstance().StopDiscovery();

    std::string networkId;
    if (IsDeviceTrusted(dev.deviceId, networkId) && IsSingle(dev)) {
        DeviceAuthWriteWrap(__func__, GetBIZSceneType(GetProtocolType()), GetAnonymousDeviceID(dev.deviceId));
        if (!CastDeviceDataManager::GetInstance().SetDeviceNetworkId(deviceId, networkId) ||
            !OpenConsultSession(deviceId)) {
            (void)UpdateDeviceState(deviceId, RemoteDeviceState::FOUND);
            return false;
        }
        NotifySessionEvent(deviceId, ConnectEvent::AUTH_START);
        (void)UpdateDeviceState(deviceId, RemoteDeviceState::CONNECTED);
        return true;
    }
    NotifySessionEvent(deviceId, ConnectEvent::AUTH_START);
    HiSysEventWriteWrap(__func__, {
            {"BIZ_SCENE", GetBIZSceneType(GetProtocolType())},
            {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_BEGIN)},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::DEVICE_AUTHENTICATION)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_IDLE)},
            {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
            {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
            {"LOCAL_SESS_NAME", ""}, {"PEER_SESS_NAME", ""},
            {"PEER_UDID", GetAnonymousDeviceID(dev.deviceId)}});

    if (!BindTarget(dev)) {
        (void)UpdateDeviceState(deviceId, RemoteDeviceState::FOUND);
        return false;
    }
    if (isBindTargetMap_.find(deviceId) != isBindTargetMap_.end()) {
        isBindTargetMap_[deviceId] = true;
    } else {
        isBindTargetMap_.insert({ deviceId, true });
    }
    CLOGI("ConnectDevice out, %s", deviceId.c_str());
    return true;
}

bool ConnectionManager::BindTarget(const CastInnerRemoteDevice &dev)
{
    CLOGD("device info is %s, device name %s, customData %s", dev.deviceId.c_str(), dev.deviceName.c_str(),
        dev.customData.c_str());
    PeerTargetId targetId = {
        .deviceId = dev.deviceId,
        .bleMac = dev.bleMac,
        .wifiIp = dev.wifiIp,
        .wifiPort = dev.wifiPort,
    };
    std::map<std::string, std::string> bindParam;
    BuildBindParam(dev, bindParam);
    int ret = DeviceManager::GetInstance().BindTarget(PKG_NAME, targetId, bindParam,
        std::make_shared<CastBindTargetCallback>());
    if (ret == ERR_DM_AUTH_BUSINESS_BUSY) {
        CLOGE("bind fail, target is binding %d", ret);
        auto networkId = CastDeviceDataManager::GetInstance().GetDeviceNetworkId(dev.deviceId);
        PeerTargetId targetId = {
            .deviceId = *networkId,
        };
        std::map<std::string, std::string> unbindParam{};
        if (!CastDeviceDataManager::GetInstance().IsDoubleFrameDevice(dev.deviceId)) {
            DeviceManager::GetInstance().UnbindTarget(PKG_NAME, targetId, unbindParam, nullptr);
        } else {
            unbindParam.insert(
                std::pair<std::string, std::string>(PARAM_KEY_META_TYPE, std::to_string(5)));
            DeviceManager::GetInstance().UnbindTarget(PKG_NAME, targetId, unbindParam, nullptr);
        }
        return false;
    }

    if (ret != DM_OK) {
        CLOGE("ConnectDevice BindTarget fail, ret = %{public}d)", ret);
        CastEngineDfx::WriteErrorEvent(AUTHENTICATE_DEVICE_FAIL);
        return false;
    }

    CastDeviceDataManager::GetInstance().SetDeviceIsActiveAuth(dev.deviceId, true);
    CLOGI("Finish to BindTarget device!");
    return true;
}

bool ConnectionManager::BuildBindParam(const CastInnerRemoteDevice &device,
    std::map<std::string, std::string> &bindParam)
{
    CLOGI("start BuildBindParam");
    if (IsSingle(device)) { // bind target by dm
        bindParam[PARAM_KEY_AUTH_TYPE] = AUTH_WITH_PIN;
    } else { // bind target by meta node
        std::unique_ptr<CastLocalDevice> local = GetLocalDeviceInfo();
        if (local == nullptr) {
            CLOGE("CastLocalDevice is null");
            return false;
        }
        bindParam[DistributedHardware::PARAM_KEY_META_TYPE] = "5";
        bindParam[KEY_TRANSFER_MODE] = std::to_string(TRANSFER_MODE_SOFTBUS_SINGLE);
        bindParam[DEVICE_NAME_KEY] = local->deviceName;
        bindParam[AUTH_VERSION_KEY] = GetAuthVersion(device);
        bindParam[KEY_SESSION_ID] = std::to_string(device.sessionId);
        bindParam["udid"] = device.udid;
    }
    return true;
}

std::string ConnectionManager::GetAuthVersion(const CastInnerRemoteDevice &device)
{
    if (!device.bleMac.empty() && !device.customData.empty()) {
        return AUTH_VERSION_2;
    }
    return device.wifiPort == 0 ? AUTH_VERSION_1 : AUTH_VERSION_2;
}

bool ConnectionManager::QueryP2PIp(const CastInnerRemoteDevice &dev)
{
    CLOGI("query p2p ip method in ");
    auto temp = CastDeviceDataManager::GetInstance().GetDeviceByDeviceId(dev.deviceId);
    if (temp == std::nullopt) {
        CLOGE("GetDeviceByDeviceId the device(%s) is missing", dev.deviceId.c_str());
        return false;
    }

    CastInnerRemoteDevice device = *temp;

    PeerTargetId targetId;
    targetId.deviceId = device.deviceId;
    targetId.wifiIp = device.wifiIp;
    targetId.wifiPort = device.wifiPort;
    targetId.bleMac = device.bleMac;

    std::string localNetworkId = "";
    if (DeviceManager::GetInstance().GetLocalDeviceNetWorkId(PKG_NAME, localNetworkId) != 0) {
        CLOGI("GetLocalDeviceNetWorkId fail %s", localNetworkId.c_str());
        return false;
    }

    // The session can only be opened using a network ID instead of a UDID in OH system
    auto remoteNetworkId = CastDeviceDataManager::GetInstance().GetDeviceNetworkId(dev.deviceId);
    if (remoteNetworkId == std::nullopt) {
        return false;
    }
    std::map<std::string, std::string> bindParam;
    bindParam[DistributedHardware::PARAM_KEY_META_TYPE] = "5";
    bindParam[KEY_BIND_TARGET_ACTION] = std::to_string(ACTION_QUERY_P2P_IP);
    bindParam["localNetworkId"] = localNetworkId;
    bindParam["remoteNetworkId"] = remoteNetworkId.value();
    CLOGI("QueryP2PIp localNetworkId=%s, remoteNetworkId=%s", localNetworkId.c_str(), remoteNetworkId.value().c_str());
    DeviceManager::GetInstance().BindTarget(PKG_NAME, targetId, bindParam,
        std::make_shared<CastBindTargetCallback>());
    return true;
}

bool ConnectionManager::OpenConsultSession(const std::string &deviceId)
{
    CLOGI("start open consult session");
    // The session can only be opened using a network ID instead of a UDID in OH system
    auto networkId = CastDeviceDataManager::GetInstance().GetDeviceNetworkId(deviceId);
    if (networkId == std::nullopt) {
        return false;
    }

    EstablishConsultWriteWrap(__func__, GetBIZSceneType(GetProtocolType()), GetAnonymousDeviceID(deviceId));

    SessionAttribute attr{};
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
            auto errorCode = GetErrorCode(CAST_ENGINE_SYSTEM_ID, CAST_ENGINE_CAST_PLUS_MODULE_ID, OPEN_SESSION_FAIL);
            HiSysEventWriteWrap(__func__, {
                    {"BIZ_SCENE", GetBIZSceneType(GetProtocolType())},
                    {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::ESTABLISH_CONSULT_SESSION)},
                    {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_IDLE)},
                    {"ERROR_CODE", errorCode}}, {
                    {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
                    {"LOCAL_SESS_NAME", ""},
                    {"PEER_SESS_NAME", ""},
                    {"PEER_UDID", GetAnonymousDeviceID(deviceId)}});

            return false;
        }
    }
    if (!CastDeviceDataManager::GetInstance().SetDeviceTransId(deviceId, transportId)) {
        CloseSession(transportId);
        return false;
    }

    HiSysEventWriteWrap(__func__, {
            {"BIZ_SCENE", GetBIZSceneType(GetProtocolType())},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::ESTABLISH_CONSULT_SESSION)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_SUCCESS)},
            {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
            {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
            {"LOCAL_SESS_NAME", ""},
            {"PEER_SESS_NAME", ""},
            {"PEER_UDID", GetAnonymousDeviceID(deviceId)}});

    UpdateDeviceState(deviceId, RemoteDeviceState::CONNECTED);
    CLOGI("Out, sessionId = %{public}d", transportId);
    return true;
}

std::unique_ptr<CastLocalDevice> ConnectionManager::GetLocalDeviceInfo()
{
    CLOGI("GetLocalDeviceInfo in");
    DmDeviceInfo local;
    if (DeviceManager::GetInstance().GetLocalDeviceInfo(PKG_NAME, local) != DM_OK) {
        CLOGE("Cannot get the local device info from DM");
        return nullptr;
    };
    auto device = std::make_unique<CastLocalDevice>();
    device->deviceId = local.deviceId;
    device->deviceType = DeviceType::DEVICE_CAST_PLUS;
    device->deviceName = local.deviceName;
    CLOGI("GetLocalDeviceInfo out");
    return device;
}

void ConnectionManager::SendConsultInfo(const std::string &deviceId, int port)
{
    CLOGI("SendConsultInfo In");
    auto remote = CastDeviceDataManager::GetInstance().GetDeviceByDeviceId(deviceId);
    if (remote == std::nullopt) {
        CLOGE("Get remote device is empty");
        return;
    }

    SendConsultData(*remote, port);
}

void ConnectionManager::SendConsultData(const CastInnerRemoteDevice &device, int port)
{
    CLOGI("In");
    int transportId = CastDeviceDataManager::GetInstance().GetDeviceTransId(device.deviceId);
    if (transportId == INVALID_ID) {
        CLOGE("transport id is invalid");
        return;
    }

    json data;
    data[VERSION_KEY] = VERSION;
    data[OPERATION_TYPE_KEY] = OPERATION_CONSULT;
    data[SEQUENCE_NUMBER] = rand();
    json body;
    GetConsultationData(device, port, body);
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

std::string ConnectionManager::GetConsultationData(const CastInnerRemoteDevice &device, int port, json &body)
{
    auto local = GetLocalDeviceInfo();
    if (local == nullptr) {
        CLOGE("local device info is nullptr");
        return "";
    }

    body[DEVICE_ID_KEY] = local->deviceId;
    body[DEVICE_NAME_KEY] = local->deviceName;
    body[KEY_SESSION_ID] = device.sessionId;
    body[KEY_TRANSFER_MODE] = TRANSFER_MODE_SOFTBUS_SINGLE;

    ProtocolType protocolType;
    GetSessionProtocolType(device.sessionId, protocolType);
    body[PROTOCOL_TYPE_KEY] = protocolType;

    if (GetAuthVersion(device) == AUTH_VERSION_2) {
        body[TYPE_SESSION_KEY] = device.sessionKey;
    }

    CLOGI("Encrypt data, localIp %s, remote is %s, port %d", device.localIp.c_str(), device.remoteIp.c_str(), port);
    EncryptPort(port, device.sessionKey, body);
    EncryptIp(device.localIp, SOURCE_IP_KEY, device.sessionKey, body);
    EncryptIp(device.remoteIp, SINK_IP_KEY, device.sessionKey, body);
    return body.dump();
}

void ConnectionManager::EncryptPort(int port, const uint8_t *sessionKey, json &body)
{
    std::unique_ptr<uint8_t[]> portArray = intToByteArray(port);
    int portArraySize = 4;
    ConstPacketData inputData = { portArray.get(), portArraySize };

    uint8_t encryptedPort[portArraySize + EncryptDecrypt::AES_IV_LEN];
    PacketData outputData = { encryptedPort, 0 };
    EncryptDecrypt::GetInstance().EncryptData(EncryptDecrypt::CTR_CODE, sessionKey, SESSION_KEY_LENGTH, inputData,
        outputData);
    CLOGD("encrypt result is %d ", outputData.length);
    std::string encryptedPortLatin1(reinterpret_cast<const char *>(outputData.data), outputData.length);
    std::string encryptedPortUtf8 = convLatin1ToUTF8(encryptedPortLatin1);
    body[PORT_KEY] = encryptedPortUtf8;
}

std::string ConnectionManager::convLatin1ToUTF8(std::string &latin1)
{
    iconv_t cd = iconv_open("utf8", "iso88591");
    if (cd == (iconv_t)-1) {
        CLOGD("andy Failed to open iconv conversion descriptor");
        return "";
    }

    size_t inSize = latin1.size();
    size_t outSize = inSize * 2;
    std::string utf8(outSize, 0);

    char *inbuf = &latin1[0];
    char *outbuf = &utf8[0];
    size_t result = iconv(cd, &inbuf, &inSize, &outbuf, &outSize);
    if (result == (size_t)-1) {
        CLOGD("Failed to convert encoding");
        iconv_close(cd);
        return "";
    }

    iconv_close(cd);
    utf8.resize(outbuf - &utf8[0]);
    return utf8;
}

void ConnectionManager::EncryptIp(const std::string &ip, const std::string &key, const uint8_t *sessionKey, json &body)
{
    if (ip.empty()) {
        return;
    }
    ConstPacketData inputData = { reinterpret_cast<const uint8_t *>(ip.c_str()), ip.size() };
    uint8_t encrypted[ip.size() + EncryptDecrypt::AES_IV_LEN];
    PacketData outputData = { encrypted, 0 };
    EncryptDecrypt::GetInstance().EncryptData(EncryptDecrypt::CTR_CODE, sessionKey, SESSION_KEY_LENGTH, inputData,
        outputData);
    for (int i = 0; i < outputData.length; i++) {
        body[key].push_back(encrypted[i]);
    }
    CLOGI("encrypt %s finish", key.c_str());
}

std::unique_ptr<uint8_t[]> ConnectionManager::intToByteArray(int32_t num)
{
    std::unique_ptr<uint8_t[]> result = std::make_unique<uint8_t[]>(INT_FOUR);
    int i = 0;
    result[i] = (num >> FOURTH_BYTE_OFFSET) & 0xFF;
    result[++i] = (num >> THIRD_BYTE_OFFSET) & 0xFF;
    result[++i] = (num >> SECOND_BYTE_OFFSET) & 0xFF;
    result[++i] = num & 0xFF;
    return result;
}

void ConnectionManager::OnConsultSessionOpened(int transportId, bool isSource)
{
    std::thread([transportId, isSource]() {
        if (isSource) {
            auto device = CastDeviceDataManager::GetInstance().GetDeviceByTransId(transportId);
            if (device == std::nullopt) {
                return;
            }

            if (ConnectionManager::GetInstance().IsHuaweiDevice(*device)) {
                ConnectionManager::GetInstance().QueryP2PIp(*device);
            } else {
                ConnectionManager::GetInstance().NotifySessionEvent((*device).deviceId, ConnectEvent::AUTH_SUCCESS);
            }
            return;
        }
        ConnectionManager::GetInstance().GrabDevice();
        ConnectionManager::GetInstance().NotifySessionIsReady(transportId);
        }).detach();
    return;
}

void ConnectionManager::NotifySessionIsReady(int transportId)
{
    int castSessionId = listener_->NotifySessionIsReady();
    if (castSessionId == INVALID_ID) {
        CLOGE("sessionId is invalid");
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
    CLOGI("GetRemoteFromJsonData in");
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

    json remote;
    if (jsonObject[DATA_KEY].is_string()) {
        std::string dataString = jsonObject[DATA_KEY];
        remote = json::parse(dataString, nullptr, false);
    } else if (jsonObject[DATA_KEY].is_object()) {
        remote = jsonObject[DATA_KEY];
    } else {
        CLOGE("data key in json object is invalid!");
        return nullptr;
    }

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
    if (remote.contains(PROTOCOL_TYPE_KEY) && remote[PROTOCOL_TYPE_KEY].is_number()) {
        device->protocolType = remote[PROTOCOL_TYPE_KEY];
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
    int castSessionId = INVALID_ID;

    constexpr int32_t sleepTimeMs = 50;
    constexpr int32_t retryTimes = 20;
    int32_t retryTime = 0;

    while (castSessionId == INVALID_ID) {
        if (castSessionId != INVALID_ID || retryTime > retryTimes) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
        castSessionId = GetCastSessionId(transportId);
        retryTime++;
    }
    if (castSessionId == INVALID_ID) {
        CLOGE("session id invalid");
        return;
    }
    CLOGI("protocolType is %d", device->protocolType);
    if (device->protocolType == ProtocolType::CAST_PLUS_STREAM) {
        SetSessionProtocolType(castSessionId, device->protocolType);
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
    CLOGI("DestroyConsulationSession in");
    int transportId = CastDeviceDataManager::GetInstance().ResetDeviceTransId(deviceId);
    if (transportId != INVALID_ID) {
        CloseSession(transportId);
    }

    auto isSink = CastDeviceDataManager::GetInstance().GetDeviceRole(deviceId);
    if (isSink == std::nullopt || (*isSink)) {
        // The sink's Server is only removed when DisableDiscoverable or Deinit is performed.
        return;
    }
}

void ConnectionManager::DisconnectDevice(const std::string &deviceId)
{
    DiscoveryManager::GetInstance().StopDiscovery();
    if (!CastDeviceDataManager::GetInstance().IsDeviceUsed(deviceId)) {
        CLOGE("Device(%s) is not used, remove it", deviceId.c_str());
        CastDeviceDataManager::GetInstance().UpdateDeivceByDeviceId(deviceId);
        return;
    }

    DestroyConsulationSession(deviceId);
    auto isActiveAuth = CastDeviceDataManager::GetInstance().GetDeviceIsActiveAuth(deviceId);
    if (isActiveAuth == std::nullopt) {
        return;
    }
    auto networkId = CastDeviceDataManager::GetInstance().GetDeviceNetworkId(deviceId);
    if (networkId == std::nullopt) {
        return;
    }

    CastDeviceDataManager::GetInstance().UpdateDeivceByDeviceId(deviceId);
}

bool ConnectionManager::EnableDiscoverable()
{
    std::lock_guard lock(mutex_);
    if (isDiscoverable_) {
        CLOGW("service has been set discoverable");
        return true;
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

    isDiscoverable_ = false;
    return true;
}

void ConnectionManager::GrabDevice()
{
    CLOGI("GrabDevice in");
    if (grabState_ == DeviceGrabState::NO_GRAB) {
        return;
    }
    if (listener_ == nullptr) {
        return;
    }
    listener_->GrabDevice(sessionId_);
}

void ConnectionManager::UpdateGrabState(bool changeState, int32_t sessionId)
{
    CLOGI("GrabDevice in");
    std::lock_guard<std::mutex> lock(mutex_);
    sessionId_ = sessionId;
    if (changeState) {
        grabState_ = DeviceGrabState::GRAB_ALLOWED;
        return;
    }
    grabState_ = DeviceGrabState::NO_GRAB;
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
    SetSessionListener(nullptr);
}

bool ConnectionManager::UpdateDeviceState(const std::string &deviceId, RemoteDeviceState state)
{
    CLOGD("UpdateDeviceState: %s", REMOTE_DEVICE_STATE_STRING[static_cast<size_t>(state)].c_str());
    return CastDeviceDataManager::GetInstance().SetDeviceState(deviceId, state);
}

void ConnectionManager::ReportErrorByListener(const std::string &deviceId, EventCode currentEventCode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!listener_) {
        return;
    }
    listener_->OnEvent(deviceId, currentEventCode);
}

int32_t ConnectionManager::GetSessionProtocolType(int sessionId, ProtocolType &protocolType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!listener_) {
        return CAST_ENGINE_ERROR;
    }
    return listener_->GetSessionProtocolType(sessionId, protocolType);
}

int32_t ConnectionManager::SetSessionProtocolType(int sessionId, ProtocolType protocolType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!listener_) {
        return CAST_ENGINE_ERROR;
    }
    return listener_->SetSessionProtocolType(sessionId, protocolType);
}

bool ConnectionManager::NotifySessionEvent(const std::string &deviceId, int result)
{
    CLOGI("NotifySessionEvent in");
    std::lock_guard<std::mutex> lock(mutex_);
    if (sessionListener_ == nullptr) {
        CLOGE("sessionListener is NULL");
        return false;
    }
    sessionListener_->NotifySessionEvent(deviceId, result);
    return true;
}

void ConnectionManager::NotifyDeviceIsOffline(const std::string &deviceId)
{
    CLOGI("NotifyDeviceIsOffline in");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!listener_) {
        return;
    }
    listener_->NotifyDeviceIsOffline(deviceId);
}

void ConnectionManager::SetSessionListener(std::shared_ptr<IConnectManagerSessionListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessionListener_ = listener;
}

void CastBindTargetCallback::OnBindResult(const PeerTargetId &targetId, int32_t result, int32_t status,
    std::string content)
{
    CLOGI("OnBindResult, device id:%s, content:%s, status: %{public}d, result: %{public}d", targetId.deviceId.c_str(),
        content.c_str(), status, result);
    json authInfo;
    if (authInfo.accept(content)) {
        authInfo = json::parse(content, nullptr, false);
        int action = -1;
        if (authInfo.contains(KEY_BIND_TARGET_ACTION) && authInfo[KEY_BIND_TARGET_ACTION].is_number()) {
            action = authInfo[KEY_BIND_TARGET_ACTION];
        }
        if (result == DM_OK && status == DmAuthStatus::STATUS_DM_AUTH_FINISH && action != -1) {
            HandleBindAction(targetId, action, authInfo);
            if (action != ACTION_CONNECT_DEVICE) {
                CLOGI("bindtarget action %d handle finish, return", action);
                return;
            }
        }
    }

    EventCode currentEventCode;
    // Non-zero RESULT as FIRST consideration, and STATE secondarily.
    if (result != 0) {
        CastEngineDfx::WriteErrorEvent(AUTHENTICATE_DEVICE_FAIL);
        OnBindResultFailedWriteWrap(__func__, result, GetAnonymousDeviceID(targetId.deviceId));
        currentEventCode = EVENT_CODE_MAP.count(result) ? EVENT_CODE_MAP.at(result) : EventCode::UNKNOWN_EVENT;
    } else {
        if (EVENT_CODE_MAP.count(status)) {
            currentEventCode = EVENT_CODE_MAP.at(status);
        } else {
            CLOGI("unknown status %d, return ", status);
            return;
        }
        HiSysEventWriteWrap(__func__, {
                {"BIZ_SCENE", static_cast<int32_t>(GetBIZSceneType(
                    ConnectionManager::GetInstance().GetProtocolType()))},
                {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_END)},
                {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::DEVICE_AUTHENTICATION)},
                {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_SUCCESS)},
                {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
                {"TO_CALL_PKG", DEVICE_MANAGER_NAME},
                {"LOCAL_SESS_NAME", ""},
                {"PEER_SESS_NAME", ""},
                {"PEER_UDID", GetAnonymousDeviceID(targetId.deviceId)}});
    }
    CLOGI("EventCode: %{public}d", static_cast<int32_t>(currentEventCode));
    ConnectionManager::GetInstance().ReportErrorByListener(targetId.deviceId, currentEventCode);
}

void CastBindTargetCallback::HandleBindAction(const PeerTargetId &targetId, int action, const json &authInfo)
{
    CLOGI("action is %d", action);
    switch (action) {
        case ACTION_CONNECT_DEVICE: {
            HandleConnectDeviceAction(targetId, authInfo);
            return;
        }
        case ACTION_QUERY_P2P_IP: {
            HandleQueryIpAction(targetId, authInfo);
            return;
        }
        case ACTION_SEND_MESSAGE: {
            CLOGI("action operate 3 send message");
            return;
        }
        default: {
            CLOGW("unknow action %d", action);
            return;
        }
    }
}

void CastBindTargetCallback::HandleConnectDeviceAction(const PeerTargetId &targetId, const json &authInfo)
{
    CLOGI("handle connect device action");
    if (authInfo.contains(NETWORK_ID) && !authInfo[NETWORK_ID].is_string()) {
        CLOGE("networkId json data is not string");
        return;
    }
    
    const std::string networkId = authInfo[NETWORK_ID];
    const std::string deviceId = targetId.deviceId;
    if (!CastDeviceDataManager::GetInstance().SetDeviceNetworkId(targetId.deviceId, networkId)) {
        return;
    }
    
    if (authInfo.contains(KEY_TRANSFER_MODE) && authInfo[KEY_TRANSFER_MODE].is_number()) {
        int mode = authInfo[KEY_TRANSFER_MODE];
        ChannelType type = mode == TRANSFER_MODE_SOFTBUS_SINGLE ? ChannelType::SOFT_BUS : ChannelType::LEGACY_CHANNEL;
        CastDeviceDataManager::GetInstance().SetDeviceChannleType(deviceId, type);
    }
    
    if (authInfo.contains(AUTH_VERSION_KEY) && authInfo[AUTH_VERSION_KEY].is_string()) {
        std::string authVersion = authInfo[AUTH_VERSION_KEY];
        CLOGE("authVersion is %s", authVersion.c_str());
        if (authVersion == AUTH_VERSION_1) {
            // 获取sessionKey
            uint8_t sessionKey[SESSION_KEY_LENGTH] = {0};
            bool result = GetSessionKey(authInfo, sessionKey);
            if (!result) {
                CLOGE("auth version 1.0, get sessionkey fail");
                return;
            }
            result = CastDeviceDataManager::GetInstance().SetDeviceSessionKey(deviceId, sessionKey);
            CLOGD("auth version 1.0, set sessionkey result is %d", result);
            ConnectionManager::GetInstance().NotifySessionEvent(deviceId, ConnectEvent::AUTH_SUCCESS);
        } else {
            uint8_t sessionKey[SESSION_KEY_LENGTH] = {0};
            RAND_bytes(sessionKey, SESSION_KEY_LENGTH);
            bool result = CastDeviceDataManager::GetInstance().SetDeviceSessionKey(deviceId, sessionKey);
            CLOGD("auth version 2.0, set sessionkey result is %d", result);
            ConnectionManager::GetInstance().OpenConsultSession(deviceId);
        }
    }
}

bool CastBindTargetCallback::GetSessionKey(const json &authInfo, uint8_t *sessionKey)
{
    if (authInfo.contains(TYPE_SESSION_KEY) && authInfo[TYPE_SESSION_KEY].is_array()) {
        for (int i = 0; i < SESSION_KEY_LENGTH; i++) {
            sessionKey[i] = authInfo[TYPE_SESSION_KEY][i];
            CLOGD("get session key auth version 1 %d", static_cast<uint8_t>(authInfo[TYPE_SESSION_KEY][i]));
        }
        return true;
    } else {
        CLOGE("get sessionkey from json data fail");
        return false;
    }
}

void CastBindTargetCallback::HandleQueryIpAction(const PeerTargetId &targetId, const json &authInfo)
{
    CLOGI("query p2p finish, notify session auth success");
    auto remote = CastDeviceDataManager::GetInstance().GetDeviceByDeviceId(targetId.deviceId);
    if (remote == std::nullopt) {
        CLOGE("Get remote device is empty");
        return;
    }
    std::string localIp;
    std::string remoteIp;
    if (authInfo.contains(KEY_LOCAL_P2P_IP) && authInfo[KEY_LOCAL_P2P_IP].is_string()) {
        localIp = authInfo[KEY_LOCAL_P2P_IP];
    }
    
    if (authInfo.contains(KEY_REMOTE_P2P_IP) && authInfo[KEY_REMOTE_P2P_IP].is_string()) {
        remoteIp = authInfo[KEY_REMOTE_P2P_IP];
    }
    CastDeviceDataManager::GetInstance().SetDeviceIp(targetId.deviceId, localIp, remoteIp);
    ConnectionManager::GetInstance().NotifySessionEvent(targetId.deviceId, ConnectEvent::AUTH_SUCCESS);
}

void CastUnBindTargetCallback::OnUnbindResult(const PeerTargetId &targetId, int32_t result, std::string content)
{
    CLOGI("OnUnbindResult,device id:%s, result: %d, content: %s", targetId.deviceId.c_str(), result, content.c_str());
}

void CastDeviceStateCallback::OnDeviceOnline(const DmDeviceInfo &deviceInfo)
{
    CLOGI("device(%s) is online", deviceInfo.deviceId);
    DiscoveryManager::GetInstance().NotifyDeviceIsOnline(deviceInfo);
    std::string deviceId = std::string(deviceInfo.deviceId);
    if (ConnectionManager::GetInstance().isBindTargetMap_.find(deviceId) ==
        ConnectionManager::GetInstance().isBindTargetMap_.end()) {
        return;
    }

    CLOGD("Online for bind target, networkId:%s", deviceInfo.networkId);
    ConnectionManager::GetInstance().isBindTargetMap_.erase(deviceId);
    if (!CastDeviceDataManager::GetInstance().SetDeviceNetworkId(deviceId, deviceInfo.networkId)) {
        return;
    }
    auto isActiveAuth = CastDeviceDataManager::GetInstance().GetDeviceIsActiveAuth(deviceId);
    if (isActiveAuth == std::nullopt || !(*isActiveAuth)) {
        return;
    }
    auto remote = CastDeviceDataManager::GetInstance().GetDeviceByDeviceId(deviceId);
    if (remote == std::nullopt) {
        CLOGE("Get remote device is empty");
        return;
    }
    if (!ConnectionManager::GetInstance().IsSingle(*remote)) {
        CLOGI("current device is not single device %s ", deviceId.c_str());
        return;
    }
    ConnectionManager::GetInstance().OpenConsultSession(deviceId);
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
    CLOGI("device(%s) is ready", deviceInfo.deviceId);
}

void ConnectionManager::SetRTSPPort(int port)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rtspPort_ = port;
}

bool ConnectionManager::IsSingle(const CastInnerRemoteDevice &device)
{
    if (device.deviceTypeId == THIRD_TV) {
        return false;
    }

    if (device.customData.empty() && device.wifiPort == 0 && device.bleMac.empty()) {
        return true;
    }

    if (device.customData.empty()) {
        return device.wifiPort != 0 || !device.bleMac.empty();
    }
    return false;
}

bool ConnectionManager::IsHuaweiDevice(const CastInnerRemoteDevice &device)
{
    if (!device.customData.empty()) {
        return true;
    }
    return false;
}

bool ConnectionManager::IsThirdDevice(const CastInnerRemoteDevice &device)
{
    if (device.deviceTypeId == THIRD_TV) {
        return true;
    }
    return device.bleMac.empty() && device.wifiPort == 0;
}

int ConnectionManager::GetRTSPPort()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return rtspPort_;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
