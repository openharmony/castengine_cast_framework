/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
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