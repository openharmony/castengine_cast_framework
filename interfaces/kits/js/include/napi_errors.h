/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply napi errors for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#ifndef NAPI_ERRORS_H
#define NAPI_ERRORS_H

#include <map>
#include "cast_engine_errors.h"

namespace OHOS {
namespace CastEngine {
namespace NapiErrors {
static std::map<int32_t, int32_t> errcode_ = {
    { CAST_ENGINE_ERROR, CAST_ENGINE_ERROR },
    { ERR_NO_MEMORY, ERR_NO_MEMORY },
    { ERR_SESSION_NOT_EXIST, ERR_SESSION_NOT_EXIST },
    { ERR_SERVICE_STATE_NOT_MATCH, ERR_SERVICE_STATE_NOT_MATCH },
    { ERR_SESSION_STATE_NOT_MATCH, ERR_SESSION_STATE_NOT_MATCH },
    { ERR_NO_PERMISSION, ERR_NO_PERMISSION },
    { ERR_INVALID_PARAM, ERR_INVALID_PARAM }
};
}
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_ENGINE_ERRORS_H