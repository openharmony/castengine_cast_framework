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
 * Description: Connection listener
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CONNECTION_LISTENER_H
#define CONNECTION_LISTENER_H

#include <string>
#include "channel.h"
#include "channel_request.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class ConnectionListener {
public:
    virtual ~ConnectionListener() = default;

    // Callback when device connection is open
    // return The upper logic determines whether the channel is opened successfully
    virtual bool OnConnectionOpened(std::shared_ptr<Channel> channel)
    {
        return false;
    }

    // Callback when channel is closed
    virtual void OnConnectionClosed(std::shared_ptr<Channel> channel) {}
    // Callback when device channel connection fails
    virtual void OnConnectionConnectFailed(ChannelRequest &channelRequest, int errorCode) {}
    // Callback when device channel connection fails
    virtual void OnConnectionError(std::shared_ptr<Channel> channel, int errorCode) {}
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CONNECTION_LISTENER_H
