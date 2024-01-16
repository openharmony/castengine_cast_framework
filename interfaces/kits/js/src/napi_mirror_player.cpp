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
 * Description: supply napi interface realization for cast mirror player.
 * Author: zhangjingnan
 * Create: 2023-5-27
 */

#include <memory>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "oh_remote_control_event.h"
#include "i_mirror_player.h"
#include "napi_castengine_utils.h"
#include "napi_mirror_player.h"
#include "napi_async_work.h"

using namespace OHOS::CastEngine::CastEngineClient;
using namespace std;

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Napi-MirrorPlayer");

thread_local napi_ref NapiMirrorPlayer::consRef_ = nullptr;

void NapiMirrorPlayer::DefineMirrorPlayerJSClass(napi_env env)
{
    napi_property_descriptor NapiMirrorPlayerDesc[] = {
        DECLARE_NAPI_FUNCTION("play", Play),
        DECLARE_NAPI_FUNCTION("pause", Pause),
        DECLARE_NAPI_FUNCTION("setAppInfo", SetAppInfo),
        DECLARE_NAPI_FUNCTION("setSurface", SetSurface),
        DECLARE_NAPI_FUNCTION("release", Release),
        DECLARE_NAPI_FUNCTION("resizeVirtualScreen", ResizeVirtualScreen)
    };

    napi_value mirrorPlayer = nullptr;
    constexpr int initialRefCount = 1;
    napi_status status = napi_define_class(env, "mirrorPlayer", NAPI_AUTO_LENGTH, NapiMirrorPlayerConstructor, nullptr,
        sizeof(NapiMirrorPlayerDesc) / sizeof(NapiMirrorPlayerDesc[0]), NapiMirrorPlayerDesc, &mirrorPlayer);
    if (status != napi_ok) {
        CLOGE("napi_define_class failed");
        return;
    }
    status = napi_create_reference(env, mirrorPlayer, initialRefCount, &consRef_);
    if (status != napi_ok) {
        CLOGE("DefineMirrorPlayerJSClass napi_create_reference failed");
    }
}

napi_value NapiMirrorPlayer::NapiMirrorPlayerConstructor(napi_env env, napi_callback_info info)
{
    CLOGD("NapiMirrorPlayer start to construct in");
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    CLOGD("NapiMirrorPlayer construct successfully");
    return thisVar;
}

napi_status NapiMirrorPlayer::CreateNapiMirrorPlayer(napi_env env, shared_ptr<IMirrorPlayer> player, napi_value &out)
{
    CLOGD("Start to create napiMirrorPlayer in");
    napi_value result = nullptr;
    napi_value constructor = nullptr;
    if (consRef_ == nullptr || player == nullptr) {
        CLOGE("CreateNapiMirrorPlayer input is null");
        return napi_generic_failure;
    }
    napi_status status = napi_get_reference_value(env, consRef_, &constructor);
    if (status != napi_ok || constructor == nullptr) {
        CLOGE("CreateNapiMirrorPlayer napi_get_reference_value failed");
        return napi_generic_failure;
    }

    constexpr size_t argc = 0;
    status = napi_new_instance(env, constructor, argc, nullptr, &result);
    if (status != napi_ok) {
        CLOGE("CreateNapiMirrorPlayer napi_new_instance failed");
        return napi_generic_failure;
    }

    NapiMirrorPlayer *napiMirrorPlayer = new NapiMirrorPlayer(player);
    if (napiMirrorPlayer == nullptr) {
        CLOGE("NapiMirrorPlayer is nullptr");
        return napi_generic_failure;
    }
    auto finalize = [](napi_env env, void *data, void *hint) {
        NapiMirrorPlayer *player = reinterpret_cast<NapiMirrorPlayer *>(data);
        if (player != nullptr) {
            CLOGI("Session deconstructed");
            delete player;
            player = nullptr;
        }
    };
    if (napi_wrap(env, result, napiMirrorPlayer, finalize, nullptr, nullptr) != napi_ok) {
        CLOGE("CreateNapiMirrorPlayer napi_wrap failed");
        delete napiMirrorPlayer;
        napiMirrorPlayer = nullptr;
        return napi_generic_failure;
    }
    out = result;
    CLOGD("Create napiMirrorPlayer successfully");
    return napi_ok;
}

