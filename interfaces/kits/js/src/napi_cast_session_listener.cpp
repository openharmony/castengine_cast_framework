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
 * Description: supply cast session listener realization for napi interface.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#include <uv.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "napi_castengine_utils.h"
#include "securec.h"
#include "napi_cast_session_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Napi-SessionListener");

NapiCastSessionListener::~NapiCastSessionListener()
{
    CLOGD("destrcutor in");
    ClearCallback(callback_->GetEnv());
}

void NapiCastSessionListener::HandleEvent(int32_t event, NapiArgsGetter getter)
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

void NapiCastSessionListener::OnDeviceState(const DeviceStateInfo &stateEvent)
{
    CLOGD("OnDeviceState start");
    NapiArgsGetter napiArgsGetter = [stateEvent](napi_env env, int &argc, napi_value *argv) {
        argc = CALLBACK_ARGC_ONE;
        argv[0] = ConvertDeviceStateInfoToJS(env, stateEvent);
    };
    HandleEvent(EVENT_DEVICE_STATE, napiArgsGetter);
    CLOGD("OnDeviceState finish");
}

void NapiCastSessionListener::OnEvent(const EventId &eventId, const std::string &jsonParam)
{
    CLOGD("OnEvent start");
    NapiArgsGetter napiArgsGetter = [eventId, jsonParam](napi_env env, int &argc, napi_value *argv) {
        argc = CALLBACK_ARGC_TWO;
        auto status = napi_create_int32(env, static_cast<int32_t>(eventId), &argv[0]);
        CHECK_RETURN_VOID(status == napi_ok, "napi_create_int32 failed");
        status = napi_create_string_utf8(env, jsonParam.c_str(), NAPI_AUTO_LENGTH, &argv[1]);
        CHECK_RETURN_VOID(status == napi_ok, "napi_create_string_utf8 failed");
    };
    HandleEvent(EVENT_ON_EVENT, napiArgsGetter);
    CLOGD("OnEvent finish");
}

void NapiCastSessionListener::OnRemoteCtrlEvent(int eventType, const uint8_t *data, uint32_t len)
{
    CLOGD("OnRemoteCtrlEvent start");
    std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(len);
    if (!buf) {
        CLOGE("buf is null");
        return;
    }
    if (memcpy_s(buf.get(), len, data, len) != EOK) {
        CLOGE("Copy data failed.");
        return;
    }
    std::shared_ptr<uint8_t[]> dataBuf = std::move(buf);
    NapiArgsGetter getter = [eventType, dataBuf, len](napi_env env, int &argc, napi_value *argv) {
        argc = CALLBACK_ARGC_THERE;
        auto status = napi_create_int32(env, static_cast<int32_t>(eventType), &argv[0]);
        CHECK_RETURN_VOID(status == napi_ok, "napi_create_int32 failed");
        void *data = dataBuf.get();
        status = napi_create_arraybuffer(env, static_cast<size_t>(len), &data, &argv[1]);
        CHECK_RETURN_VOID(status == napi_ok, "napi_create_arraybuffer failed");
    };
    HandleEvent(EVENT_REMOTE_CTRL, getter);
    CLOGD("OnRemoteCtrlEvent finish");
}

napi_status NapiCastSessionListener::AddCallback(napi_env env, int32_t event, napi_value callback)
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

napi_status NapiCastSessionListener::RemoveCallback(napi_env env, int32_t event, napi_value callback)
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

napi_status NapiCastSessionListener::ClearCallback(napi_env env)
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
