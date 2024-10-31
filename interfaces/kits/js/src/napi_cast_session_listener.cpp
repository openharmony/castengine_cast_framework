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
}

void NapiCastSessionListener::HandleEvent(int32_t event, NapiArgsGetter &getter)
{
    std::lock_guard<std::mutex> lockGuard(lock_);
    if (!callback_) {
        CLOGE("callback_ is nullptr, event:%{public}d", event);
        return;
    }
    callback_->HandleEvent(event, getter);
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
    std::lock_guard<std::mutex> lockGuard(lock_);
    if (callback_ == nullptr) {
        callback_ = std::make_shared<NapiCallback>(env);
        if (callback_ == nullptr) {
            CLOGE("no memory");
            return napi_generic_failure;
        }
    }
    return callback_->AddCallback(env, event, callback);
}

napi_status NapiCastSessionListener::RemoveCallback(napi_env env, int32_t event, napi_value callback)
{
    CLOGI("Remove callback %{public}d", event);
    std::lock_guard<std::mutex> lockGuard(lock_);
    if (!callback_) {
        CLOGE("no memory");
        return napi_ok;
    }
    return callback_->RemoveCallback(env, event, callback);
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
