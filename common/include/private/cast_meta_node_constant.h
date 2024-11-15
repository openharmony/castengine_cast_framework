 /*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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
 * Author: chenggong
 * Create: 2023-10-23
 */

#ifndef CAST_META_NODE_CONSTANT_H
#define CAST_META_NODE_CONSTANT_H

#include <string>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {

const std::string KEY_BIND_TARGET_ACTION = "action";
constexpr int ACTION_CONNECT_DEVICE = 0;
constexpr int ACTION_QUERY_P2P_IP = 1;
constexpr int ACTION_SEND_MESSAGE = 2;
constexpr int ACTION_FINISH_BINDING_STATUS = 3;

const std::string KEY_LOCAL_NETWORK_ID = "localNetworkId";
const std::string KEY_REMOTE_NETWORK_ID = "remoteNetworkId";
const std::string KEY_LOCAL_WIFI_IP = "localWifiIp";
const std::string KEY_REMOTE_WIFI_IP = "remoteWifiIp";
const std::string KEY_LOCAL_P2P_IP = "localP2PIp";
const std::string KEY_REMOTE_P2P_IP = "remoteP2PIp";
const std::string NETWORK_ID = "networkId";
const std::string KEY_TRANSFER_MODE = "transferMode";
const std::string KEY_SESSION_ID = "sessionId";
const std::string PORT_KEY = "port";
const std::string SOURCE_IP_KEY = "sourceIp";
const std::string SINK_IP_KEY = "sinkIp";
const std::string TYPE_SESSION_KEY = "sessionKey";
const std::string KEY_CONNECT_TYPE = "connectType";
const std::string CONNECT_TYPE_WIFI = "lan"; // Do not modify the string
const std::string CONNECT_TYPE_P2P = "p2p"; // Do not modify the string
const std::string KEY_PROJECTION_MODE = "projectionMode";
const std::string PROJECTION_MODE_MIRROR = "MIRROR";
const std::string PROJECTION_MODE_STREAM = "MEDIA_RESOURCE";
const std::string KEY_UNBIND_TARGET_CAUSE = "unbindCause";

constexpr int CAUSE_AUTH_SUCCESS = 0;
constexpr int CAUSE_AUTH_FAILED = 1;
constexpr int CAUSE_DISCONNECT = 2;
inline constexpr int OK = 0;
inline constexpr int INVALID_VALUE = -1;

constexpr int AUTH_MODE_GENERIC = 1;
constexpr int AUTH_MODE_PWD = 2;

constexpr int CAST_SESSION_KEY_LENGTH = 16;

const std::string TIME_RECORD = "TimeRecord";
const std::string TOTAL_AUTH_TIME = "totalAuthTime";
const std::string TIME_RECORD_STRING = "timeRecordString";

const std::string VERSION_OH = "OH1.0";

const std::string PACKAGE_NAME_KEY = "packageName";
const std::string VERSION_NAME_KEY = "castpluskitVersionName";
const std::string PRODUCT_NAME_KEY = "productName";
const std::string CAST_PLUS_APP_VERSION_KEY = "castplusAppVersion";
const std::string PRODUCT_ID_KEY = "productId";
const std::string IS_VIRGIN_SINK = "isVirginSink";
const std::string ALLOWED_ALWAYS = "allowedAlways";
const std::string DEVICE_CAST_SOURCE = "deviceCastSource";
const std::string AUTH_MODE_KEY = "authMode";
const std::string VERSION_KEY = "version";
const std::string OPERATION_TYPE_KEY = "operType";
const std::string SEQUENCE_NUMBER = "sequenceNumber";
const std::string DATA_KEY = "data";
const std::string DEVICE_SALT_KEY = "salt";
const std::string CONNECT_TYPE_PRIORITY_KEY = "ConnectTypePriority";
const std::string TRIGGER_TYPE_KEY = "triggerType";
const std::string HANDSHAKE_RESULT_KEY = "handshakeResult";
const std::string DEVICE_IP_KEY = "deviceIp";
const std::string CURRENT_TIME_KEY = "currentTime";
const std::string KIT_AUTH_STATUS_KEY = "kitAuthStatus";
const std::string KIT_AUTH_POLICY_KEY = "kitAuthPolicy";
const std::string IS_GENERIC_TRUSTED_KEY = "isGenericTrusted";
const std::string IS_PWD_TRUSTED_KEY = "isPwdTrusted";
const std::string IS_SAME_ACCOUNT = "isSameAccount";
const std::string IS_SAME_FAMILY_GROUP = "isSameFamilyGroup";
const std::string IS_CONFIRMED_KEY = "isConfirmed";
const std::string DEVICE_ONLINE_INFO_KEY = "onlineInfo";
const std::string COORDINATION_CAPABILITY_KEY = "hasCoordinationCapability";
const std::string KIT_AUTH_CHOICE_KEY = "kitAuthChoice";
const std::string PHONE_MODEL_VERSION_KEY = "phoneModelVersion";
const std::string MODEL_NAME_KEY = "modelName";
const std::string CONSULT_RESULT = "consultResult";
const std::string SUB_REASON = "subReason";
const std::string SN_KEY = "serial_number";
const std::string CAPABILITY_INFO_KEY = "capabilityInfo";
// consult key
const std::string DEVICE_ID_KEY = "deviceId";
const std::string DEVICE_NAME_KEY = "deviceName";
const std::string REMOTE_DEVICE_NAME_KEY = "remoteDeviceName";
const std::string PROTOCOL_TYPE_KEY = "protocolType";
const std::string ACCOUNT_ID_KEY = "accountId";
const std::string USER_ID_KEY = "userId";

const std::string KIT_VER_PREFIX = "castpluskit";
// Prefix of third party castplus kit
const std::string KIT_VER_1X = "castpluskit 1";
// Prefix of HMOS2.x castplus kit
const std::string KIT_VER_2X = "castpluskit 2";

enum OperationStep : int32_t {
    OPERATION_HANDSHAKE = 1,
    OPERATION_AUTHENTICATE = 2,
    OPERATION_CONSULT = 3,
};

enum DmAuthStatusExt : int32_t {
    HANDSHAKE_RESPONSE = 1000,
};

}  // namespace CastEngineService
}  // namespace CastEngine
}  // namespace OHOS

#endif