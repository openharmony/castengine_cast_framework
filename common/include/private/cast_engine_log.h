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
 * Description: Log format definitions.
 * Author: zhangge
 * Create: 2022-06-15
 */

#ifndef CAST_ENGINE_LOG_H
#define CAST_ENGINE_LOG_H

#include "hilog/log_cpp.h"

using OHOS::HiviewDFX::HiLog;
using OHOS::HiviewDFX::HiLogLabel;

namespace OHOS {
namespace CastEngine {
inline constexpr unsigned int CASTPLUS_LOG_BEGIN = 0xD004600;
inline constexpr unsigned int CAST_ENGINE_LOG_ID = CASTPLUS_LOG_BEGIN + 0x01;
inline constexpr unsigned int CASTPLUS_LOG_END = CASTPLUS_LOG_BEGIN + 0x10;

inline constexpr bool DEBUG = true;

#define DEFINE_CAST_ENGINE_LABEL(name) \
    static constexpr HiLogLabel CAST_ENGINE_LABEL = { LOG_CORE, OHOS::CastEngine::CAST_ENGINE_LOG_ID, name }

#define CLOGV(format, ...)                                                                                \
    do {                                                                                                  \
        if (DEBUG) {                                                                                      \
            (void)HiLog::Error(CAST_ENGINE_LABEL, "[%{public}s:%{public}d]: " format, __func__, __LINE__, \
                ##__VA_ARGS__);                                                                           \
        }                                                                                                 \
    } while (0)
#define CLOGD(format, ...) \
    (void)HiLog::Error(CAST_ENGINE_LABEL, "[%{public}s:%{public}d]: " format, __func__, __LINE__, ##__VA_ARGS__)
#define CLOGI(format, ...) \
    (void)HiLog::Error(CAST_ENGINE_LABEL, "[%{public}s:%{public}d]: " format, __func__, __LINE__, ##__VA_ARGS__)
#define CLOGW(format, ...) \
    (void)HiLog::Error(CAST_ENGINE_LABEL, "[%{public}s:%{public}d]: " format, __func__, __LINE__, ##__VA_ARGS__)
#define CLOGE(format, ...) \
    (void)HiLog::Error(CAST_ENGINE_LABEL, "[%{public}s:%{public}d]: " format, __func__, __LINE__, ##__VA_ARGS__)

#define CHECK_AND_RETURN_RET_LOG(cond, ret, fmt, ...)  \
    do {                                               \
        if (cond) {                                    \
            CLOGE(fmt, ##__VA_ARGS__);                 \
            return ret;                                \
        }                                              \
    } while (0)
} // namespace CastEngine
} // namespace OHOS

#endif