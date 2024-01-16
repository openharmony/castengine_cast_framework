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