napi_value NapiMirrorPlayer::Play(napi_env env, napi_callback_info info)
{
    CLOGD("Start to play in");
    struct ConcreteTask : public NapiAsyncTask {
        string deviceId_;
    };
    auto napiAsyntask = std::make_shared<ConcreteTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }

    auto inputParser = [env, napiAsyntask](size_t argc, napi_value *argv) {
        constexpr size_t expectedArgc = 1;
        CHECK_ARGS_RETURN_VOID(napiAsyntask, argc == expectedArgc, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napi_valuetype expectedTypes[expectedArgc] = { napi_string };
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->deviceId_ = ParseString(env, argv[0]);
    };
    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        auto *napiPlayer = reinterpret_cast<NapiMirrorPlayer *>(napiAsyntask->native);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, napiPlayer != nullptr, "napiPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        shared_ptr<IMirrorPlayer> mirrorPlayer = napiPlayer->GetMirrorPlayer();
        CHECK_ARGS_RETURN_VOID(napiAsyntask, mirrorPlayer, "IMirrorPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        int32_t ret = mirrorPlayer->Play(napiAsyntask->deviceId_);
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "Play failed : no permission";
            } else {
                napiAsyntask->errMessage = "Play failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "Play", executor, complete);
}

napi_value NapiMirrorPlayer::Pause(napi_env env, napi_callback_info info)
{
    CLOGD("Start to pause in");
    struct ConcreteTask : public NapiAsyncTask {
        string deviceId_;
    };
    auto napiAsyntask = std::make_shared<ConcreteTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }

    auto inputParser = [env, napiAsyntask](size_t argc, napi_value *argv) {
        constexpr size_t expectedArgc = 1;
        CHECK_ARGS_RETURN_VOID(napiAsyntask, argc == expectedArgc, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napi_valuetype expectedTypes[expectedArgc] = { napi_string };
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->deviceId_ = ParseString(env, argv[0]);
    };
    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        auto *napiPlayer = reinterpret_cast<NapiMirrorPlayer *>(napiAsyntask->native);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, napiPlayer != nullptr, "napiPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        shared_ptr<IMirrorPlayer> mirrorPlayer = napiPlayer->GetMirrorPlayer();
        CHECK_ARGS_RETURN_VOID(napiAsyntask, mirrorPlayer, "IMirrorPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        int32_t ret = mirrorPlayer->Pause(napiAsyntask->deviceId_);
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "Pause failed : no permission";
            } else {
                napiAsyntask->errMessage = "Pause failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "Pause", executor, complete);
}

napi_value NapiMirrorPlayer::SetSurface(napi_env env, napi_callback_info info)
{
    CLOGD("Start to set surface in");
    struct ConcreteTask : public NapiAsyncTask {
        string surfaceId_;
    };
    auto napiAsyntask = std::make_shared<ConcreteTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }

    auto inputParser = [env, napiAsyntask](size_t argc, napi_value *argv) {
        constexpr size_t expectedArgc = 1;
        CHECK_ARGS_RETURN_VOID(napiAsyntask, argc == expectedArgc, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napi_valuetype expectedTypes[expectedArgc] = { napi_string };
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->surfaceId_ = ParseString(env, argv[0]);
    };
    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        auto *napiPlayer = reinterpret_cast<NapiMirrorPlayer *>(napiAsyntask->native);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, napiPlayer != nullptr, "napiPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        shared_ptr<IMirrorPlayer> mirrorPlayer = napiPlayer->GetMirrorPlayer();
        CHECK_ARGS_RETURN_VOID(napiAsyntask, mirrorPlayer, "IMirrorPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        int32_t ret = mirrorPlayer->SetSurface(napiAsyntask->surfaceId_);
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "SetSurface failed : no permission";
            } else {
                napiAsyntask->errMessage = "SetSurface failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "SetSurface", executor, complete);
}

napi_value NapiMirrorPlayer::SetAppInfo(napi_env env, napi_callback_info info)
{
    CLOGD("Start to set appInfo in");
    struct ConcreteTask : public NapiAsyncTask {
        int32_t appUid;
        uint32_t appTokenId;
        int32_t appPid;
    };
    auto napiAsyntask = std::make_shared<ConcreteTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }

    auto inputParser = [env, napiAsyntask](size_t argc, napi_value *argv) {
        constexpr size_t expectedArgc = 3;
        napi_valuetype expectedTypes[expectedArgc] = { napi_number,  napi_number, napi_number};
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->appUid = ParseInt32(env, argv[0]);
        napiAsyntask->appTokenId = static_cast<uint32_t>(ParseInt32(env, argv[1]));
        napiAsyntask->appPid = ParseInt32(env, argv[2]);
    };

    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        auto *napiPlayer = reinterpret_cast<NapiMirrorPlayer *>(napiAsyntask->native);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, napiPlayer != nullptr, "napiPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        shared_ptr<IMirrorPlayer> mirrorPlayer = napiPlayer->GetMirrorPlayer();
        CHECK_ARGS_RETURN_VOID(napiAsyntask, mirrorPlayer, "IMirrorPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        CLOGI("SetAppInfo in");
        int32_t ret = mirrorPlayer->SetAppInfo({napiAsyntask->appUid, napiAsyntask->appTokenId, napiAsyntask->appPid});
        CLOGI("SetAppInfo out");
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "SetAppInfo failed : no permission";
            } else {
                napiAsyntask->errMessage = "SetAppInfo failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };
    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "SetAppInfo", executor, complete);
}

napi_value NapiMirrorPlayer::Release(napi_env env, napi_callback_info info)
{
    CLOGD("Start to release in");
    auto napiAsyntask = std::make_shared<NapiAsyncTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }
    napiAsyntask->GetJSInfo(env, info);

    auto executor = [napiAsyntask]() {
        auto *napiPlayer = reinterpret_cast<NapiMirrorPlayer *>(napiAsyntask->native);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, napiPlayer != nullptr, "napiPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        shared_ptr<IMirrorPlayer> mirrorPlayer = napiPlayer->GetMirrorPlayer();
        CHECK_ARGS_RETURN_VOID(napiAsyntask, mirrorPlayer, "IMirrorPlayer is null",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        int32_t ret = mirrorPlayer->Release();
        if (ret == CAST_ENGINE_SUCCESS) {
            napiPlayer->Reset();
        } else if (ret == ERR_NO_PERMISSION) {
            napiAsyntask->errMessage = "Release failed : no permission";
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        } else {
            napiAsyntask->errMessage = "Release failed : native server exception";
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "Release", executor, complete);
}

napi_value NapiMirrorPlayer::ResizeVirtualScreen(napi_env env, napi_callback_info info)
{
    struct ConcreteTask : public NapiAsyncTask {
        uint32_t width_;
        uint32_t height_;
    };
    auto napiAsyntask = std::make_shared<ConcreteTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }

    auto inputParser = [env, napiAsyntask](size_t argc, napi_value *argv) {
        constexpr size_t expectedArgc = 2;
        CHECK_ARGS_RETURN_VOID(napiAsyntask, argc == expectedArgc, "invalid arguments",
                               NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napi_valuetype expectedTypes[expectedArgc] = { napi_number, napi_number };
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
                               NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->width_ = ParseUint32(env, argv[0]);
        napiAsyntask->height_ = ParseUint32(env, argv[1]);
    };
    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        auto *napiMirrorPlayer = reinterpret_cast<NapiMirrorPlayer *>(napiAsyntask->native);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, napiMirrorPlayer != nullptr, "napiMirrorPlayer is null",
                               NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        std::shared_ptr<IMirrorPlayer> mirrorPlayer = napiMirrorPlayer->GetMirrorPlayer();
        CHECK_ARGS_RETURN_VOID(napiAsyntask, mirrorPlayer, "IMirrorPlayer is null",
                               NapiErrors::errcode_[CAST_ENGINE_ERROR]);
        int32_t ret = mirrorPlayer->ResizeVirtualScreen(napiAsyntask->width_, napiAsyntask->height_);
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "ResizeVirtualScreen failed : no permission";
            } else if (ret == ERR_INVALID_PARAM) {
                napiAsyntask->errMessage = "ResizeVirtualScreen failed : invalid parameters";
            } else {
                napiAsyntask->errMessage = "ResizeVirtualScreen failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "ResizeVirtualScreen", executor, complete);
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS