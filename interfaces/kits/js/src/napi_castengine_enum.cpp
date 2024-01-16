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
 * Description: supply napi nums realization for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#include <string>

#include "napi/native_common.h"
#include "cast_engine_common.h"
#include "napi_castengine_enum.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Napi-Enum");

static napi_status SetNamedProperty(napi_env env, napi_value &obj, const std::string &name, int32_t value)
{
    napi_value property = nullptr;
    napi_status status = napi_create_int32(env, value, &property);
    if (status != napi_ok) {
        CLOGE("napi_create_int32 failed");
        return status;
    }
    status = napi_set_named_property(env, obj, name.c_str(), property);
    if (status != napi_ok) {
        CLOGD("napi_set_named_property failed");
        return status;
    }
    return status;
}

static napi_value ExportPlayerStates(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "PLAYER_STATE_ERROR", static_cast<int32_t>(PlayerStates::PLAYER_STATE_ERROR));
    (void)SetNamedProperty(env, result, "PLAYER_IDLE", static_cast<int32_t>(PlayerStates::PLAYER_IDLE));
    (void)SetNamedProperty(env, result, "PLAYER_INITIALIZED", static_cast<int32_t>(PlayerStates::PLAYER_INITIALIZED));
    (void)SetNamedProperty(env, result, "PLAYER_PREPARING", static_cast<int32_t>(PlayerStates::PLAYER_PREPARING));
    (void)SetNamedProperty(env, result, "PLAYER_PREPARED", static_cast<int32_t>(PlayerStates::PLAYER_PREPARED));
    (void)SetNamedProperty(env, result, "PLAYER_STARTED", static_cast<int32_t>(PlayerStates::PLAYER_STARTED));
    (void)SetNamedProperty(env, result, "PLAYER_PAUSED", static_cast<int32_t>(PlayerStates::PLAYER_PAUSED));
    (void)SetNamedProperty(env, result, "PLAYER_STOPPED", static_cast<int32_t>(PlayerStates::PLAYER_STOPPED));
    (void)SetNamedProperty(env, result, "PLAYER_PLAYBACK_COMPLETE",
        static_cast<int32_t>(PlayerStates::PLAYER_PLAYBACK_COMPLETE));
    (void)SetNamedProperty(env, result, "PLAYER_RELEASED", static_cast<int32_t>(PlayerStates::PLAYER_RELEASED));
    (void)SetNamedProperty(env, result, "PLAYER_BUFFERING", static_cast<int32_t>(PlayerStates::PLAYER_BUFFERING));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportPlaybackSpeed(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "SPEED_FORWARD_0_75_X",
        static_cast<int32_t>(PlaybackSpeed::SPEED_FORWARD_0_75_X));
    (void)SetNamedProperty(env, result, "SPEED_FORWARD_1_00_X",
        static_cast<int32_t>(PlaybackSpeed::SPEED_FORWARD_1_00_X));
    (void)SetNamedProperty(env, result, "SPEED_FORWARD_1_25_X",
        static_cast<int32_t>(PlaybackSpeed::SPEED_FORWARD_1_25_X));
    (void)SetNamedProperty(env, result, "SPEED_FORWARD_1_75_X",
        static_cast<int32_t>(PlaybackSpeed::SPEED_FORWARD_1_75_X));
    (void)SetNamedProperty(env, result, "SPEED_FORWARD_2_00_X",
        static_cast<int32_t>(PlaybackSpeed::SPEED_FORWARD_2_00_X));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportLoopMode(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "LOOP_MODE_SEQUENCE", static_cast<int32_t>(LoopMode::LOOP_MODE_SEQUENCE));
    (void)SetNamedProperty(env, result, "LOOP_MODE_SINGLE", static_cast<int32_t>(LoopMode::LOOP_MODE_SINGLE));
    (void)SetNamedProperty(env, result, "LOOP_MODE_LIST", static_cast<int32_t>(LoopMode::LOOP_MODE_LIST));
    (void)SetNamedProperty(env, result, "LOOP_MODE_SHUFFLE", static_cast<int32_t>(LoopMode::LOOP_MODE_SHUFFLE));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportDeviceType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "DEVICE_OTHERS", static_cast<int32_t>(DeviceType::DEVICE_OTHERS));
    (void)SetNamedProperty(env, result, "DEVICE_SCREEN_PLAYER", static_cast<int32_t>(DeviceType::DEVICE_SCREEN_PLAYER));
    (void)SetNamedProperty(env, result, "DEVICE_HW_TV", static_cast<int32_t>(DeviceType::DEVICE_HW_TV));
    (void)SetNamedProperty(env, result, "DEVICE_SOUND_BOX", static_cast<int32_t>(DeviceType::DEVICE_SOUND_BOX));
    (void)SetNamedProperty(env, result, "DEVICE_HICAR", static_cast<int32_t>(DeviceType::DEVICE_HICAR));
    (void)SetNamedProperty(env, result, "DEVICE_MATEBOOK", static_cast<int32_t>(DeviceType::DEVICE_MATEBOOK));
    (void)SetNamedProperty(env, result, "DEVICE_PAD", static_cast<int32_t>(DeviceType::DEVICE_PAD));
    (void)SetNamedProperty(env, result, "DEVICE_CAST_PLUS", static_cast<int32_t>(DeviceType::DEVICE_CAST_PLUS));
    (void)SetNamedProperty(env, result, "DEVICE_SMART_SCREEN_UNF",
                           static_cast<int32_t>(DeviceType::DEVICE_SMART_SCREEN_UNF));
    (void)SetNamedProperty(env, result, "DEVICE_PAD_IN_CAR",
                           static_cast<int32_t>(DeviceType::DEVICE_PAD_IN_CAR));
    (void)SetNamedProperty(env, result, "DEVICE_SUPER_LAUNCHER",
                           static_cast<int32_t>(DeviceType::DEVICE_SUPER_LAUNCHER));
    (void)SetNamedProperty(env, result, "DEVICE_CAR_MULTI_SCREEN_PLAY",
                           static_cast<int32_t>(DeviceType::DEVICE_CAR_MULTI_SCREEN_PLAY));
    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportSubDeviceType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "SUB_DEVICE_DEFAULT", static_cast<int32_t>(SubDeviceType::SUB_DEVICE_DEFAULT));
    (void)SetNamedProperty(env, result, "SUB_DEVICE_MATEBOOK_PAD",
        static_cast<int32_t>(SubDeviceType::SUB_DEVICE_MATEBOOK_PAD));
    (void)SetNamedProperty(env, result, "SUB_DEVICE_CAST_PLUS_WHITEBOARD",
        static_cast<int32_t>(SubDeviceType::SUB_DEVICE_CAST_PLUS_WHITEBOARD));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportTriggerType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "UNSPEC_TAG", static_cast<int32_t>(TriggerType::UNSPEC_TAG));
    (void)SetNamedProperty(env, result, "PASSIVE_MATCH_TAG", static_cast<int32_t>(TriggerType::PASSIVE_MATCH_TAG));
    (void)SetNamedProperty(env, result, "ACTIVE_MATCH_TAG", static_cast<int32_t>(TriggerType::ACTIVE_MATCH_TAG));
    (void)SetNamedProperty(env, result, "PASSIVE_BIND_TAG", static_cast<int32_t>(TriggerType::PASSIVE_BIND_TAG));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportDeviceState(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "CONNECTING", static_cast<int32_t>(DeviceState::CONNECTING));
    (void)SetNamedProperty(env, result, "CONNECTED", static_cast<int32_t>(DeviceState::CONNECTED));
    (void)SetNamedProperty(env, result, "PAUSED", static_cast<int32_t>(DeviceState::PAUSED));
    (void)SetNamedProperty(env, result, "PLAYING", static_cast<int32_t>(DeviceState::PLAYING));
    (void)SetNamedProperty(env, result, "DISCONNECTING", static_cast<int32_t>(DeviceState::DISCONNECTING));
    (void)SetNamedProperty(env, result, "DISCONNECTED", static_cast<int32_t>(DeviceState::DISCONNECTED));
    (void)SetNamedProperty(env, result, "STREAM", static_cast<int32_t>(DeviceState::STREAM));
    (void)SetNamedProperty(env, result, "AUTHING", static_cast<int32_t>(DeviceState::AUTHING));
    (void)SetNamedProperty(env, result, "DEVICE_STATE_MAX", static_cast<int32_t>(DeviceState::DEVICE_STATE_MAX));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportEventCode(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "UNKNOWN_EVENT", static_cast<int32_t>(EventCode::UNKNOWN_EVENT));
    (void)SetNamedProperty(env, result, "ERR_CONNECTION_FAILED", static_cast<int32_t>(
        EventCode::ERR_CONNECTION_FAILED));
    (void)SetNamedProperty(env, result, "ERR_PIN_CODE_RETRY_COUNT_EXCEEDED", static_cast<int32_t>(
        EventCode::ERR_PIN_CODE_RETRY_COUNT_EXCEEDED));
    (void)SetNamedProperty(env, result, "ERR_CANCEL_BY_SINK", static_cast<int32_t>(EventCode::ERR_CANCEL_BY_SINK));
    (void)SetNamedProperty(env, result, "ERR_DISTRUST_BY_SINK", static_cast<int32_t>(EventCode::ERR_DISTRUST_BY_SINK));
    (void)SetNamedProperty(env, result, "DEFAULT_EVENT", static_cast<int32_t>(EventCode::DEFAULT_EVENT));
    (void)SetNamedProperty(env, result, "EVT_TRUST_BY_SINK", static_cast<int32_t>(EventCode::EVT_TRUST_BY_SINK));
    (void)SetNamedProperty(env, result, "EVT_CANCEL_BY_SOURCE", static_cast<int32_t>(EventCode::EVT_CANCEL_BY_SOURCE));
    (void)SetNamedProperty(env, result, "EVT_AUTHENTICATION_COMPLETED",
        static_cast<int32_t>(EventCode::EVT_AUTHENTICATION_COMPLETED));
    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportServiceStatus(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "DISCONNECTED", static_cast<int32_t>(ServiceStatus::DISCONNECTED));
    (void)SetNamedProperty(env, result, "DISCONNECTING", static_cast<int32_t>(ServiceStatus::DISCONNECTING));
    (void)SetNamedProperty(env, result, "CONNECTING", static_cast<int32_t>(ServiceStatus::CONNECTING));
    (void)SetNamedProperty(env, result, "CONNECTED", static_cast<int32_t>(ServiceStatus::CONNECTED));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportDeviceStatusState(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "DEVICE_AVAILABLE", static_cast<int32_t>(DeviceStatusState::DEVICE_AVAILABLE));
    (void)SetNamedProperty(env, result, "DEVICE_CONNECTED", static_cast<int32_t>(DeviceStatusState::DEVICE_CONNECTED));
    (void)SetNamedProperty(env, result, "DEVICE_DISCONNECTED",
        static_cast<int32_t>(DeviceStatusState::DEVICE_DISCONNECTED));
    (void)SetNamedProperty(env, result, "DEVICE_CONNECT_REQ",
        static_cast<int32_t>(DeviceStatusState::DEVICE_CONNECT_REQ));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportPropertyType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "VIDEO_SIZE", static_cast<int32_t>(PropertyType::VIDEO_SIZE));
    (void)SetNamedProperty(env, result, "VIDEO_FPS", static_cast<int32_t>(PropertyType::VIDEO_FPS));
    (void)SetNamedProperty(env, result, "WINDOW_SIZE", static_cast<int32_t>(PropertyType::WINDOW_SIZE));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportChannelType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "SOFT_BUS", static_cast<int32_t>(ChannelType::SOFT_BUS));
    (void)SetNamedProperty(env, result, "LEGACY_CHANNEL", static_cast<int32_t>(ChannelType::LEGACY_CHANNEL));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportProtocolType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "CAST_PLUS_MIRROR", static_cast<int32_t>(ProtocolType::CAST_PLUS_MIRROR));
    (void)SetNamedProperty(env, result, "CAST_PLUS_STREAM", static_cast<int32_t>(ProtocolType::CAST_PLUS_STREAM));
    (void)SetNamedProperty(env, result, "MIRACAST", static_cast<int32_t>(ProtocolType::MIRACAST));
    (void)SetNamedProperty(env, result, "DLNA", static_cast<int32_t>(ProtocolType::DLNA));
    (void)SetNamedProperty(env, result, "COOPERATION", static_cast<int32_t>(ProtocolType::COOPERATION));
    (void)SetNamedProperty(env, result, "COOPERATION_LEGACY", static_cast<int32_t>(ProtocolType::COOPERATION_LEGACY));
    (void)SetNamedProperty(env, result, "HICAR", static_cast<int32_t>(ProtocolType::HICAR));
    (void)SetNamedProperty(env, result, "SUPER_LAUNCHER", static_cast<int32_t>(ProtocolType::SUPER_LAUNCHER));
    (void)SetNamedProperty(env, result, "CAST_COOPERATION", static_cast<int32_t>(ProtocolType::CAST_COOPERATION));
    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportEndType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "CAST_SINK", static_cast<int32_t>(EndType::CAST_SINK));
    (void)SetNamedProperty(env, result, "CAST_SOURCE", static_cast<int32_t>(EndType::CAST_SOURCE));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportEventId(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "EVENT_BEGIN", static_cast<int32_t>(EventId::EVENT_BEGIN));
    (void)SetNamedProperty(env, result, "EVENT_END", static_cast<int32_t>(EventId::EVENT_END));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportColorStandard(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "BT709", static_cast<int32_t>(ColorStandard::BT709));
    (void)SetNamedProperty(env, result, "BT601_PAL", static_cast<int32_t>(ColorStandard::BT601_PAL));
    (void)SetNamedProperty(env, result, "BT601_NTSC", static_cast<int32_t>(ColorStandard::BT601_NTSC));
    (void)SetNamedProperty(env, result, "BT2020", static_cast<int32_t>(ColorStandard::BT2020));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportVideoCodecType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "H264", static_cast<int32_t>(VideoCodecType::H264));
    (void)SetNamedProperty(env, result, "H265", static_cast<int32_t>(VideoCodecType::H265));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

static napi_value ExportCastModeType(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    (void)SetNamedProperty(env, result, "MIRROR_CAST", static_cast<int32_t>(CastMode::MIRROR_CAST));
    (void)SetNamedProperty(env, result, "APP_CAST", static_cast<int32_t>(CastMode::APP_CAST));

    NAPI_CALL(env, napi_object_freeze(env, result));
    return result;
}

napi_status InitEnums(napi_env env, napi_value exports)
{
    const napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("PlayerStates", ExportPlayerStates(env)),
        DECLARE_NAPI_PROPERTY("PlaybackSpeed", ExportPlaybackSpeed(env)),
        DECLARE_NAPI_PROPERTY("LoopMode", ExportLoopMode(env)),
        DECLARE_NAPI_PROPERTY("DeviceType", ExportDeviceType(env)),
        DECLARE_NAPI_PROPERTY("SubDeviceType", ExportSubDeviceType(env)),
        DECLARE_NAPI_PROPERTY("TriggerType", ExportTriggerType(env)),
        DECLARE_NAPI_PROPERTY("DeviceState", ExportDeviceState(env)),
        DECLARE_NAPI_PROPERTY("EventCode", ExportEventCode(env)),
        DECLARE_NAPI_PROPERTY("ServiceStatus", ExportServiceStatus(env)),
        DECLARE_NAPI_PROPERTY("DeviceStatusState", ExportDeviceStatusState(env)),
        DECLARE_NAPI_PROPERTY("PropertyType", ExportPropertyType(env)),
        DECLARE_NAPI_PROPERTY("ChannelType", ExportChannelType(env)),
        DECLARE_NAPI_PROPERTY("ProtocolType", ExportProtocolType(env)),
        DECLARE_NAPI_PROPERTY("EndType", ExportEndType(env)),
        DECLARE_NAPI_PROPERTY("EventId", ExportEventId(env)),
        DECLARE_NAPI_PROPERTY("ColorStandard", ExportColorStandard(env)),
        DECLARE_NAPI_PROPERTY("VideoCodecType", ExportVideoCodecType(env)),
        DECLARE_NAPI_PROPERTY("CastMode", ExportCastModeType(env))
    };

    size_t count = sizeof(properties) / sizeof(napi_property_descriptor);
    return napi_define_properties(env, exports, count, properties);
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
