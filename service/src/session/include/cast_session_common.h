/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
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
    SESSION_STATE_MAX,
};

const std::array<std::string, static_cast<size_t>(SessionState::SESSION_STATE_MAX)> SESSION_STATE_STRING = {
    "DEFAULT", "DISCONNECTED", "CONNECTING", "CONNECTED", "PLAYING", "PAUSED", "DISCONNECTING", "STREAM",
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
