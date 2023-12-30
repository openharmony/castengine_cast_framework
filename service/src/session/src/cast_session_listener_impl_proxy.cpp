/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast session listener implement proxy class.
 * Author: zhangge
 * Create: 2022-6-15
 */

#include "cast_session_listener_impl_proxy.h"

#include "cast_engine_common_helper.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Session-Listener");

void CastSessionListenerImplProxy::OnDeviceState(const DeviceStateInfo &stateInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!WriteDeviceStateInfo(data, stateInfo)) {
        CLOGE("Failed to write the state info");
        return;
    }
    if (Remote()->SendRequest(ON_DEVICE_STATE, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when reporting cast session event");
    }
}

void CastSessionListenerImplProxy::OnEvent(const EventId &eventId, const std::string &jsonParam)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!WriteEvent(data, eventId, jsonParam)) {
        CLOGE("Failed to write the event");
        return;
    }
    if (Remote()->SendRequest(ON_EVENT, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when reporting cast session event");
    }
}

void CastSessionListenerImplProxy::OnRemoteCtrlEvent(int eventType, const uint8_t *data, uint32_t len)
{
    MessageParcel parcelData;
    MessageParcel reply;
    MessageOption option;
    CLOGE("OnRemoteCtrlEvent in eventType:%{public}d len:%{public}u", eventType, len);
    if (len > parcelData.GetDataCapacity()) {
        CLOGD("OnBytesReceived SetDataCapacity totalSize: %u", len);
        parcelData.SetMaxCapacity(len + len);
        parcelData.SetDataCapacity(len);
    }

    if (!parcelData.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!parcelData.WriteInt32(eventType)) {
        CLOGE("Failed to write len");
        return;
    }
    if (!parcelData.WriteUint32(len)) {
        CLOGE("Failed to write len");
        return;
    }
    if (!parcelData.WriteBuffer(data, len)) {
        CLOGE("Failed to write buffer");
        return;
    }
    CLOGD("send request");
    if (Remote()->SendRequest(ON_REMOTE_CTRL_EVENT, parcelData, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when reporting remote control event");
    }
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
