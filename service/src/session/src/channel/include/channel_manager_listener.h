/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: the listener for channel listener
 * Author: sunhong
 * Create: 2022-01-19
 */
#ifndef CHANNEL_MANAGER_LISTENER_H
#define CHANNEL_MANAGER_LISTENER_H

#include <string>
#include "channel.h"
#include "channel_request.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class IChannelManagerListener {
public:
    IChannelManagerListener() = default;
    virtual ~IChannelManagerListener() = default;

    // Callback when channel is created successfully
    virtual void OnChannelCreated(std::shared_ptr<Channel> channel) {}
    // Callback when channel creation fails
    virtual void OnChannelOpenFailed(ChannelRequest &channelRequest, int errorCode) {}
    // Callback when removing the channel
    virtual void OnChannelRemoved(std::shared_ptr<Channel> channel) {}
    // Callback when screen casting channel is abnormal
    virtual void OnChannelError(std::shared_ptr<Channel> channel, int errorCode) {}
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CHANNEL_MANAGER_LISTENER_H
