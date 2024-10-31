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
 * Description: supply napi callback for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#ifndef NAPI_CALLBACK_H
#define NAPI_CALLBACK_H

#include <functional>
#include <list>
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "uv.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class NapiCallback final : public std::enable_shared_from_this<NapiCallback> {
public:
    using NapiArgsGetter = std::function<void(napi_env env, int &argc, napi_value *argv)>;

    explicit NapiCallback(napi_env env);
    ~NapiCallback();

    napi_env GetEnv() const;

    void HandleEvent(int32_t event, NapiArgsGetter &getter);
    napi_status AddCallback(napi_env env, int32_t event, napi_value callback);
    napi_status RemoveCallback(napi_env env, int32_t event, napi_value callback);
    bool IsCallbackListEmpty();

private:
    struct DataContext {
        napi_env env;
        napi_ref method;
        NapiArgsGetter getter;
        std::shared_ptr<NapiCallback> callback;
        int32_t event;
    };
    void Call(napi_ref method, NapiArgsGetter &getter, int32_t event);
    void AfterWorkCallback(std::shared_ptr<DataContext> context);
    bool IsCallbackValid(napi_env env, napi_ref ref, int32_t event);
    napi_status ReleaseRef(napi_env env, napi_ref ref);

    static constexpr int EVENT_TYPE_MAX = 30;
    static constexpr size_t ARGC_MAX = 6;

    napi_env env_ = nullptr;
    uv_loop_s *loop_ = nullptr;
    std::mutex lock_;
    std::list<napi_ref> callbacks_[EVENT_TYPE_MAX] {};
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif