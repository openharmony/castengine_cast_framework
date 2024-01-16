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
 * Description: supply cast session implement proxy
 * Author: zhangge
 * Create: 2022-5-29
 */

#ifndef CAST_SESSION_IMPL_PROXY_H
#define CAST_SESSION_IMPL_PROXY_H

#include "cast_engine_common.h"
#include "i_cast_session_impl.h"
#include "iremote_proxy.h"
#include "oh_remote_control_event.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class CastSessionImplProxy : public IRemoteProxy<ICastSessionImpl> {
public:
    explicit CastSessionImplProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ICastSessionImpl>(impl) {}
    ~CastSessionImplProxy() override;

    int32_t RegisterListener(sptr<ICastSessionListenerImpl> listener) override;
    int32_t UnregisterListener() override;
    int32_t AddDevice(const CastRemoteDevice &remoteDevice) override;
    int32_t RemoveDevice(const std::string &deviceId) override;
    int32_t StartAuth(const AuthInfo &authInfo) override;
    int32_t GetSessionId(std::string &sessionId) override;
    int32_t GetDeviceState(const std::string &deviceId, DeviceState &deviceState) override;
    int32_t SetSessionProperty(const CastSessionProperty &property) override;
    int32_t CreateMirrorPlayer(sptr<IMirrorPlayerImpl> &mirrorPlayer) override;
    int32_t CreateStreamPlayer(sptr<IStreamPlayerIpc> &streamPlayer) override;
    int32_t Release() override;
    int32_t NotifyEvent(EventId eventId, std::string &jsonParam) override;
    int32_t SetCastMode(CastMode mode, std::string &jsonParam) override;

private:
    static inline BrokerDelegator<CastSessionImplProxy> delegator_;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_ENGINE_PROXY_H