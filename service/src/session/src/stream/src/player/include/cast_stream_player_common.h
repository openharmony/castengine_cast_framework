/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply cast stream player common.
 * Author: zhangjingnan
 * Create: 2023-3-11
 */

#ifndef CAST_STREAM_PLAYER_COMMON_H
#define CAST_STREAM_PLAYER_COMMON_H

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
static std::map<Media::PlaybackRateMode, PlaybackSpeed> g_mediaSpeedToPlaybackSpeedMap = {
    { Media::SPEED_FORWARD_0_75_X, PlaybackSpeed::SPEED_FORWARD_0_75_X},
    { Media::SPEED_FORWARD_1_00_X, PlaybackSpeed::SPEED_FORWARD_1_00_X },
    { Media::SPEED_FORWARD_1_25_X, PlaybackSpeed::SPEED_FORWARD_1_25_X },
    { Media::SPEED_FORWARD_1_75_X, PlaybackSpeed::SPEED_FORWARD_1_75_X },
    { Media::SPEED_FORWARD_2_00_X, PlaybackSpeed::SPEED_FORWARD_2_00_X }
};
static std::map<PlaybackSpeed, Media::PlaybackRateMode> g_doubleToModeTypeMap = {
    { PlaybackSpeed::SPEED_FORWARD_0_75_X, Media::SPEED_FORWARD_0_75_X },
    { PlaybackSpeed::SPEED_FORWARD_1_00_X, Media::SPEED_FORWARD_1_00_X },
    { PlaybackSpeed::SPEED_FORWARD_1_25_X, Media::SPEED_FORWARD_1_25_X },
    { PlaybackSpeed::SPEED_FORWARD_1_75_X, Media::SPEED_FORWARD_1_75_X },
    { PlaybackSpeed::SPEED_FORWARD_2_00_X, Media::SPEED_FORWARD_2_00_X }
};
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_COMMON_H