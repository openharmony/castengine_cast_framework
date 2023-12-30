/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply local file channel class.
 * Author: huangchanggui
 * Create: 2023-04-20
 */

#ifndef I_CAST_LOCAL_FILE_CHANNEL_H
#define I_CAST_LOCAL_FILE_CHANNEL_H

#include "channel.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {

class ICastLocalFileChannel {
public:
    virtual ~ICastLocalFileChannel() = default;
    virtual std::shared_ptr<IChannelListener> GetChannelListener() = 0;
    virtual void AddChannel(std::shared_ptr<Channel> channel) = 0;
    virtual void RemoveChannel(std::shared_ptr<Channel> channel) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif