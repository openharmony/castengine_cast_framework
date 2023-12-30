/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply cast stream player utils.
 * Author: zhangjingnan
 * Create: 2023-8-11
 */

#ifndef CAST_STREAM_PLAYER_UTILS_H
#define CAST_STREAM_PLAYER_UTILS_H

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastStreamPlayerUtils {
public:
    static int GetVolume();
    static int GetMaxVolume();
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_UTILS_H