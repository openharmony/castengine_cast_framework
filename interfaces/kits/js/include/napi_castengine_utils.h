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
 * Description: supply utils for napi interface.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#ifndef NAPI_CASTENGINE_UTILS_H
#define NAPI_CASTENGINE_UTILS_H

#include <list>
#include <uv.h>
#include <variant>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "i_cast_session.h"
#include "oh_remote_control_event.h"

#define CHECK_ARGS_RETURN_VOID(context, condition, message, code) \
    do {                                                          \
        if (!(condition)) {                                       \
            (context)->status = napi_invalid_arg;                 \
            (context)->errMessage = std::string(message);         \
            (context)->errCode = code;                            \
            CLOGE("(" #condition ") failed: " message);           \
            return;                                               \
        }                                                         \
    } while (0)

#define CHECK_STATUS_RETURN_VOID(context, message, code)            \
    do {                                                            \
        if ((context)->status != napi_ok) {                         \
            (context)->errMessage = std::string(message);           \
            (context)->errCode = code;                              \
            CLOGE("(context->status == napi_ok) failed: " message); \
            return;                                                 \
        }                                                           \
    } while (0)

#define CHECK_RETURN_VOID(condition, message)           \
    do {                                                \
        if (!(condition)) {                             \
            CLOGE("(" #condition ") failed: " message); \
            return;                                     \
        }                                               \
    } while (0)

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
constexpr int32_t CALLBACK_ARGC_ZERO = 0;
constexpr int32_t CALLBACK_ARGC_ONE = 1;
constexpr int32_t CALLBACK_ARGC_TWO = 2;
constexpr int32_t CALLBACK_ARGC_THERE = 3;
constexpr int32_t CALLBACK_ARGC_FOUR = 4;

std::string ParseString(napi_env env, napi_value args);
int32_t ParseInt32(napi_env env, napi_value args);
uint32_t ParseUint32(napi_env env, napi_value args);
bool ParseBool(napi_env env, napi_value args);
std::string JsObjectToString(napi_env env, napi_value &object, const char *fieldStr);
int32_t JsObjectToInt32(napi_env env, napi_value &object, const char *fieldStr);
bool JsObjectToBool(napi_env env, napi_value &object, const char *fieldStr);
uint32_t JsObjectToUint32(napi_env env, napi_value &object, const char *fieldStr);
double JsObjectToDouble(napi_env env, napi_value &object, const char *fieldStr);
int64_t JsObjectToInt64(napi_env env, napi_value &object, const char *fieldStr);

napi_value ConvertDeviceListToJS(napi_env env, const std::vector<CastRemoteDevice> &devices);
napi_value ConvertCastSessionToJS(napi_env env, const std::shared_ptr<ICastSession> &castSession);
napi_value ConvertDeviceStateInfoToJS(napi_env env, const DeviceStateInfo &stateEvent);
napi_value ConvertMediaInfoToJS(napi_env env, const MediaInfo &mediaInfo);
napi_value ConvertMediaInfoHolderToJS(napi_env env, const MediaInfoHolder &mediaInfoHolder);
napi_value ConvertCastRemoteDeviceToJS(napi_env env, const CastRemoteDevice &castRemoteDevice);

CastRemoteDevice GetCastRemoteDeviceFromJS(napi_env env, napi_value &object);
CastSessionProperty GetCastSessionPropertyFromJS(napi_env env, napi_value &object);
AudioProperty GetAudioPropertyFromJS(napi_env env, napi_value &object);
VideoProperty GetVideoPropertyFromJS(napi_env env, napi_value &object);
WindowProperty GetWindowPropertyFromJS(napi_env env, napi_value &object);
bool GetMediaInfoFromJS(napi_env env, napi_value &object, MediaInfo &mediaInfo);
bool GetProtocolTypesFromJS(napi_env env, napi_value &object, int &protocolTypes);
bool GetMediaInfoHolderFromJS(napi_env env, napi_value &object, MediaInfoHolder &mediaInfoHolder);

bool GetJSFuncParams(napi_env env, napi_callback_info info, napi_value argv[], size_t expectedArgc,
    napi_valuetype expectedTypes[]);
bool CheckJSParamsType(napi_env env, napi_value argv[], size_t expectedArgc, napi_valuetype expectedTypes[]);
void CallJSFunc(napi_env env, napi_ref func, size_t argc, napi_value argv[]);
napi_value GetUndefinedValue(napi_env env);
bool Equals(napi_env env, napi_value value, napi_ref copy);
napi_status GetRefByCallback(napi_env env, std::list<napi_ref> callbackList, napi_value callback,
    napi_ref &callbackRef);
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif
