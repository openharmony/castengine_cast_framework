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