/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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
 * Author: lijianzhao
 * Create: 2024-07-28
 */

#ifndef RADAR_CONSTANTS_H
#define RADAR_CONSTANTS_H
#include <string>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {

struct BasicInfo {
    std::string func;
    int32_t bizStage;
    int32_t stageRes;
    int32_t bizState;
    int32_t errorCode = 0;
};

struct LocalAndPeerDeviceInfo {
    std::string localUdid;
    std::string localNetId;
    std::string localDevType;
    std::string localWifiMac;
    std::string localIp;
    std::string localPort;

    std::string peerUdid;
    std::string peerNetId;
    std::string peerDevType;
    std::string peerWifiMac;
    std::string peerIp;
    std::string peerPort;
    std::string peerVer;
    std::string peerDeviceVer;
    int32_t triggerType;
};

struct CommonEventForm {
    BasicInfo basicInfo;
    LocalAndPeerDeviceInfo deviceInfo;
};

/*
 * moduleID range 0~31
 * 1~6 for HMOS4.x and before
 * 0,7~9 reserved
 * 10~18 for cast+
 * 18~31 for miracast and DLNA
 */
enum class RadarModuleID : int32_t {
    DISCOVERY = 10,
    CONNECTION = 11,
    CAST_SESSION = 12,
    RTSP = 13,
    VIDEO = 14,
    AUDIO = 15,
    REMOTE_CTRL = 16,
    STREAM = 17,
    UICAST = 18,
    MIRACAST_CONNECTION = 19
};

/*
 * 1~99 for castplus
 * 100~199 for miracast
 * 200~299 for DLNA
 */
enum class BizScene : int32_t {
    DISCOVERY_CAST = 1,
    CONNECTION_CAST_MIRROR = 2,
    CONNECTION_CAST_STREAM = 3,
    CONNECTION_COLLABORATION = 4,
    CONNECTION_CAST_MIRROR_LEAGACY = 5,
    CONNECTION_CAST_STREAM_LEAGACY = 6,
    CONNECTION_COLLABORATION_LEAGACY = 7,
    DISCOVERY_MIRRACAST = 100,
    CONNECTION_MIRRACAST = 101,
    DISCOVERY_DLNA = 200,
    CONNECTION_DLNA = 201,
};

enum DiscoverStage : int32_t {
    S1_START_DISCOVERY = 1,
    S2_DISCOVERYING = 2,
};

enum ConnectionStage : int32_t {
    S1_START_CONNECTION = 1,
    S2_AUTH_CHANEL = 2,
    S3_SELECT_TRUST = 3,
    S4_PIN_CODE_BIND = 4,
    S5_NETWORKING_ONLINE = 5,
    S6_OPEN_CONSULT_CHANNEL = 6,
    S7_QUERY_P2P_IP = 7,
    S8_RSTP_CREATE_PORT = 8,
    S9_CONSULT_DATA = 9,
    S10_RTSP_NEGOTIATE = 10,
    S11_AUDIO_VIDEO_CHANNELS = 11,
    S12_AVCODEC_INIT = 12,
    S13_AVCODEC_START = 13,
    S11_STREAM_CHANNEL = 14,
};

enum TriggerMode : int32_t {
    TRIGGER_BT = 1,
    TRIGGER_WIFI = 2,
};

static constexpr char DOMAIN_CAST_ENGINE[] = "CAST_ENGINE";
static constexpr char DOMAIN_PROJECTION_UE[] = "PROJECTION_UE";  // for UE dfx
static const std::string EVENT_NAME = "CAST_PLUS_BEHAVIOR";

static const std::string PKG_CAST_ENGINE = "cast_engine_service";
static const std::string PKG_DEVICE_MANAGER = "deviceManager";
static const std::string PKG_DSOFTBUS = "dsoftbus";
static const std::string PKG_COLLABORATION_SERVICE = "collaboration_service";
static const std::string PKG_AVSESSION = "av_session";
static const std::string PKG_COLLABORATION_FWK = "CollaborationFwk";

static const std::string REMOTE_VER_THIRD_PARTY = "third_party";

static constexpr uint8_t SUBSYSTEM_CAST_ENGINE = 208;
static constexpr uint32_t MASK_SUBSYSTEM = 0xff;
static constexpr uint32_t MASK_MODULE_ID = 0x1f;
static constexpr uint32_t MASK_ERROR_CODE = 0xffff;
static constexpr uint8_t OFFSET_MODULE_ID = 16;
static constexpr uint8_t OFFSET_SUBSYSTEM = 21;

#define CAST_ENGINE_ERR_CODE(moduleID) \
    ((SUBSYSTEM_CAST_ENGINE << OFFSET_SUBSYSTEM) | (static_cast<uint32_t>(moduleID) << OFFSET_MODULE_ID))

enum RadarErrCode : int32_t {
    SUCCESS = 0,

    //@RadarModuleID::DISCOVERY error code, base is 436862976
    DISCOVERY_BASE = CAST_ENGINE_ERR_CODE(RadarModuleID::DISCOVERY),
    DISCOVERY_NO_LISTENER = DISCOVERY_BASE + 1,
    DISCOVERY_NO_HANDLER = DISCOVERY_BASE + 2,

    //@RadarModuleID::CONNECTION error code, base is 436928512
    CONNECTION_BASE = CAST_ENGINE_ERR_CODE(RadarModuleID::CONNECTION),
    CONNECTION_IP_ADDRESS_INVAILD = CONNECTION_BASE + 1,
    CONNECTION_DEVICE_IS_MISSING = CONNECTION_BASE + 2,
    CONNECTION_NETWORK_ID_INVAILD = CONNECTION_BASE + 3,
    CONNECTION_NO_SESSION_KEY = CONNECTION_BASE + 4,
    CONNECTION_CONSULT_FAIL_FROM_SINK = CONNECTION_BASE + 5,
    CONNECTION_DEVICE_LOST = CONNECTION_BASE + 6,

    // error code for meta node
    CONNECTION_META_UNKNOW_BIND_ACTION = CONNECTION_BASE + 1000,
    CONNECTION_META_AUTH_INIT_FAIL = CONNECTION_BASE + 1001,
    CONNECTION_META_GET_AUTH_CHANNEL_FAIL = CONNECTION_BASE + 1002,
    CONNECTION_META_NO_CONSULT_DATA = CONNECTION_BASE + 1003,
    CONNECTION_META_NO_AUTH_VERSION = CONNECTION_BASE + 1004,
    CONNECTION_META_FAIL_FROM_SINK = CONNECTION_BASE + 1005,
    CONNECTION_META_GET_GM_INSTANCE_FAIL = CONNECTION_BASE + 1006,
    CONNECTION_META_MEMCPY_FAIL = CONNECTION_BASE + 1007,
    CONNECTION_META_GET_HICHAIN_INTERFACE_FAIL = CONNECTION_BASE + 1008,
    CONNECTION_META_GET_HICHAIN_INSTANCE_FAIL = CONNECTION_BASE + 1009,
    CONNECTION_META_NO_WIFI_BLE_ADDR = CONNECTION_BASE + 1010,
    CONNECTION_META_DEVICE_IS_BUSY = CONNECTION_BASE + 1011,

    //@RadarModuleID::CAST_SESSION error code, base is 436994048
    SESSION_BASE = CAST_ENGINE_ERR_CODE(RadarModuleID::CAST_SESSION),
    SESSION_DEVICE_IS_MISS = SESSION_BASE + 1,
    SESSION_RTSP_START_FAIL = SESSION_BASE + 2,
    SESSION_RTSP_BUILD_REQ_FAIL = SESSION_BASE + 3,
    SESSION_MEDIA_START_FAIL = SESSION_BASE + 4,
    SESSION_SETUP_FAIL = SESSION_BASE + 5,
    SESSION_CONNECT_TIMEOUT = SESSION_BASE + 6,
    // todo, temp define
    SESSION_MULTI_CONNECT_FAIL = SESSION_BASE + 100,

    //@RadarModuleID::RTSP error code, base is 437059584
    RTSP_BASE = CAST_ENGINE_ERR_CODE(RadarModuleID::RTSP),
    RTSP_ANNOUNCE_SEND_FAIL = RTSP_BASE + 1,
    RTSP_ANNOUNCE_REQUEST_FAIL = RTSP_BASE + 2,
    RTSP_M1_SEND_FAIL = RTSP_BASE + 3,
    RTSP_M3_SEND_FAIL = RTSP_BASE + 4,
    RTSP_M3_RECV_FAIL = RTSP_BASE + 5,
    RTSP_M4_SEND_FAIL = RTSP_BASE + 6,
    RTSP_M4_RECV_FAIL = RTSP_BASE + 7,
    RTSP_M5_RECV_FAIL = RTSP_BASE + 8,
    RTSP_M6_RECV_FAIL = RTSP_BASE + 9,
    RTSP_RENDER_READY_SEND_FAIL = RTSP_BASE + 10,
    RTSP_RENDER_READY_RECV_FAIL = RTSP_BASE + 11,
    RTSP_PAUSE_SEND_FAIL = RTSP_BASE + 12,
    RTSP_PAUSE_RECV_FAIL = RTSP_BASE + 13,
    RTSP_PLAY_SEND_FAIL = RTSP_BASE + 14,
    RTSP_PLAY_REQUEST_FAIL = RTSP_BASE + 15,
    RTSP_TEARDOWN_SEND_FAIL = RTSP_BASE + 16,
    RTSP_TEARDOWN_RECV_FAIL = RTSP_BASE + 17,

    //@RadarModuleID::VIDEO error code, base is 437125120
    VIDEO_BASE = CAST_ENGINE_ERR_CODE(RadarModuleID::VIDEO),
    VIDEO_ENCODER_IS_NULL = VIDEO_BASE + 1,
    VIDEO_ENCODER_START_FAIL = VIDEO_BASE + 2,
    VIDEO_DEFAULTDISPLAY_IS_NULLPTR = VIDEO_BASE + 3,
    VIDEO_SET_VIRTUAL_SCREEN_SURFACE_FAIL = VIDEO_BASE + 4,
    VIDEO_SCREEN_MANAGER_REGISTER_LISTENER_FAIL = VIDEO_BASE + 5,

    //@RadarModuleID::AUDIO error code, base is 437190656
    AUDIO_BASE = CAST_ENGINE_ERR_CODE(RadarModuleID::AUDIO),
    AUDIO_ENCODER_START_FAIL = AUDIO_BASE + 1,
    AUDIO_CAPTURE_START_FAIL = AUDIO_BASE + 2,

    //@RadarModuleID::MIRACAST error code, base is 437452800
    WIFI_DISPLAY_BASE = CAST_ENGINE_ERR_CODE(RadarModuleID::MIRACAST_CONNECTION),
    WIFI_DISPLAY_GENERAL_ERROR = WIFI_DISPLAY_BASE + 1,
    WIFI_DISPLAY_BAD_PARAMETER = WIFI_DISPLAY_BASE + 2,
    WIFI_DISPLAY_P2P_STATE_EXCEPTION = WIFI_DISPLAY_BASE + 3,
    WIFI_DISPLAY_P2P_CONNECTION_FAILURE = WIFI_DISPLAY_BASE + 4,
    WIFI_DISPLAY_P2P_CONNECTION_TIMEOUT = WIFI_DISPLAY_BASE + 5,
    WIFI_DISPLAY_NETWORK_ERROR = WIFI_DISPLAY_BASE + 6,
    WIFI_DISPLAY_RTSP_KEEPALIVE_TIMEOUT = WIFI_DISPLAY_BASE + 7,
    WIFI_DISPLAY_RTSP_CONNECTION_TIMEOUT = WIFI_DISPLAY_BASE + 8,
    WIFI_DISPLAY_RTSP_INTERACTION_TIMEOUT = WIFI_DISPLAY_BASE + 9,
    WIFI_DISPLAY_RTSP_SOCKET_ERROR = WIFI_DISPLAY_BASE + 10,
    WIFI_DISPLAY_P2P_OPT_FAILED = WIFI_DISPLAY_BASE + 11,
    WIFI_DISPLAY_P2P_OPT_NOT_SUPPORTED = WIFI_DISPLAY_BASE + 12,
    WIFI_DISPLAY_P2P_OPT_INVALID_PARAM = WIFI_DISPLAY_BASE + 13,
    WIFI_DISPLAY_P2P_OPT_FORBID_AIRPLANE = WIFI_DISPLAY_BASE + 14,
    WIFI_DISPLAY_P2P_OPT_FORBID_POWSAVING = WIFI_DISPLAY_BASE + 15,
    WIFI_DISPLAY_P2P_OPT_PERMISSION_DENIED = WIFI_DISPLAY_BASE + 16,
    WIFI_DISPLAY_P2P_OPT_OPEN_FAIL_WHEN_CLOSING = WIFI_DISPLAY_BASE + 17,
    WIFI_DISPLAY_P2P_OPT_OPEN_SUCC_WHEN_OPENED = WIFI_DISPLAY_BASE + 18,
    WIFI_DISPLAY_P2P_OPT_CLOSE_FAIL_WHEN_OPENING = WIFI_DISPLAY_BASE + 19,
    WIFI_DISPLAY_P2P_OPT_CLOSE_SUCC_WHEN_CLOSED = WIFI_DISPLAY_BASE + 20,
    WIFI_DISPLAY_P2P_OPT_STA_NOT_OPENED = WIFI_DISPLAY_BASE + 21,
    WIFI_DISPLAY_P2P_OPT_SCAN_NOT_OPENED = WIFI_DISPLAY_BASE + 22,
    WIFI_DISPLAY_P2P_OPT_AP_NOT_OPENED = WIFI_DISPLAY_BASE + 23,
    WIFI_DISPLAY_P2P_OPT_INVALID_CONFIG = WIFI_DISPLAY_BASE + 24,
    WIFI_DISPLAY_P2P_OPT_P2P_NOT_OPENED = WIFI_DISPLAY_BASE + 25,
    WIFI_DISPLAY_P2P_OPT_P2P_MAC_NOT_FOUND = WIFI_DISPLAY_BASE + 26,
    WIFI_DISPLAY_P2P_OPT_P2P_ERR_MAC_FORMAT = WIFI_DISPLAY_BASE + 27,
    WIFI_DISPLAY_P2P_OPT_P2P_ERR_CONTEN = WIFI_DISPLAY_BASE + 28,
    WIFI_DISPLAY_P2P_OPT_P2P_ERR_SIZE_NW_NAME = WIFI_DISPLAY_BASE + 29,
    WIFI_DISPLAY_P2P_OPT_MOVING_FREEZE_CTRL = WIFI_DISPLAY_BASE + 30,
    WIFI_DISPLAY_P2P_OPT_NON_SYSTEMAPP = WIFI_DISPLAY_BASE + 31,
    WIFI_DISPLAY_P2P_OPT_ENTERPRISE_DENIED = WIFI_DISPLAY_BASE + 32,
    WIFI_DISPLAY_P2P_DISCONNECTED_STATE_CLOSED = WIFI_DISPLAY_BASE + 33,
    WIFI_DISPLAY_P2P_DISCONNECTED_DEVICE_CHANGE = WIFI_DISPLAY_BASE + 34,
    WIFI_DISPLAY_P2P_DISCONNECTED_CONNECTION_CHANGE = WIFI_DISPLAY_BASE + 35,
};

}  // namespace CastEngineService
}  // namespace CastEngine
}  // namespace OHOS

#endif
