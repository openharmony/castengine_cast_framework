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
 * Description: channel manager
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <map>
#include <mutex>
#include <string>
#include <memory>
#include "channel_request.h"
#include "connection.h"
#include "channel_listener.h"
#include "channel_manager_listener.h"
#include "channel_info.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class ChannelManager {
public:
    ChannelManager(const int sessionIndex, std::shared_ptr<IChannelManagerListener> channelManagerListener);
    ~ChannelManager();

    int CreateChannel(ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener);
    int CreateChannel(ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener,
        ChannelFileSchema &fileSchema);
    bool IsAllChannelOpened() const;
    bool DestroyChannel(const Channel &channel);
    bool DestroyChannel(ModuleType moduleType);
    void DestroyAllChannels();

private:
    class ConnectionListenerInner : public ConnectionListener {
    public:
        explicit ConnectionListenerInner(std::shared_ptr<IChannelManagerListener> listener)
            : channelManagerListenerInner_(listener) {};

        ~ConnectionListenerInner() {};

    private:
        std::shared_ptr<IChannelManagerListener> channelManagerListenerInner_;

        bool OnConnectionOpened(std::shared_ptr<Channel> channel) override
        {
            channelManagerListenerInner_->OnChannelCreated(channel);
            return true;
        }

        void OnConnectionConnectFailed(ChannelRequest &channelRequest, int errorCode) override
        {
            channelManagerListenerInner_->OnChannelOpenFailed(channelRequest, errorCode);
        }

        void OnConnectionClosed(std::shared_ptr<Channel> channel) override
        {
            channelManagerListenerInner_->OnChannelRemoved(channel);
        }

        void OnConnectionError(std::shared_ptr<Channel> channel, int errorCode) override
        {
            channelManagerListenerInner_->OnChannelError(channel, errorCode);
        }
    };

    bool IsRequestValid(const ChannelRequest &request) const;
    std::shared_ptr<Connection> GetConnection(ChannelLinkType linkType);

    static const int RET_ERR = -1;
    int sessionId_{ -1 };
    int sessionIndex_{ -1 };
    int connectionNum_{ 0 };
    std::map<ChannelRequest, std::shared_ptr<Connection>> connectionMap_;
    std::mutex connectionMapMtx_;
    std::shared_ptr<IChannelManagerListener> channelManagerListener_;
    std::shared_ptr<ConnectionListener> connectionListener_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS


#endif // CHANNEL_MANAGER_H
