/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Channel for transmitting data
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CASTSESSION_CHANNEL_H
#define CASTSESSION_CHANNEL_H

#include <memory>
#include "channel_request.h"
#include "channel_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class Channel {
public:
    virtual ~Channel() = default;

    void SetRequest(const ChannelRequest &request)
    {
        channelRequest_ = request;
    }

    const ChannelRequest &GetRequest() const
    {
        return channelRequest_;
    }

    void SetListener(std::shared_ptr<IChannelListener> listener)
    {
        channelListener_ = listener;
    }

    std::shared_ptr<IChannelListener> GetListener()
    {
        return channelListener_;
    }

    virtual bool Send(const uint8_t *buffer, int length)
    {
        return false;
    }

private:
    ChannelRequest channelRequest_;
    std::shared_ptr<IChannelListener> channelListener_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CASTSESSION_CHANNEL_H
