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
 * Description: connection class
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include "channel_request.h"
#include "connection_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class Connection {
public:
    virtual ~Connection() {};

    virtual void SetConnectionListener(std::shared_ptr<ConnectionListener> listener)
    {
        listener_ = listener;
    }

    // init request when openConnection or startListen
    virtual void StashRequest(const ChannelRequest &request)
    {
        channelRequest_ = request;
    }

    // Open the connection asynchronously.
    virtual int StartConnection(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener)
    {
        return -1;
    }

    /*
     * Start listening for device channel connections.
     * When the channel type is VTP or TCP, the return value represents the port number, and a negative value
     * represents failure.
     * When the channel type is softbus, the return value of 0 indicates successful registration and listening,
     * and a negative value represents failure.
     *
     * @param request channel unique ID
     * @return Results of starting listening
     */
    virtual int StartListen(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener)
    {
        return -1;
    }

    // Close connection and all channels
    virtual void CloseConnection() {};

protected:
    ChannelRequest channelRequest_;
    std::shared_ptr<ConnectionListener> listener_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS


#endif // CONNECTION_H
