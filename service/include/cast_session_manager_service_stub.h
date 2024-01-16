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
 * Description:  cast session manager service stub class.
 * Author: zhangge
 * Create: 2022-5-29
 */

#ifndef CAST_SESSION_MANAGER_SERVICE_STUB_H
#define CAST_SESSION_MANAGER_SERVICE_STUB_H

#include "cast_stub_helper.h"
#include "i_cast_session_manager_service.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastSessionManagerServiceStub : public IRemoteStub<ICastSessionManagerService> {
public:
    CastSessionManagerServiceStub();
    ~CastSessionManagerServiceStub() override;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DECLARE_STUB_TASK_MAP(CastSessionManagerServiceStub);

    int32_t DoRegisterListenerTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoUnregisterListenerTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoReleaseTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoSetLocalDeviceTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoCreateCastSessionTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoSetSinkSessionCapacityTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoStartDiscoveryTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoSetDiscoverableTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoStopDiscoveryTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoGetCastSessionTask(MessageParcel &data, MessageParcel &reply);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS


#endif // CAST_SESSION_MANAGER_SERVICE_STUB_H