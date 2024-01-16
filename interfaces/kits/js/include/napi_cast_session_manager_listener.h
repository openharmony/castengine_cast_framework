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
 * Description: supply cast session manager listener for napi interface.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#ifndef NAPI_CAST_SESSION_MANAGER_LISTENER_H
#define NAPI_CAST_SESSION_MANAGER_LISTENER_H

#include <memory>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include <list>
#include "cast_engine_common.h"
#include "i_cast_session_manager_listener.h"
#include "napi_callback.h"
#include "napi_castengine_utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class NapiCastSessionManagerListener : public ICastSessionManagerListener {
public:
    using NapiArgsGetter = std::function<void(napi_env env, int &argc, napi_value *argv)>;
    enum {
        EVENT_SERVICE_DIED,
        EVENT_DEVICE_FOUND,
        EVENT_SESSION_CREATE,
        EVENT_DEVICE_OFFLINE,
        EVENT_TYPE_MAX
    };
    NapiCastSessionManagerListener() {};
    ~NapiCastSessionManagerListener() override;

    void OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList) override;
    void OnDeviceOffline(const std::string &deviceId) override;
    void OnSessionCreated(const std::shared_ptr<ICastSession> &castSession) override;
    void OnServiceDied() override;

    napi_status AddCallback(napi_env env, int32_t event, napi_value callback);
    napi_status RemoveCallback(napi_env env, int32_t event, napi_value callback);

private:
    void HandleEvent(int32_t event, NapiArgsGetter getter);
    napi_status ClearCallback(napi_env env);

    std::mutex lock_;
    std::shared_ptr<NapiCallback> callback_;
    std::list<napi_ref> callbacks_[EVENT_TYPE_MAX] {};
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif