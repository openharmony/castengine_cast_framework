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
