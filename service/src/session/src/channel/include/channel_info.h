/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: link type of channel and the calling module type
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CHANNEL_INFO_H
#define CHANNEL_INFO_H

#include <array>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
enum class ChannelLinkType {
    SOFT_BUS,
    TCP,
    VTP
};

enum class ModuleType {
    AUTH,
    RTSP,
    RTCP,
    VIDEO,
    AUDIO,
    REMOTE_CONTROL,
    STREAM,
    UI_FILES,
    UI_BYTES,
    MODULE_TYPE_MAX
};

const std::array<std::string, static_cast<size_t>(ModuleType::MODULE_TYPE_MAX)> MODULE_TYPE_STRING = {
    "AUTH", "RTSP", "RTCP", "VIDEO", "AUDIO", "REMOTE_CONTROL", "STREAM", "UI_FILES", "UI_BYTES"
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CHANNEL_INFO_H