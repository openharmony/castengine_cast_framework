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
 * Description: supply cast session listener for napi interface.
 * Author: zhangjingnan
 * Create: 2022-7-11
 */

#ifndef NAPI_CAST_SESSION_LISTENER_H
#define NAPI_CAST_SESSION_LISTENER_H

#include <memory>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include <list>
#include "i_cast_session.h"
#include "napi_callback.h"
#include "cast_engine_common.h"
#include "napi_castengine_utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class NapiCastSessionListener : public ICastSessionListener {
public:
    using NapiArgsGetter = std::function<void(napi_env env, int &argc, napi_value *argv)>;
    enum {
        EVENT_ON_EVENT,
        EVENT_DEVICE_STATE,
        EVENT_REMOTE_CTRL,
        EVENT_TYPE_MAX
    };
    NapiCastSessionListener() {};
    ~NapiCastSessionListener() override;

    void OnDeviceState(const DeviceStateInfo &stateEvent) override;
    void OnEvent(const EventId &eventId, const std::string &jsonParam) override;
    void OnRemoteCtrlEvent(int eventType, const uint8_t *data, uint32_t len) override;

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