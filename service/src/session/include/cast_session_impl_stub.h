/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast session implement stub class.
 * Author: zhangge
 * Create: 2022-06-15
 */

#ifndef CAST_SESSION_IMPL_STUB_H
#define CAST_SESSION_IMPL_STUB_H

#include "cast_stub_helper.h"
#include "i_cast_session_impl.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastSessionImplStub : public IRemoteStub<ICastSessionImpl> {
public:
    CastSessionImplStub();

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DECLARE_STUB_TASK_MAP(CastSessionImplStub);

    int32_t DoRegisterListenerTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoUnregisterListenerTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoAddDeviceTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoRemoveDeviceTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoStartAuthTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoGetSessionIdTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoGetDeviceStateTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoSetSessionProperty(MessageParcel &data, MessageParcel &reply);
    int32_t DoCreateMirrorPlayer(MessageParcel &data, MessageParcel &reply);
    int32_t DoCreateStreamPlayer(MessageParcel &data, MessageParcel &reply);
    int32_t DoNotifyEvent(MessageParcel &data, MessageParcel &reply);
    int32_t DoSetCastMode(MessageParcel &data, MessageParcel &reply);
    int32_t DoRelease(MessageParcel &data, MessageParcel &reply);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
