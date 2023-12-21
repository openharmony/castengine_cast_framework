/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Cast engine service related common data stucture definitions.
 * Author: zhangge
 * Create: 2022-10-26
 */

#ifndef CAST_SERVICE_COMMON_H
#define CAST_SERVICE_COMMON_H

#include <string>

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
struct CastInnerRemoteDevice {
    std::string deviceId;
    std::string deviceName;
    DeviceType deviceType{ DeviceType::DEVICE_OTHERS };
    SubDeviceType subDeviceType{ SubDeviceType::SUB_DEVICE_DEFAULT };
    std::string ipAddress;
    TriggerType triggerType{ TriggerType::UNSPEC_TAG };
    std::string authData;
    int sessionId{ INVALID_ID };
    ChannelType channelType{ ChannelType::SOFT_BUS };
    const uint8_t *sessionKey{ nullptr };
    uint32_t sessionKeyLength{ 0 };
};

inline constexpr char PKG_NAME[] = "CastEngineService";
} // namespace CastEngine
} // namespace OHOS

#endif
