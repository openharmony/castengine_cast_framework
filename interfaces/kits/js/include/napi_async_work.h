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
 * Description: supply napi async work for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#ifndef NAPI_CAST_ASYNC_WORK_H
#define NAPI_CAST_ASYNC_WORK_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
using NapiCbInfoParser = std::function<void(size_t argc, napi_value *argv)>;
using NapiAsyncExecute = std::function<void(void)>;
using NapiAsyncComplete = std::function<void(napi_value &)>;

struct NapiAsyncTask {
    virtual ~NapiAsyncTask();
    void GetJSInfo(napi_env envi, napi_callback_info info, NapiCbInfoParser parser = NapiCbInfoParser(),
        bool sync = false);

    napi_env env = nullptr;
    napi_value output = nullptr;
    napi_status status = napi_invalid_arg;
    std::string errMessage;
    int32_t errCode;
    void *native = nullptr;
    std::string taskName;

private:
    napi_deferred deferred = nullptr;
    napi_async_work work = nullptr;
    napi_ref callbackRef = nullptr;
    NapiAsyncExecute execute = nullptr;
    NapiAsyncComplete complete = nullptr;
    std::shared_ptr<NapiAsyncTask> hold; /* cross thread data */

    static constexpr size_t ARGC_MAX = 4;
    friend class NapiAsyncWork;
};

class NapiAsyncWork {
public:
    static napi_value Enqueue(napi_env env, std::shared_ptr<NapiAsyncTask> ctxt, const std::string &name,
        NapiAsyncExecute execute = NapiAsyncExecute(), NapiAsyncComplete complete = NapiAsyncComplete());

private:
    static void GenerateOutput(NapiAsyncTask *napiAsyncTask);
    enum {
        RESULT_ERROR = 0,
        RESULT_DATA = 1,
        RESULT_ALL = 2
    };
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif // NAPI_CAST_ASYNC_WORK_H