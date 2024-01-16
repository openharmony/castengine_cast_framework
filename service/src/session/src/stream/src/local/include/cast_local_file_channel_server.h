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
 * Description: Cast local file channel server operation
 * Author: liaoxin
 * Create: 2023-4-20
 */

#ifndef CAST_LOCAL_FILE_CHANNEL_SERVER_H
#define CAST_LOCAL_FILE_CHANNEL_SERVER_H

#include <string>
#include <map>
#include <mutex>

#include "cast_engine_common.h"
#include "channel_listener.h"
#include "channel.h"
#include "i_cast_local_file_channel.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastLocalFileChannelServer : public ICastLocalFileChannel,
    public IChannelListener,
    public std::enable_shared_from_this<CastLocalFileChannelServer> {
public:
    CastLocalFileChannelServer();
    ~CastLocalFileChannelServer();

    bool AddLocalFileInfo(MediaInfo &mediaInfo);
    void ClearAllLocalFileInfo();

    std::shared_ptr<IChannelListener> GetChannelListener() override;

    void AddChannel(std::shared_ptr<Channel> channel) override;
    void RemoveChannel(std::shared_ptr<Channel> channel) override;

    void OnDataReceived(const uint8_t *buffer, unsigned int length, long timeCost) override;

private:
    struct LocalFileInfo {
        std::string encodedUrl;
        std::string localFile;
        int fd = INVALID_VALUE;
        int64_t fileLen = 0;
    };

    std::map<std::string, struct LocalFileInfo> fileMap_;
    std::shared_ptr<Channel> channel_;
    std::mutex chLock_;
    std::mutex mapLock_;

    int64_t GetFileLengthByFd(int fd);
    int64_t GetFileLengthByFileName(const std::string &file);
    int FindLocalFd(const std::string &encodedUri);
    struct LocalFileInfo FindLocalFileInfo(const std::string &encodedUri);
    int64_t FindFileLengthByUri(const std::string &encodeUri);
    void AddFileInfoToMap(const std::string encodedId, const struct LocalFileInfo &data);
    int ReadFileData(const struct LocalFileInfo &data, int64_t start, int64_t sendLen, uint8_t *ptr);
    void ProcessRequestData(const uint8_t *buffer, int length);
    void ResponseFileLengthRequest(const std::string &uri, int64_t fileLen);
    void ResponseFileDataRequest(const std::string &uri, int64_t fileLen, int64_t start, int64_t end);
    void ResponseFileRequest(const std::string &uri, int64_t start, int64_t end);
    void SendData(const uint8_t *buffer, int length);
    void ClearAllMapInfo();
    int ReadFileDataByFd(int fd, int64_t start, int sendLen, uint8_t *buffer);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_LOCAL_FILE_CHANNEL_SERVER_H
