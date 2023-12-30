/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
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