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
 * Description: supply cast stream common.
 * Author: zhangjingnan
 * Create: 2023-3-11
 */

#ifndef CAST_STREAM_COMMON_H
#define CAST_STREAM_COMMON_H

namespace OHOS {
namespace CastEngine {
constexpr int32_t CAST_STREAM_FULL_VOLUME = 100;
constexpr int32_t CAST_STREAM_INT_INIT = 0;
constexpr int32_t CAST_STREAM_INT_INVALID = -1;
constexpr int32_t CAST_STREAM_INT_IGNORE = -1;
constexpr int32_t CAST_STREAM_MAX_TIMES = 40;
constexpr int32_t CAST_STREAM_WAIT_TIME = 100;
constexpr double CAST_STREAM_DOUBLE_INVALID = -1;
constexpr int32_t ERR_CODE_PLAY_FAILED = 10003;
constexpr int32_t AUTO_POSITION_UPDATE_INTERVAL = 100;
constexpr int32_t AUTO_POSITION_SYNC_INTERVAL = 10000;
constexpr int32_t POSITION_LAG_MINIMUM = 50;
constexpr int32_t POSITION_LAG_MAXIMUM = 150;
const std::string PLAYER_ERROR = "PLAYER_ERROR";

#define RETURN_IF_PARSE_WRONG(value, data, key, ret, jsonType)            \
    do {                                                                  \
        if (!(data).contains(key) || !(data)[key].is_##jsonType()) {      \
            CLOGE("json object does not contains key:%s", (key).c_str()); \
            return ret;                                                   \
        }                                                                 \
        (value) = (data)[key];                                            \
    } while (0)

#define RET_VOID

#define RETURN_FALSE_IF_PARSE_NUMBER_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, false, number)

#define RETURN_FALSE_IF_PARSE_STRING_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, false, string)

#define RETURN_VOID_IF_PARSE_STRING_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, RET_VOID, string)

#define RETURN_FALSE_IF_PARSE_BOOL_WRONG(value, data, key) RETURN_IF_PARSE_WRONG(value, data, key, false, boolean)
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_COMMON_H