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
