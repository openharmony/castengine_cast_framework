/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: supply cast session listener implement proxy apis.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef CAST_SESSION_LISTENER_STUB_H
#define CAST_SESSION_LISTENER_STUB_H

#include "cast_stub_helper.h"
#include "i_cast_session_listener_impl.h"
#include "iremote_stub.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class CastSessionListenerImplStub : public IRemoteStub<ICastSessionListenerImpl> {
public:
    CastSessionListenerImplStub(std::shared_ptr<ICastSessionListener> userListener);

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DECLARE_STUB_TASK_MAP(CastSessionListenerImplStub);

    int32_t DoOnDeviceStateTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoOnEventTask(MessageParcel &data, MessageParcel &reply);
    void OnDeviceState(const DeviceStateInfo &stateInfo) override;
    void OnEvent(const EventId &eventId, const std::string &jsonParam) override;
    std::shared_ptr<ICastSessionListener> userListener_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS

#endif