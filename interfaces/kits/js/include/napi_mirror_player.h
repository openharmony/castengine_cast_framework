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
 * Description: supply napi interface for cast mirror player.
 * Author: zhangjingnan
 * Create: 2023-5-27
 */

#ifndef NAPI_CAST_MIRROR_PLAYER_H_
#define NAPI_CAST_MIRROR_PLAYER_H_

#include <map>
#include <memory>
#include <mutex>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "napi_errors.h"
#include "cast_engine_errors.h"
#include "i_mirror_player.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class NapiMirrorPlayer {
public:
    static void DefineMirrorPlayerJSClass(napi_env env);
    static napi_status CreateNapiMirrorPlayer(napi_env env, std::shared_ptr<IMirrorPlayer> mirrorPlayer,
        napi_value &out);

    NapiMirrorPlayer(std::shared_ptr<IMirrorPlayer> mirrorPlayer) : mirrorPlayer_(mirrorPlayer) {}
    NapiMirrorPlayer() = default;
    ~NapiMirrorPlayer() = default;
    std::shared_ptr<IMirrorPlayer> GetMirrorPlayer()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return mirrorPlayer_;
    }
    void Reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        mirrorPlayer_.reset();
    }

private:
    static napi_value NapiMirrorPlayerConstructor(napi_env env, napi_callback_info info);
    static napi_value Play(napi_env env, napi_callback_info info);
    static napi_value Pause(napi_env env, napi_callback_info info);
    static napi_value SetAppInfo(napi_env env, napi_callback_info info);
    static napi_value SetSurface(napi_env env, napi_callback_info info);
    static napi_value Release(napi_env env, napi_callback_info info);
    static napi_value ResizeVirtualScreen(napi_env env, napi_callback_info info);

    std::mutex mutex_;
    std::shared_ptr<IMirrorPlayer> mirrorPlayer_;
    static thread_local napi_ref consRef_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif