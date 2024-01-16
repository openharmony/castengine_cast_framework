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
 * Description: implement the cast source connect
 * Author: zhangge
 * Create: 2022-08-23
 */

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <map>
#include <mutex>
#include <optional>
#include <string>

#include "cast_device_data_manager.h"
#include "cast_engine_common.h"
#include "cast_engine_log.h"
#include "cast_service_common.h"
#include "connection_manager_listener.h"
#include "device_manager.h"
#include "dm_device_info.h"
#include "device_manager_callback.h"
#include "session.h"
#include "json.hpp"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using OHOS::DistributedHardware::BindTargetCallback;
using OHOS::DistributedHardware::UnbindTargetCallback;
using OHOS::DistributedHardware::DeviceStateCallback;
using OHOS::DistributedHardware::DmDeviceInfo;
using OHOS::DistributedHardware::PeerTargetId;
using nlohmann::json;

class CastBindTargetCallback : public BindTargetCallback {
public:
    void OnBindResult(const PeerTargetId &targetId, int32_t result, int32_t status, std::string content) override;
private:
    void HandleBindAction(const PeerTargetId &targetId, int action, const json &authInfo);
    void HandleConnectDeviceAction(const PeerTargetId &targetId, const json &authInfo);
    bool GetSessionKey(const json &authInfo, uint8_t *sessionkey);
    void HandleQueryIpAction(const PeerTargetId &targetId, const json &authInfo);
    static const std::map<int32_t, EventCode> EVENT_CODE_MAP;
};

class CastUnBindTargetCallback : public UnbindTargetCallback {
public:
    void OnUnbindResult(const PeerTargetId &targetId, int32_t result, std::string content) override;
};

class IConnectionManagerListener {
public:
    IConnectionManagerListener() = default;
    virtual ~IConnectionManagerListener() = default;

    virtual int NotifySessionIsReady() = 0;
    virtual void NotifyDeviceIsOffline(const std::string &deviceId) = 0;
    virtual bool NotifyRemoteDeviceIsReady(int castSessionId, const CastInnerRemoteDevice &device) = 0;
    virtual void OnEvent(const std::string &deviceId, EventCode currentEventCode) = 0;
    virtual void GrabDevice(int32_t sessionId) = 0;
    virtual int32_t GetSessionProtocolType(int sessionId, ProtocolType &protocolType) = 0;
    virtual int32_t SetSessionProtocolType(int sessionId, ProtocolType protocolType) = 0;
};

class ConnectionManager {
public:
    static ConnectionManager &GetInstance();

    void Init(std::shared_ptr<IConnectionManagerListener> listener);
    void Deinit();

    bool IsDeviceTrusted(const std::string &deviceId, std::string &networkId);
    DmDeviceInfo GetDmDeviceInfo(const std::string &deviceId);
    bool EnableDiscoverable();
    bool DisableDiscoverable();
    void GrabDevice();

    bool OpenConsultSession(const std::string &deviceId);
    void OnConsultDataReceived(int transportId, const void *data, unsigned int dataLen);
    void OnConsultSessionOpened(int transportId, bool isSource);

    bool ConnectDevice(const CastInnerRemoteDevice &dev);
    void DisconnectDevice(const std::string &deviceId);

    bool UpdateDeviceState(const std::string &deviceId, RemoteDeviceState state);

    int GetProtocolType() const;
    void SetProtocolType(int protocols);

    std::unique_ptr<CastLocalDevice> GetLocalDeviceInfo();
    void NotifySessionIsReady(int transportId);
    void NotifyDeviceIsOffline(const std::string &deviceId);
    bool NotifySessionEvent(const std::string &deviceId, int result);
    void ReportErrorByListener(const std::string &deviceId, EventCode currentEventCode);
    void UpdateGrabState(bool changeState, int32_t sessionId);
    void SetSessionListener(std::shared_ptr<IConnectManagerSessionListener> listener);
    int32_t GetSessionProtocolType(int sessionId, ProtocolType &protocolType);
    int32_t SetSessionProtocolType(int sessionId, ProtocolType protocolType);
    void SetRTSPPort(int port);
    bool IsSingle(const CastInnerRemoteDevice &device);
    void SendConsultInfo(const std::string &deviceId, int port);

    std::map<std::string, bool> isBindTargetMap_;

private:
    bool BindTarget(const CastInnerRemoteDevice &dev);
    bool BuildBindParam(const CastInnerRemoteDevice &device, std::map<std::string, std::string> &bindParam);
    std::string GetAuthVersion(const CastInnerRemoteDevice &device);
    void SendConsultData(const CastInnerRemoteDevice &device, int port);
    bool QueryP2PIp(const CastInnerRemoteDevice &device);
    bool IsHuaweiDevice(const CastInnerRemoteDevice &device);
    bool IsThirdDevice(const CastInnerRemoteDevice &device);

    std::string GetConsultationData(const CastInnerRemoteDevice &device, int port, json &body);
    void EncryptPort(int port, const uint8_t *sessionKey, json &body);
    std::string convLatin1ToUTF8(std::string &latin1);
    void EncryptIp(const std::string &ip, const std::string &key, const uint8_t *sessionKey, json &body);
    std::unique_ptr<uint8_t[]> intToByteArray(int32_t num);

    void DestroyConsulationSession(const std::string &deviceId);
    int GetCastSessionId(int transportId);
    std::unique_ptr<CastInnerRemoteDevice> GetRemoteFromJsonData(const std::string &Data);

    void SetListener(std::shared_ptr<IConnectionManagerListener> listener);
    bool HasListener();
    void ResetListener();
    int GetRTSPPort();

    std::mutex mutex_;
    int protocolType_ = 0;
    std::shared_ptr<IConnectionManagerListener> listener_;
    std::shared_ptr<IConnectManagerSessionListener> sessionListener_;
    std::map<int, int> transIdToCastSessionIdMap_;
    bool isDiscoverable_{ false };
    DeviceGrabState grabState_{ DeviceGrabState::NO_GRAB };
    int32_t sessionId_{ -1 };
    int rtspPort_{ INVALID_PORT };
};

class CastDeviceStateCallback : public DeviceStateCallback {
public:
    void OnDeviceOnline(const DmDeviceInfo &deviceInfo) override;
    void OnDeviceOffline(const DmDeviceInfo &deviceInfo) override;
    void OnDeviceChanged(const DmDeviceInfo &deviceInfo) override;
    void OnDeviceReady(const DmDeviceInfo &deviceInfo) override;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CONNECTION_MANAGER_H