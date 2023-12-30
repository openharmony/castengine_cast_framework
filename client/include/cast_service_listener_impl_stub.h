/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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