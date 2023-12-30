/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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
