/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: supply napi interface for cast session manager.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#ifndef NAPI_CAST_SESSION_MANAGER_H_
#define NAPI_CAST_SESSION_MANAGER_H_

#include <map>
#include <memory>
#include <list>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "napi_errors.h"
#include "cast_engine_errors.h"
#include "cast_session_manager.h"
#include "napi_cast_session_manager_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class EXPORT NapiCastSessionManager {
public:
    using OnEventHandlerType = std::function<napi_status(napi_env, napi_value)>;
    using OffEventHandlerType = std::function<napi_status(napi_env, napi_value)>;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value StartDiscovery(napi_env env, napi_callback_info info);
    static napi_value StopDiscovery(napi_env env, napi_callback_info info);
    static napi_value SetDiscoverable(napi_env env, napi_callback_info info);
    static napi_value CreateCastSession(napi_env env, napi_callback_info info);
    static napi_value Release(napi_env env, napi_callback_info info);

    static napi_value OnEvent(napi_env env, napi_callback_info info);
    static napi_value OffEvent(napi_env env, napi_callback_info info);

    static napi_status OnServiceDied(napi_env env, napi_value callback);
    static napi_status OnDeviceFound(napi_env env, napi_value callback);
    static napi_status OnSessionCreated(napi_env env, napi_value callback);
    static napi_status OnDeviceOffline(napi_env env, napi_value callback);

    static napi_status OffServiceDie(napi_env env, napi_value callback);
    static napi_status OffDeviceFound(napi_env env, napi_value callback);
    static napi_status OffSessionCreated(napi_env env, napi_value callback);
    static napi_status OffDeviceOffline(napi_env env, napi_value callback);

    static napi_status RegisterNativeSessionManagerListener();
    static std::map<std::string, std::pair<OnEventHandlerType, OffEventHandlerType>> eventHandlers_;
    static std::shared_ptr<NapiCastSessionManagerListener> listener_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif