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
 * Description: supply napi interface realization for cast session manager.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#include <memory>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "cast_session_manager.h"
#include "napi_cast_session_manager_listener.h"
#include "napi_cast_session.h"
#include "napi_castengine_utils.h"
#include "napi_cast_session_manager.h"
#include "napi_async_work.h"

using namespace OHOS::CastEngine::CastEngineClient;
using namespace std;

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Napi-SessionManager");

std::shared_ptr<NapiCastSessionManagerListener> NapiCastSessionManager::listener_;

std::map<std::string, std::pair<NapiCastSessionManager::OnEventHandlerType,
    NapiCastSessionManager::OffEventHandlerType>>
    NapiCastSessionManager::eventHandlers_ = {
    { "serviceDie", { OnServiceDied, OffServiceDie } },
    { "deviceFound", { OnDeviceFound, OffDeviceFound } },
    { "sessionCreate", { OnSessionCreated, OffSessionCreated } },
    { "deviceOffline", { OnDeviceOffline, OffDeviceOffline } }
};

napi_value NapiCastSessionManager::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor NapiCastSessionManagerDesc[] = {
        DECLARE_NAPI_FUNCTION("on", OnEvent),
        DECLARE_NAPI_FUNCTION("off", OffEvent),
        DECLARE_NAPI_FUNCTION("startDiscovery", StartDiscovery),
        DECLARE_NAPI_FUNCTION("stopDiscovery", StopDiscovery),
        DECLARE_NAPI_FUNCTION("setDiscoverable", SetDiscoverable),
        DECLARE_NAPI_FUNCTION("createCastSession", CreateCastSession),
        DECLARE_NAPI_FUNCTION("release", Release)
    };

    napi_status status = napi_define_properties(env, exports,
        sizeof(NapiCastSessionManagerDesc) / sizeof(napi_property_descriptor), NapiCastSessionManagerDesc);
    if (status != napi_ok) {
        CLOGE("define manager properties failed");
        return GetUndefinedValue(env);
    }
    return exports;
}

napi_value NapiCastSessionManager::StartDiscovery(napi_env env, napi_callback_info info)
{
    CLOGD("Start to discovery in");
    struct ConcreteTask : public NapiAsyncTask {
        int protocolType_;
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
        napi_valuetype expectedTypes[expectedArgc] = { napi_object };
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        int32_t protocolTypeInt;
        bool isProtocolTypesValid = GetProtocolTypesFromJS(env, argv[0], protocolTypeInt);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isProtocolTypesValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->protocolType_ = protocolTypeInt;
    };
    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        int32_t ret = CastSessionManager::GetInstance().StartDiscovery(napiAsyntask->protocolType_);
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "StartDiscovery failed : no permission";
            } else if (ret == ERR_INVALID_PARAM) {
                napiAsyntask->errMessage = "StartDiscovery failed : invalid parameters";
            } else {
                napiAsyntask->errMessage = "StartDiscovery failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "StartDiscovery", executor, complete);
}

napi_value NapiCastSessionManager::StopDiscovery(napi_env env, napi_callback_info info)
{
    CLOGD("Start to stop discovery in");
    auto napiAsyntask = std::make_shared<NapiAsyncTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }
    napiAsyntask->GetJSInfo(env, info);

    auto executor = [napiAsyntask]() {
        int32_t ret = CastSessionManager::GetInstance().StopDiscovery();
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "StopDiscovery failed : no permission";
            } else {
                napiAsyntask->errMessage = "StopDiscovery failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };
    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "StopDiscovery", executor, complete);
}

napi_value NapiCastSessionManager::SetDiscoverable(napi_env env, napi_callback_info info)
{
    CLOGD("Start to set discoverable in");
    struct ConcreteTask : public NapiAsyncTask {
        bool isEnable_;
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
        napi_valuetype expectedTypes[expectedArgc] = { napi_boolean };
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->isEnable_ = ParseBool(env, argv[0]);
    };
    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        int32_t ret = CastSessionManager::GetInstance().SetDiscoverable(napiAsyntask->isEnable_);
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "SetDiscoverable failed : no permission";
            } else {
                napiAsyntask->errMessage = "SetDiscoverable failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "SetDiscoverable", executor, complete);
}

napi_value NapiCastSessionManager::CreateCastSession(napi_env env, napi_callback_info info)
{
    CLOGD("Start to create castSession in");
    struct ConcreteTask : public NapiAsyncTask {
        CastSessionProperty property_;
        std::shared_ptr<ICastSession> session_;
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
        napi_valuetype expectedTypes[expectedArgc] = { napi_object };
        bool isParamsTypeValid = CheckJSParamsType(env, argv, expectedArgc, expectedTypes);
        CHECK_ARGS_RETURN_VOID(napiAsyntask, isParamsTypeValid, "invalid arguments",
            NapiErrors::errcode_[ERR_INVALID_PARAM]);
        napiAsyntask->property_ = GetCastSessionPropertyFromJS(env, argv[0]);
    };
    napiAsyntask->GetJSInfo(env, info, inputParser);
    auto executor = [napiAsyntask]() {
        int32_t ret =
            CastSessionManager::GetInstance().CreateCastSession(napiAsyntask->property_, napiAsyntask->session_);
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "CreateCastSession failed : no permission";
            } else if (ret == ERR_INVALID_PARAM) {
                napiAsyntask->errMessage = "CreateCastSession failed : invalid parameters";
            } else {
                napiAsyntask->errMessage = "CreateCastSession failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };

    auto complete = [napiAsyntask](napi_value &output) {
        napiAsyntask->status =
            NapiCastSession::CreateNapiCastSession(napiAsyntask->env, napiAsyntask->session_, output);
        CHECK_STATUS_RETURN_VOID(napiAsyntask, "convert native object to javascript object failed",
            NapiErrors::errcode_[CAST_ENGINE_ERROR]);
    };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "CreateCastSession", executor, complete);
}

