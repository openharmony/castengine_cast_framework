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
 * Description: supply napi interface for cast session.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#ifndef NAPI_CAST_SESSION_H_
#define NAPI_CAST_SESSION_H_

#include <map>
#include <memory>
#include <mutex>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "napi_errors.h"
#include "cast_engine_errors.h"
#include "i_cast_session.h"
#include "napi_cast_session_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class EXPORT NapiCastSession {
public:
    using OnEventHandlerType = std::function<napi_status(napi_env, napi_value, NapiCastSession *)>;
    using OffEventHandlerType = std::function<napi_status(napi_env, napi_value, NapiCastSession *)>;

    static void DefineCastSessionJSClass(napi_env env);
    static napi_status CreateNapiCastSession(napi_env env, std::shared_ptr<ICastSession> session, napi_value &out);
    std::shared_ptr<NapiCastSessionListener> NapiListenerGetter();
    void NapiListenerSetter(std::shared_ptr<NapiCastSessionListener> listener);

    NapiCastSession(std::shared_ptr<ICastSession> session) : session_(session) {}
    NapiCastSession() = default;
    ~NapiCastSession() = default;
    std::shared_ptr<ICastSession> GetCastSession()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return session_;
    }
    void Reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        session_.reset();
        listener_.reset();
    }

private:
    static napi_value NapiCastSessionConstructor(napi_env env, napi_callback_info info);
    static napi_value AddDevice(napi_env env, napi_callback_info info);
    static napi_value RemoveDevice(napi_env env, napi_callback_info info);
    static napi_value GetSessionId(napi_env env, napi_callback_info info);
    static napi_value SetSessionProperty(napi_env env, napi_callback_info info);
    static napi_value CreateMirrorPlayer(napi_env env, napi_callback_info info);
    static napi_value CreateStreamPlayer(napi_env env, napi_callback_info info);
    static napi_value SetCastMode(napi_env env, napi_callback_info info);
    static napi_value Release(napi_env env, napi_callback_info info);

    static napi_value OnEvent(napi_env env, napi_callback_info info);
    static napi_value OffEvent(napi_env env, napi_callback_info info);

    static napi_status OnInnerEvent(napi_env env, napi_value callback, NapiCastSession *napiSession);
    static napi_status OnDeviceState(napi_env env, napi_value callback, NapiCastSession *napiSession);

    static napi_status OffInnerEvent(napi_env env, napi_value callback, NapiCastSession *napiSession);
    static napi_status OffDeviceState(napi_env env, napi_value callback, NapiCastSession *napiSession);

    static NapiCastSession *GetNapiCastSession(napi_env env, napi_callback_info info);
    static napi_status RegisterNativeSessionListener(NapiCastSession *napiSession);

    static std::map<std::string, std::pair<OnEventHandlerType, OffEventHandlerType>> eventHandlers_;
    std::mutex mutex_;
    std::shared_ptr<ICastSession> session_;
    std::shared_ptr<NapiCastSessionListener> listener_;
    static thread_local napi_ref consRef_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif