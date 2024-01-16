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
 * Description: supply cast service listener implement proxy
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef CAST_SERVICE_LISTENER_IMPL_STUB_H
#define CAST_SERVICE_LISTENER_IMPL_STUB_H

#include "cast_engine_common.h"
#include "cast_stub_helper.h"
#include "i_cast_service_listener_impl.h"
#include "i_cast_session_manager_listener.h"
#include "iremote_stub.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class CastServiceListenerImplStub : public IRemoteStub<ICastServiceListenerImpl> {
public:
    CastServiceListenerImplStub(std::shared_ptr<ICastSessionManagerListener> userListener);

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DECLARE_STUB_TASK_MAP(CastServiceListenerImplStub);

    bool GetCastRemoteDevices(MessageParcel &data, std::vector<CastRemoteDevice> &deviceList);
    bool GetCastSession(MessageParcel &data, std::shared_ptr<ICastSession> &castSession);
    int32_t DoDeviceFoundTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoDeviceOfflineTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoSessionCreateTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoServiceDieTask(MessageParcel &data, MessageParcel &reply);
    void OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList) override;
    void OnDeviceOffline(const std::string &deviceId) override;
    void OnSessionCreated(const sptr<ICastSessionImpl> &castSession) override;
    void OnServiceDied() override;

    std::shared_ptr<ICastSessionManagerListener> userListener_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif