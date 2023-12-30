/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Cast session implement class.
 * Author: zhangge
 * Create: 2022-07-19
 */

#ifndef CAST_SESSION_COMMON_H
#define CAST_SESSION_COMMON_H

#include <array>
#include "cast_engine_common.h"
#include "cast_service_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
struct CastRemoteDeviceInfo {
    CastInnerRemoteDevice remoteDevice;
    DeviceState deviceState;
};

enum class SessionState : uint8_t {
    DEFAULT,
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    PLAYING,
    PAUSED,
    DISCONNECTING,
    STREAM,
    AUTHING,
    SESSION_STATE_MAX,
};

const std::array<std::string, static_cast<size_t>(SessionState::SESSION_STATE_MAX)> SESSION_STATE_STRING = {
    "DEFAULT", "DISCONNECTED", "CONNECTING", "CONNECTED", "PLAYING", "PAUSED", "DISCONNECTING", "STREAM",
    "AUTHING"
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
