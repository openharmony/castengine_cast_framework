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
 * Description: supply napi callback realization for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#include <memory>
#include "cast_engine_log.h"
#include "napi_callback.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Napi-Callback");

NapiCallback::NapiCallback(napi_env env) : env_(env)
{
    if (env != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_get_uv_event_loop(env, &loop_));
    }
}

NapiCallback::~NapiCallback()
{
    CLOGD("no memory leak for queue-callback");
    env_ = nullptr;
}

napi_env NapiCallback::GetEnv() const
{
    return env_;
}

void NapiCallback::AfterWorkCallback(uv_work_t *work, int status)
{
    std::shared_ptr<DataContext> context(static_cast<DataContext *>(work->data), [work](DataContext *ptr) {
        delete ptr;
        delete work;
    });

    int argc = 0;
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(context->env, &scope);
    napi_value argv[ARGC_MAX] = { nullptr };
    if (context->getter) {
        argc = ARGC_MAX;
        context->getter(context->env, argc, argv);
    }

    napi_value undefined = nullptr;
    if (napi_get_undefined(context->env, &undefined) != napi_ok) {
        CLOGE("napi_get_undefined failed");
        napi_close_handle_scope(context->env, scope);
        return;
    }
    napi_value callback = nullptr;
    if (napi_get_reference_value(context->env, context->method, &callback) != napi_ok) {
        CLOGE("napi_get_reference_value failed");
        napi_close_handle_scope(context->env, scope);
        return;
    }
    napi_value callResult = nullptr;
    if (napi_call_function(context->env, undefined, callback, argc, argv, &callResult) != napi_ok) {
        CLOGE("napi_call_function failed");
    }
    napi_close_handle_scope(context->env, scope);
}

void NapiCallback::Call(napi_ref method, NapiArgsGetter getter)
{
    CLOGD("Start to have JS call");
    if (loop_ == nullptr) {
        CLOGE("loop_ is nullptr");
        return;
    }
    if (method == nullptr) {
        CLOGE("method is nullptr");
        return;
    }

    auto *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        CLOGE("no memory for uv_work_t");
        return;
    }

    work->data = new DataContext { env_, method, std::move(getter) };
    int res = uv_queue_work(
        loop_, work, [](uv_work_t *work) {}, AfterWorkCallback);
    if (res != 0) {
        CLOGE("uv queue work failed");
        delete work;
        return;
    }
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS