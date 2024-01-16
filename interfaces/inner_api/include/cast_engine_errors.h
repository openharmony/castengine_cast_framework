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
 * Description: supply errors definition for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#ifndef CAST_ENGINE_ERRORS_H
#define CAST_ENGINE_ERRORS_H

#include <cinttypes>
#include "errors.h"
#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
constexpr int32_t EXPORT CAST_ENGINE_ERROR = -1;
constexpr int32_t EXPORT CAST_ENGINE_SUCCESS = 0;
constexpr int32_t EXPORT CAST_ENGINE_ERROR_BASE = 1000;
constexpr int32_t EXPORT ERR_NO_MEMORY = -(CAST_ENGINE_ERROR_BASE + 1);
constexpr int32_t EXPORT ERR_INVALID_PARAM = -(CAST_ENGINE_ERROR_BASE + 2);
constexpr int32_t EXPORT ERR_NO_PERMISSION = -(CAST_ENGINE_ERROR_BASE + 3);
constexpr int32_t EXPORT ERR_SESSION_NOT_EXIST = -(CAST_ENGINE_ERROR_BASE + 4);
constexpr int32_t EXPORT ERR_SERVICE_STATE_NOT_MATCH = -(CAST_ENGINE_ERROR_BASE + 5);
constexpr int32_t EXPORT ERR_SESSION_STATE_NOT_MATCH = -(CAST_ENGINE_ERROR_BASE + 6);
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_ENGINE_ERRORS_H