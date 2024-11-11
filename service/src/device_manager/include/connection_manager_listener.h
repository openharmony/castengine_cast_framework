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
 * Description: connection manager session listener
 * Author: jiangfan
 * Create: 2023-11-08
 */

#ifndef CONNECTION_MANAGER_LISTENER_H
#define CONNECTION_MANAGER_LISTENER_H

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {

class IConnectionManagerListener {
public:
    IConnectionManagerListener() = default;
    virtual ~IConnectionManagerListener() = default;

    virtual int NotifySessionIsReady() = 0;
    virtual void ReportSessionCreate(int castSessionId) = 0;
    virtual void NotifyDeviceIsOffline(const std::string &deviceId) = 0;
    virtual bool NotifyRemoteDeviceIsReady(int castSessionId, const CastInnerRemoteDevice &device) = 0;
    virtual void GrabDevice(int32_t sessionId) = 0;
    virtual int32_t GetSessionProtocolType(int sessionId, ProtocolType &protocolType) = 0;
    virtual int32_t SetSessionProtocolType(int sessionId, ProtocolType protocolType) = 0;
};

class IConnectManagerSessionListener {
public:
    virtual ~IConnectManagerSessionListener() {}

    virtual void NotifyConnectStage(const std::string &deviceId, int result, int32_t reasonCode) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CONNECTION_MANAGER_LISTENER_H