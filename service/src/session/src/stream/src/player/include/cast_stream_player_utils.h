/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
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