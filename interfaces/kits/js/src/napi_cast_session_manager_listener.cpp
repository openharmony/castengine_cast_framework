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
 * Description: supply cast session manager listener realization for napi interface.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#include <uv.h>
#include <memory>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "cast_session_manager.h"
#include "napi_cast_session.h"
#include "napi_castengine_utils.h"
#include "napi_cast_session_manager_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Napi-SessionManagerListener");

NapiCastSessionManagerListener::~NapiCastSessionManagerListener()
{
    CLOGD("destrcutor in");
    ClearCallback(callback_->GetEnv());
}

void NapiCastSessionManagerListener::HandleEvent(int32_t event, NapiArgsGetter getter)
{
    std::lock_guard<std::mutex> lockGuard(lock_);
    if (callbacks_[event].empty()) {
        CLOGE("not register callback event=%{public}d", event);
        return;
    }
    for (auto ref = callbacks_[event].begin(); ref != callbacks_[event].end(); ++ref) {
        callback_->Call(*ref, getter);
    }
}

void NapiCastSessionManagerListener::OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList)
{
    CLOGD("OnDeviceFound start");
    NapiArgsGetter napiArgsGetter = [deviceList](napi_env env, int &argc, napi_value *argv) {
        argc = CALLBACK_ARGC_ONE;
        argv[0] = ConvertDeviceListToJS(env, deviceList);
    };
    HandleEvent(EVENT_DEVICE_FOUND, napiArgsGetter);
    CLOGD("OnDeviceFound finish");
}

void NapiCastSessionManagerListener::OnDeviceOffline(const std::string &deviceId)
{
    CLOGD("OnDeviceOffline start");
    NapiArgsGetter napiArgsGetter = [deviceId](napi_env env, int &argc, napi_value *argv) {
        argc = CALLBACK_ARGC_ONE;
        auto status = napi_create_string_utf8(env, deviceId.c_str(), NAPI_AUTO_LENGTH, &argv[0]);
        CHECK_RETURN_VOID(status == napi_ok, "napi_create_string_utf8 failed");
    };
    HandleEvent(EVENT_DEVICE_OFFLINE, napiArgsGetter);
    CLOGD("OnDeviceOffline finish");
}

void NapiCastSessionManagerListener::OnSessionCreated(const std::shared_ptr<ICastSession> &castSession)
{
    CLOGD("OnSessionCreated start");
    NapiArgsGetter napiArgsGetter = [castSession](napi_env env, int &argc, napi_value *argv) {
        argc = CALLBACK_ARGC_ONE;
        argv[0] = ConvertCastSessionToJS(env, castSession);
    };
    HandleEvent(EVENT_SESSION_CREATE, napiArgsGetter);
    CLOGD("OnSessionCreated finish");
}

void NapiCastSessionManagerListener::OnServiceDied()
{
    CLOGD("OnServiceDied start");
    NapiArgsGetter napiArgsGetter = [](napi_env env, int &argc, napi_value *argv) {
        argc = CALLBACK_ARGC_ZERO;
        argv[0] = { nullptr };
    };
    HandleEvent(EVENT_SERVICE_DIED, napiArgsGetter);
    CLOGD("OnServiceDied finish");
}

napi_status NapiCastSessionManagerListener::AddCallback(napi_env env, int32_t event, napi_value callback)
{
    CLOGI("Add callback %{public}d", event);
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
    if (callback_ == nullptr) {
        callback_ = std::make_shared<NapiCallback>(env);
        if (callback_ == nullptr) {
            CLOGE("no memory");
            return napi_generic_failure;
        }
    }
    callbacks_[event].push_back(ref);
    return napi_ok;
}

napi_status NapiCastSessionManagerListener::RemoveCallback(napi_env env, int32_t event, napi_value callback)
{
    if (callback == nullptr) {
        for (auto &callbackRef : callbacks_[event]) {
            napi_status ret = napi_delete_reference(env, callbackRef);
            if (ret != napi_ok) {
                CLOGE("delete callback reference failed");
                return ret;
            }
        }
        callbacks_[event].clear();
        return napi_ok;
    }
    napi_ref ref = nullptr;
    if (GetRefByCallback(env, callbacks_[event], callback, ref) != napi_ok) {
        CLOGE("get callback reference failed");
        return napi_generic_failure;
    }
    if (ref != nullptr) {
        CLOGD("callback has been remove");
        return napi_ok;
    }
    callbacks_[event].remove(ref);
    return napi_delete_reference(env, ref);
}

napi_status NapiCastSessionManagerListener::ClearCallback(napi_env env)
{
    for (auto &callback : callbacks_) {
        for (auto &callbackRef : callback) {
            napi_status ret = napi_delete_reference(env, callbackRef);
            if (ret != napi_ok) {
                CLOGE("delete callback reference failed");
                return ret;
            }
        }
        callback.clear();
    }

    return napi_ok;
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS