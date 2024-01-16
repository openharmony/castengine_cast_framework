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
 * Description: Cast local file channel client operation
 * Author: liaoxin
 * Create: 2023-4-20
 */

#ifndef CAST_LOCAL_FILE_CHANNEL_CLIENT_H
#define CAST_LOCAL_FILE_CHANNEL_CLIENT_H

#include <string>
#include <map>
#include <list>
#include <condition_variable>

#include "cast_engine_common.h"
#include "channel_listener.h"
#include "channel.h"
#include "i_cast_local_file_channel.h"
#include "i_cast_stream_manager_server.h"
#include "i_data_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastLocalFileChannelClient : public ICastLocalFileChannel,
    public IChannelListener,
    public std::enable_shared_from_this<CastLocalFileChannelClient> {
public:
    CastLocalFileChannelClient(std::shared_ptr<ICastStreamManagerServer> callback);
    ~CastLocalFileChannelClient();

    std::shared_ptr<IChannelListener> GetChannelListener() override;
    void AddChannel(std::shared_ptr<Channel> channel) override;
    void RemoveChannel(std::shared_ptr<Channel> channel) override;

    void OnDataReceived(const uint8_t *buffer, unsigned int length, long timeCost) override;

    void RequestByteData(int64_t start, int64_t end, const std::string &fileId);
    int64_t RequestFileLength(const std::string &fileId);

    void NotifyCreateChannel();
    void WaitCreateChannel();
    void AddDataListener(std::shared_ptr<IDataListener> dataListener);
    void RemoveDataListener(std::shared_ptr<IDataListener> dataListener);

private:
    std::weak_ptr<ICastStreamManagerServer> callback_;
    std::shared_ptr<Channel> channel_;
    std::list<std::shared_ptr<IDataListener>> dataListeners_;
    std::condition_variable cond_;
    std::mutex chLock_;
    std::mutex listenerLock_;

    bool ProcessServerResponse(const uint8_t *buffer, unsigned int length, std::map<std::string, std::string> &response,
        size_t &dataOffset);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_LOCAL_FILE_CHANNEL_CLIENT_H
