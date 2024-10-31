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
#include "napi_castengine_utils.h"

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

void NapiCallback::AfterWorkCallback(std::shared_ptr<DataContext> context)
{
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
    if (!IsCallbackValid(context->env, context->method, context->event)) {
        CLOGE("callback is invalid event:%{public}d", context->event);
        napi_close_handle_scope(context->env, scope);
        return;
    }
    napi_value callback = nullptr;
    if (napi_get_reference_value(context->env, context->method, &callback) != napi_ok) {
        CLOGE("napi_get_reference_value failed");
        ReleaseRef(context->env, context->method);
        napi_close_handle_scope(context->env, scope);
        return;
    }
    napi_value callResult = nullptr;
    if (napi_call_function(context->env, undefined, callback, argc, argv, &callResult) != napi_ok) {
        CLOGE("napi_call_function failed");
    }
    ReleaseRef(context->env, context->method);
    napi_close_handle_scope(context->env, scope);
}

void NapiCallback::Call(napi_ref method, NapiArgsGetter &getter, int32_t event)
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
    auto callback = shared_from_this();
    work->data = new (std::nothrow) DataContext { env_, method, std::move(getter), callback, event };
    if (work->data == nullptr) {
        CLOGE("no memory for DataContext");
        delete work;
        return;
    }
    int res = uv_queue_work(loop_, work, [](uv_work_t *work) {}, [](uv_work_t *work, int status) {
        std::shared_ptr<DataContext> context(static_cast<DataContext *>(work->data), [work](DataContext *ptr) {
            delete ptr;
            delete work;
        });
        if (!context->callback) {
            CLOGE("callback is nullptr");
            return;
        }
        context->callback->AfterWorkCallback(context);
    });
    if (res != 0) {
        CLOGE("uv queue work failed");
        delete static_cast<DataContext *>(work->data);
        delete work;
        return;
    }
}

napi_status NapiCallback::AddCallback(napi_env env, int32_t event, napi_value callback)
{
    if ((event >= EVENT_TYPE_MAX) || (event < 0)) {
        CLOGE("event %{public}d is invalid", event);
        return napi_generic_failure;
    }
    CLOGI("Add callback %{public}d", event);
    std::lock_guard<std::mutex> lockGuard(lock_);
    constexpr int initialRefCount = 1;
    napi_ref ref = nullptr;
    if (GetRefByCallback(env, callbacks_[event], callback, ref) != napi_ok) {
        CLOGE("get callback reference failed");
        return napi_generic_failure;
    }
    if (ref != nullptr) {
        CLOGD("callback has been registered");
        return napi_ok;
    }
    napi_status status = napi_create_reference(env, callback, initialRefCount, &ref);
    if (status != napi_ok) {
        CLOGE("napi_create_reference failed");
        return status;
    }
    callbacks_[event].push_back(ref);
    return napi_ok;
}

napi_status NapiCallback::RemoveCallback(napi_env env, int32_t event, napi_value callback)
{
    if ((event >= EVENT_TYPE_MAX) || (event < 0)) {
        CLOGE("event %{public}d is invalid", event);
        return napi_generic_failure;
    }
    std::lock_guard<std::mutex> lockGuard(lock_);
    if (callback == nullptr) {
        for (auto &callbackRef : callbacks_[event]) {
            ReleaseRef(env, callbackRef);
        }
        callbacks_[event].clear();
        return napi_ok;
    }
    napi_ref ref = nullptr;
    if (GetRefByCallback(env, callbacks_[event], callback, ref) != napi_ok) {
        CLOGE("get callback reference failed");
        return napi_generic_failure;
    }
    if (ref == nullptr) {
        CLOGD("callback has been remove");
        return napi_ok;
    }
    callbacks_[event].remove(ref);

    return ReleaseRef(env, ref);
}

void NapiCallback::HandleEvent(int32_t event, NapiArgsGetter &getter)
{
    if ((event >= EVENT_TYPE_MAX) || (event < 0)) {
        CLOGE("event %{public}d is invalid", event);
        return;
    }
    std::lock_guard<std::mutex> lockGuard(lock_);
    if (callbacks_[event].empty()) {
        CLOGE("not register callback event=%{public}d", event);
        return;
    }
    for (auto ref = callbacks_[event].begin(); ref != callbacks_[event].end(); ++ref) {
        Call(*ref, getter, event);
    }
}

bool NapiCallback::IsCallbackValid(napi_env env, napi_ref ref, int32_t event)
{
    if ((event >= EVENT_TYPE_MAX) || (event < 0)) {
        CLOGE("event %{public}d is invalid", event);
        return false;
    }
    std::lock_guard<std::mutex> lockGuard(lock_);
    for (auto &callbackRef : callbacks_[event]) {
        if (callbackRef != ref) {
            continue;
        }
        uint32_t refCount = 0xff;
        napi_status ret = napi_reference_ref(env, ref, &refCount);
        if (ret != napi_ok || refCount <= 1) {
            CLOGE("callback is invalid ret: %{public}d refCount: %{public}d", ret, refCount);
            return false;
        }
        return true;
    }
    CLOGE("callback has been remove");
    return false;
}

bool NapiCallback::IsCallbackListEmpty()
{
    std::lock_guard<std::mutex> lockGuard(lock_);
    for (auto &callback : callbacks_) {
        if (!callback.empty()) {
            return false;
        }
    }
    return true;
}

napi_status NapiCallback::ReleaseRef(napi_env env, napi_ref ref)
{
    uint32_t refCount = 0xff;
    napi_status ret = napi_ok;
    napi_reference_unref(env, ref, &refCount);
    if (refCount == 0) {
        CLOGI("refCount is 0, delete callback reference");
        ret = napi_delete_reference(env, ref);
    }
    if (ret != napi_ok) {
        CLOGE("delete callback reference failed");
    }
    return ret;
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS