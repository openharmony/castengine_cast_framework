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
 * Description: cast engine dfx.
 * Author: wangxueshuang
 * Create: 2023-06-05
 */

#ifndef CAST_ENGINE_DFX_H
#define CAST_ENGINE_DFX_H

#include <string>
#include <cast_engine_common.h>
#include "hisysevent.h"

#include "json.hpp"

namespace OHOS {
namespace CastEngine {
using nlohmann::json;

class CastEngineDfx {
public:
    static void WriteErrorEvent(int32_t errorCode);
    static void SetStreamInfo(const std::string &streamInfoKey, const std::string &streamInfoValue);
    static void SetLocalDeviceInfo(const std::string &localDeviceInfoKey, const std::string &localDeviceInfoValue);
    static void SetRemoteDeviceInfo(const std::string &remoteDeviceInfoKey, const std::string &remoteDeviceInfoValue);
    static void SetConnectInfo(const std::string &connectInfoKey, const std::string &connectInfoValue);
    static std::string GetStreamInfo();
    static std::string GetLocalDeviceInfo();
    static std::string GetRemoteDeviceInfo();
    static std::string GetConnectInfo();
    static std::string GetSequentialId();
    static std::string GetBizPackageName();

private:
    static json jsonSteamInfo_;
    static json jsonLocalDeviceInfo_;
    static json jsonRemoteDeviceInfo_;
    static json jsonConnectInfo_;
    static const std::string PACKAGE_NAME;
    static const std::string SEQUENTIAL_ID_CHARS;
    static const int SN_LENGTH = 32;
    static const int SEQUENTIAL_ID_CHARS_LENGTH = 62;
};

// discovery fail error code
static const int32_t START_DISCOVERY_FAIL = 100;

// connection fail error code
static const int32_t AUTHENTICATE_DEVICE_FAIL = 200;
static const int32_t SOURCE_CREATE_SESSION_SERVER_FAIL = 201;
static const int32_t SINK_CREATE_SESSION_SERVER_FAIL = 202;
static const int32_t OPEN_SESSION_FAIL = 203;
static const int32_t SEND_CONSULTION_DATA_FAIL = 204;

// stream fail error code
static const int32_t PLAYER_INIT_FAIL = 300;

static constexpr char CAST_ENGINE_DFX_DOMAIN_NAME[] = "CAST_ENGINE";
static constexpr char ORG_PKG[] = "cast_engine_service";
static constexpr char EVENT_NAME[] = "CAST_PLUS_BEHAVIOR";
static constexpr char DEVICE_MANAGER_NAME[] = "deviceManager";
static constexpr char DSOFTBUS_NAME[] = "dsoftbus";
static constexpr char COLLABORATIONFWK_NAME[] = "CollaborationFwk";
static constexpr char HUAWEICAST_NAME[] = "com.huawei.hmos.huaweicast";

enum class BIZSceneStage : int32_t {
    // scene1
    START_DISCOVERY = 1,
    DEVICE_DISCOVERY = 2,

    DEVICE_AUTHENTICATION = 1,
    ESTABLISH_CONSULT_SESSION = 2,
    ESTABLISH_RTSP_CHANNEL = 3,

    // scene2
    ESTABLISH_AUDIO_CHANNEL = 4,
    ESTABLISH_VIDEO_CHANNEL = 5,
    ESTABLISH_REVERSE_CHNANEL = 6,
    CAST_SUCCESS = 7,

    // scene3
    LOCAL_ESTABLISH_STREAM_CHANNEL = 4,
    LOCAL_STREAM_PLAY_RESOURCE_SUCCESS = 5,

    // scene4
    ONLINE_STREAM_PLAY_RESOURCE_SUCCESS = 5,

    // scene5
    SINK_CONTROL = 1,
    SOURCE_CONTROL = 2,
    SINK_RESPONSE = 3,
    SOURCE_RESPONSE = 4,

    // scene6
    COOPERATION_ESTABLISH_RTSP_CHANNEL = 1,
    COOPERATION_ESTABLISH_VIDEO_CHANNEL = 2,
    COOPERATION_ESTABLISH_REVERSE_CHNANEL = 3,
    COOPERATION_CAST_SUCCESS = 4,

    // scene7
    SINK_SEND_REVERSE_EVENT = 1,
    SOURCE_RECEIVE_REVERSE_EVENT = 2,
    SOURCE_OUTPUT_VIDEO_FRAME = 3,
    SINK_RECEIVE_VIDEO_FRAME = 4,
    SINK_BEGIN_DECODE_VIDEO_FRAME = 5,
    SINK_DISPLAY_VIDEO_FRAME = 6,

    // scene8
    START_DISCONNECT = 1,
    DISCONNECT_END = 2,

    DLNA_DEVICE_DISCOVERY = 501,
    DLNA_DEVICE_CONNECT = 502,
    DLNA_DEVICE_CAST = 503,
    DLNA_ACQUIRE_RESOURCE_FINISH = 504,
    DLNA_PLAY_RESOURCE_FINISH = 505,
    MIRACAST_DEVICE_DISCOVERY = 601,
    MIRACAST_DEVICE_CONNECT = 602,
    MIRACAST_DATA_CONSULT = 603,
    MIRACAST_AUDIO_VIDEO_CHANNEL_FINISH = 604,
    MIRACAST_REVERSE_CHANNEL_FINISH = 605,
    MIRACAST_AUDIO_VIDEO_PLAY_SUCCESS = 606
};

enum class BIZStateType : int32_t {
    BIZ_STATE_BEGIN = 1,
    BIZ_STATE_END
};

enum class StageResType : int32_t {
    STAGE_RES_IDLE = 0,
    STAGE_RES_SUCCESS,
    STAGE_RES_FAILED,
    STAGE_RES_CANCEL,
    STAGE_RES_UNKNOWN
};

enum class BIZSceneType : int32_t {
    DEVICE_DISCOVERY = 1,
    CAST_MIRROR,
    CAST_STREAM,
    ONLINE_CAST_STREAM,
    DUAL_POINTS_CONTROL,
    COOPERATION,
    REVERSE_CONTROL,
    DEVICE_DISCONNECTED,
    DLNA,
    MIRACAST
};

static const uint8_t CAST_ENGINE_SYSTEM_ID = 208;
static const uint8_t CAST_ENGINE_CAST_PLUS_MODULE_ID = 10;
inline uint32_t GetErrorCode(uint8_t systemID, uint8_t moduleID, uint16_t code)
{
    const int moduleIndex = 16;
    const int systemIndex = 21;

    uint32_t errorCode = systemID << systemIndex | moduleID << moduleIndex | code;
    return errorCode;
}

inline std::string GetAnonymousDeviceID(const std::string& deviceId)
{
    const std::size_t minimumLen = 10;
    const std::size_t subIndex = 5;

    if (deviceId.empty() || deviceId.length() < minimumLen) {
        return "unknown";
    }
    return deviceId.substr(0, subIndex) + "**" + deviceId.substr(deviceId.length() - subIndex);
}

int GetBIZSceneType(int protocols);

static int32_t CAST_RADAR_SUCCESS = 0;
using RadarParamInt = std::map<std::string, int32_t>;
using RadarParamString = std::map<std::string, std::string>;
void HiSysEventWriteWrap(const std::string& funcName, const RadarParamInt& paramInt,
    const RadarParamString& paramStr);
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_ENGINE_DFX_H