napi_value NapiCastSessionManager::Release(napi_env env, napi_callback_info info)
{
    CLOGD("Start to release in");
    auto napiAsyntask = std::make_shared<NapiAsyncTask>();
    if (napiAsyntask == nullptr) {
        CLOGE("Create NapiAsyncTask failed");
        return GetUndefinedValue(env);
    }
    napiAsyntask->GetJSInfo(env, info);

    auto executor = [napiAsyntask]() {
        int32_t ret = CastSessionManager::GetInstance().Release();
        if (ret != CAST_ENGINE_SUCCESS) {
            if (ret == ERR_NO_PERMISSION) {
                napiAsyntask->errMessage = "Release failed : no permission";
            } else {
                napiAsyntask->errMessage = "Release failed : native server exception";
            }
            napiAsyntask->status = napi_generic_failure;
            napiAsyntask->errCode = NapiErrors::errcode_[ret];
        }
    };
    auto complete = [env](napi_value &output) { output = GetUndefinedValue(env); };
    return NapiAsyncWork::Enqueue(env, napiAsyntask, "Release", executor, complete);
}

napi_value NapiCastSessionManager::OnEvent(napi_env env, napi_callback_info info)
{
    constexpr size_t expectedArgc = 2;
    napi_value argv[expectedArgc] = { 0 };
    napi_valuetype expectedTypes[expectedArgc] = { napi_string, napi_function };
    if (!GetJSFuncParams(env, info, argv, expectedArgc, expectedTypes)) {
        return GetUndefinedValue(env);
    }

    std::string eventName = ParseString(env, argv[0]);
    auto it = eventHandlers_.find(eventName);
    if (it == eventHandlers_.end()) {
        CLOGE("event name invalid");
        return GetUndefinedValue(env);
    }

    if (RegisterNativeSessionManagerListener() == napi_generic_failure) {
        return GetUndefinedValue(env);
    }
    if (it->second.first(env, argv[1]) != napi_ok) {
        CLOGE("event name invalid");
    }

    return GetUndefinedValue(env);
}

napi_value NapiCastSessionManager::OffEvent(napi_env env, napi_callback_info info)
{
    constexpr size_t expectedArgc = 2;
    napi_value argv[expectedArgc] = { 0 };
    napi_valuetype expectedTypes[expectedArgc] = { napi_string, napi_function};
    if (!GetJSFuncParams(env, info, argv, expectedArgc, expectedTypes)) {
        return GetUndefinedValue(env);
    }

    std::string eventName = ParseString(env, argv[0]);
    auto it = eventHandlers_.find(eventName);
    if (it == eventHandlers_.end()) {
        CLOGE("event name invalid");
        return GetUndefinedValue(env);
    }
    if (it->second.second(env, argv[1]) != napi_ok) {
        CLOGE("event name invalid");
    }

    return GetUndefinedValue(env);
}

napi_status NapiCastSessionManager::OnServiceDied(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->AddCallback(env, NapiCastSessionManagerListener::EVENT_SERVICE_DIED, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::OnDeviceFound(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->AddCallback(env, NapiCastSessionManagerListener::EVENT_DEVICE_FOUND, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::OnSessionCreated(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->AddCallback(env, NapiCastSessionManagerListener::EVENT_SESSION_CREATE, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::OnDeviceOffline(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->AddCallback(env, NapiCastSessionManagerListener::EVENT_DEVICE_OFFLINE, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::OffServiceDie(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->RemoveCallback(env, NapiCastSessionManagerListener::EVENT_SERVICE_DIED, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::OffDeviceFound(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->RemoveCallback(env, NapiCastSessionManagerListener::EVENT_DEVICE_FOUND, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::OffSessionCreated(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->RemoveCallback(env, NapiCastSessionManagerListener::EVENT_SESSION_CREATE, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::OffDeviceOffline(napi_env env, napi_value callback)
{
    if (!listener_) {
        CLOGE("cast session manager callback is null");
        return napi_generic_failure;
    }
    if (listener_->RemoveCallback(env, NapiCastSessionManagerListener::EVENT_DEVICE_OFFLINE, callback) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status NapiCastSessionManager::RegisterNativeSessionManagerListener()
{
    if (listener_) {
        return napi_ok;
    }
    listener_ = std::make_shared<NapiCastSessionManagerListener>();
    if (!listener_) {
        CLOGE("Failed to malloc session manager listener");
        return napi_generic_failure;
    }
    int32_t ret = CastSessionManager::GetInstance().RegisterListener(listener_);
    if (ret != CAST_ENGINE_SUCCESS) {
        CLOGE("native register session manager listener failed");
        return napi_generic_failure;
    }

    return napi_ok;
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS