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
 * Description: supply local data source cache implement class
 * Author: huangchanggui
 * Create: 2023-04-20
 */

#ifndef DATA_SOURCE_BUFFER_H
#define DATA_SOURCE_BUFFER_H

#include <mutex>
#include <condition_variable>
#include "cast_local_file_channel_client.h"
#include "cast_stream_common.h"
#include "i_data_listener.h"
#include "media_data_source.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class Cache final {
public:
    Cache(int64_t pos, int64_t capacity = MAX_BUFFER_SIZE);
    ~Cache();
    int64_t Read(uint8_t *data, uint32_t length, int64_t pos = 0);
    bool Write(const uint8_t *data, int64_t offset, int64_t length);
    bool IsMatch(int64_t pos);
    bool IsValid();
    int IsNeedReqData(int64_t &start, int64_t &end);
    void Reset(int64_t pos);
    int64_t GetUsedTime();

    static const int NO_NEED_REQ = 0;
    static const int NEED_REQ_IN_CURR_CACHE = 1;
    static const int NEED_REQ_IN_NEXT_CACHE = 2;

private:
    void UpdateUsedTimeLocked();
    void Init(int64_t pos);

    static const int PAUSE_REQUEST_WATER_LINE = 4 * 1024 * 1024; // 4MB
    static const int SINGLE_REQUEST_MAX_SIZE = 2 * 1024 * 1024;  // 2MB
    static const int FIRST_REQUEST_SIZE = 1 * 1024 * 1024;       // 1MB
    static const int MAX_BUFFER_SIZE = 5 * 1024 * 1024;          // 5MB
    static const int WAIT_DATA_TIME_OUT_MS = 100;
    static const int REQUEST_RETRY_TIME_INTERVAL_MS = 3000;

    std::condition_variable dataCond_;
    std::mutex dataMutex_;
    std::unique_ptr<uint8_t[]> buffer_;
    int64_t capacity_{ 0 };
    int64_t startPos_{ 0 };
    int64_t currPos_{ 0 };
    int64_t endPos_{ 0 };
    int64_t nextEndPos_{ 0 };
    int64_t lastUsedTime_{ 0 };
    int64_t lastRequestTime_{ 0 };
};

class LocalDataSource : public Media::IMediaDataSource,
    public IDataListener,
    public std::enable_shared_from_this<LocalDataSource> {
public:
    LocalDataSource(const std::string &fileId, int64_t fileLength,
        std::shared_ptr<CastLocalFileChannelClient> channelClient)
        : fileId_(fileId), fileLength_(fileLength), channelClient_(channelClient) {}
    virtual ~LocalDataSource();
    int32_t ReadAt(const std::shared_ptr<Media::AVSharedMemory> &mem, uint32_t length,
        int64_t pos = CAST_STREAM_INT_IGNORE) override;
    int32_t ReadAt(int64_t pos, uint32_t length, const std::shared_ptr<Media::AVSharedMemory> &mem) override;
    int32_t ReadAt(uint32_t length, const std::shared_ptr<Media::AVSharedMemory> &mem) override;
    int32_t GetSize(int64_t &size) override;
    bool OnBytesReceived(const std::string &fileId, const uint8_t *bytes, int64_t offset, int64_t length) override;
    int32_t ReadBuffer(uint8_t *data, uint32_t length, int64_t pos);
    bool Start();
    bool Stop();

private:
    std::shared_ptr<Cache> GetBestCache(int64_t pos);
    void SolveReqData(std::shared_ptr<Cache> cache, int64_t pos);

    static const int MAX_CACHE_COUNT = 4; // total cache: 4 * 5 = 20MB

    std::string fileId_;
    int64_t fileLength_{ 0 };

    std::mutex dataMutex_;
    std::vector<std::shared_ptr<Cache>> lruCache_;
    std::shared_ptr<CastLocalFileChannelClient> channelClient_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
