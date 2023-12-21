/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: supply cast session listener implement stub.
 * Author: zhangge
 * Create: 2022-6-15
 */

#include "cast_session_listener_impl_stub.h"

#include "cast_engine_common_helper.h"
#include "cast_stub_helper.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-SessionListener");

int CastSessionListenerImplStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    RETRUEN_IF_WRONG_TASK(code, data, reply, option);
    return EXECUTE_SINGLE_STUB_TASK(code, data, reply);
}

CastSessionListenerImplStub::CastSessionListenerImplStub(std::shared_ptr<ICastSessionListener> userListener_)
    : userListener_(userListener_)
{
    FILL_SINGLE_STUB_TASK(ON_DEVICE_STATE, &CastSessionListenerImplStub::DoOnDeviceStateTask);
    FILL_SINGLE_STUB_TASK(ON_EVENT, &CastSessionListenerImplStub::DoOnEventTask);
}

int32_t CastSessionListenerImplStub::DoOnDeviceStateTask(MessageParcel &data, MessageParcel &reply)
{
    static_cast<void>(reply);
    auto stateInfo = ReadDeviceStateInfo(data);
    if (stateInfo == nullptr) {
        CLOGE("sate info is null");
        return ERR_NULL_OBJECT;
    }
    userListener_->OnDeviceState(*stateInfo);

    return ERR_NONE;
}

int32_t CastSessionListenerImplStub::DoOnEventTask(MessageParcel &data, MessageParcel &reply)
{
    static_cast<void>(reply);
    int32_t eventId;
    std::string param;
    if (!ReadEvent(data, eventId, param)) {
        CLOGE("read event failed");
        return ERR_NULL_OBJECT;
    }
    userListener_->OnEvent(static_cast<EventId>(eventId), param);

    return ERR_NONE;
}

void CastSessionListenerImplStub::OnDeviceState(const DeviceStateInfo &stateInfo)
{
    static_cast<void>(stateInfo);
}

void CastSessionListenerImplStub::OnEvent(const EventId &eventId, const std::string &jsonParam)
{
    static_cast<void>(eventId);
    static_cast<void>(jsonParam);
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
