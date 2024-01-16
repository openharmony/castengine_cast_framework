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
 * Description: supply cast session interface.
 * Author: zhangge
 * Create: 2022-06-15
 */

#ifndef I_CAST_SESSION_IMPL_H
#define I_CAST_SESSION_IMPL_H

#include <string>

#include "cast_engine_common.h"
#include "cast_service_common.h"
#include "cast_engine_errors.h"
#include "i_cast_session.h"
#include "i_cast_session_listener_impl.h"
#include "iremote_broker.h"
#include "oh_remote_control_event.h"
#include "surface_utils.h"
#include "i_stream_player_ipc.h"
#include "i_mirror_player_impl.h"

namespace OHOS {
namespace CastEngine {
class ICastSessionImpl : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.CastEngine.ICastSessionImpl");

    ICastSessionImpl() = default;
    ICastSessionImpl(const ICastSessionImpl &) = delete;
    ICastSessionImpl &operator=(const ICastSessionImpl &) = delete;
    ICastSessionImpl(ICastSessionImpl &&) = delete;
    ICastSessionImpl &operator=(ICastSessionImpl &&) = delete;
    ~ICastSessionImpl() override = default;

    virtual int32_t RegisterListener(sptr<ICastSessionListenerImpl> listener) = 0;
    virtual int32_t UnregisterListener() = 0;
    virtual int32_t AddDevice(const CastRemoteDevice &remoteDevice) = 0;
    virtual int32_t RemoveDevice(const std::string &deviceId) = 0;
    virtual int32_t StartAuth(const AuthInfo &authInfo) = 0;
    virtual int32_t GetSessionId(std::string &sessionId) = 0;
    virtual int32_t GetDeviceState(const std::string &deviceId, DeviceState &deviceState) = 0;
    virtual int32_t SetSessionProperty(const CastSessionProperty &property) = 0;
    virtual int32_t CreateMirrorPlayer(sptr<IMirrorPlayerImpl> &mirrorPlayer) = 0;
    virtual int32_t CreateStreamPlayer(sptr<IStreamPlayerIpc> &streamPlayer) = 0;
    virtual int32_t Release() = 0;
    virtual int32_t NotifyEvent(EventId eventId, std::string &jsonParam) = 0;
    virtual int32_t SetCastMode(CastMode mode, std::string &jsonParam) = 0;
    virtual void OnSessionEvent(const std::string &deviceId, const EventCode eventCode) {}
    virtual bool AddDevice(const CastInnerRemoteDevice &remoteDevice)
    {
        return false;
    }
    virtual bool ReleaseSessionResources(pid_t pid)
    {
        return false;
    }
    virtual void Stop() {}
    virtual int32_t GetSessionProtocolType(ProtocolType &protocolType)
    {
        return CAST_ENGINE_ERROR;
    }
    virtual void SetSessionProtocolType(ProtocolType protocolType) {}

protected:
    enum {
        REGISTER_LISTENER = 1,
        UNREGISTER_LISTENER,
        ADD_DEVICE,
        REMOVE_DEVICE,
        START_AUTH,
        GET_SESSION_ID,
        GET_DEVICE_STATE,
        SET_SESSION_PROPERTY,
        CREAT_MIRROR_PLAYER,
        CREAT_STREAM_PLAYER,
        RELEASE,
        NOTIFY_EVENT,
        SET_CAST_MODE,
    };
};
} // namespace CastEngine
} // namespace OHOS

#endif
