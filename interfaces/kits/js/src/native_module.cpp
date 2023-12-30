/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: register napi module for napi interfaces.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_cast_session_manager.h"
#include "napi_cast_session.h"
#include "napi_castengine_utils.h"
#include "napi_stream_player.h"
#include "napi_mirror_player.h"
#include "napi_castengine_enum.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
EXTERN_C_START

static napi_value Init(napi_env env, napi_value exports)
{
    InitEnums(env, exports);
    NapiCastSessionManager::Init(env, exports);
    NapiCastSession::DefineCastSessionJSClass(env);
    NapiStreamPlayer::DefineStreamPlayerJSClass(env);
    NapiMirrorPlayer::DefineMirrorPlayerJSClass(env);
    return exports;
}
EXTERN_C_END

// module description
static napi_module napiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "cast",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

// module registration
extern "C" __attribute__((constructor)) void RegisterModule()
{
    napi_module_register(&napiModule);
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS