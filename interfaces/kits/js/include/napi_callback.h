/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply napi callback for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#ifndef NAPI_CALLBACK_H
#define NAPI_CALLBACK_H

#include <functional>
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "uv.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class NapiCallback final {
public:
    using NapiArgsGetter = std::function<void(napi_env env, int &argc, napi_value *argv)>;

    explicit NapiCallback(napi_env env);
    ~NapiCallback();

    napi_env GetEnv() const;

    void Call(napi_ref method, NapiArgsGetter getter);

private:
    static void AfterWorkCallback(uv_work_t *work, int status);

    struct DataContext {
        napi_env env;
        napi_ref method;
        NapiArgsGetter getter;
    };
    napi_env env_ = nullptr;
    uv_loop_s *loop_ = nullptr;

    static constexpr size_t ARGC_MAX = 6;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif