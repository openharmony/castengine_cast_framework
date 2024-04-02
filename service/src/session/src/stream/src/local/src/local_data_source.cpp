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

#include "local_data_source.h"
#include <cinttypes>
#include <securec.h>
#include "cast_engine_log.h"
#include "media_errors.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-LocalDataSource");

LocalDataSource::~LocalDataSource()
{
    CLOGD("destructor in");
}

bool LocalDataSource::Start()
{
    CLOGD("in");
    if (!channelClient_) {
        return false;
    }
    channelClient_->AddDataListener(shared_from_this());
    return true;
}

bool LocalDataSource::Stop()
{
    CLOGD("in");
    if (!channelClient_) {
        return false;
    }
    channelClient_->RemoveDataListener(shared_from_this());
    return true;
}

std::shared_ptr<Cache> LocalDataSource::GetBestCache(int64_t pos)
{
    std::unique_lock<std::mutex> lock(dataMutex_);
    // get hit cache
    for (auto iter = lruCache_.begin(); iter != lruCache_.end(); iter++) {
        if ((*iter)->IsMatch(pos)) {
            return *iter;
        }
    }
    std::shared_ptr<Cache> cache;
    // cache limit not reached, create a new cache
    if (lruCache_.size() < MAX_CACHE_COUNT) {
        cache = std::make_shared<Cache>(pos);
        if (!cache || !cache->IsValid()) {
            CLOGE("malloc failed");
            return nullptr;
        }
        lruCache_.push_back(cache);
        return cache;
    }
    // cache limit reached, get the oldest used cache for reuse
    for (auto iter = lruCache_.begin(); iter != lruCache_.end(); iter++) {
        if (!cache || cache->GetUsedTime() > (*iter)->GetUsedTime()) {
            cache = *iter;
        }
    }
    CLOGD("GetBestCache:reset:%{public}" PRId64, pos);
    cache->Reset(pos);
    return cache;
}

void LocalDataSource::SolveReqData(std::shared_ptr<Cache> cache, int64_t pos)
{
    int64_t start;
    int64_t end;
    int isNeedReq = cache->IsNeedReqData(start, end);
    if (isNeedReq == Cache::NO_NEED_REQ) {
        return;
    }
    if (isNeedReq == Cache::NEED_REQ_IN_NEXT_CACHE) {
        cache = GetBestCache(start);
        if (!cache) {
            return;
        }
        if (cache->IsNeedReqData(start, end) != Cache::NEED_REQ_IN_CURR_CACHE) {
            return;
        }
    }
    CLOGD("request data, start:%{public}" PRId64 " end:%{public}" PRId64 " pos:%{public}" PRId64, start, end, pos);
    channelClient_->RequestByteData(start, end, fileId_);
}

int32_t LocalDataSource::ReadAt(const std::shared_ptr<Media::AVSharedMemory> &mem, uint32_t length, int64_t pos)
{
    return ReadBuffer(mem->GetBase(), length, pos);
}

int32_t LocalDataSource::ReadAt(int64_t pos, uint32_t length, const std::shared_ptr<Media::AVSharedMemory> &mem)
{
    return ReadAt(mem, length, pos);
}

int32_t LocalDataSource::ReadAt(uint32_t length, const std::shared_ptr<Media::AVSharedMemory> &mem)
{
    return ReadAt(mem, length);
}

int32_t LocalDataSource::ReadBuffer(uint8_t *data, uint32_t length, int64_t pos)
{
    if (pos >= fileLength_) {
        CLOGE("ReadAt EOF, pos:%{public}" PRId64 " fileLength_:%{public}" PRId64, pos, fileLength_);
        return Media::SOURCE_ERROR_EOF;
    }
    if (pos < 0) {
        CLOGE("don't support noseek mode, pos:%{public}" PRId64, pos);
        return Media::SOURCE_ERROR_IO;
    }

    auto cache = GetBestCache(pos);
    if (!cache) {
        return Media::SOURCE_ERROR_IO;
    }
    // The cache may be a new that has no data, need req data before reading
    SolveReqData(cache, pos);
    int32_t readBytes = static_cast<int32_t>(cache->Read(data, length, pos));
    // cache data may be not enoungh after reading, req data in advance for next reading
    SolveReqData(cache, pos);
    return readBytes;
}

int32_t LocalDataSource::GetSize(int64_t &size)
{
    if (fileLength_ > 0) {
        size = fileLength_;
        return Media::MSERR_OK;
    }
    // requestLength
    fileLength_ = channelClient_->RequestFileLength(fileId_);
    size = fileLength_;
    return Media::MSERR_OK;
}

bool LocalDataSource::OnBytesReceived(const std::string &fileId, const uint8_t *bytes, int64_t offset, int64_t length)
{
    if (fileId.compare(fileId_) != 0) {
        CLOGE("fileId:%{public}s is not match fileId_:%{public}s", fileId.c_str(), fileId_.c_str());
        return false;
    }
    std::unique_lock<std::mutex> lock(dataMutex_);
    for (auto iter = lruCache_.begin(); iter != lruCache_.end(); iter++) {
        if ((*iter)->Write(bytes, offset, length)) {
            return true;
        }
    }
    CLOGE("OnBytesReceived out, not process");
    return false;
}

Cache::Cache(int64_t pos, int64_t capacity)
{
    CLOGD("Cache in");
    if (capacity > MAX_BUFFER_SIZE || capacity <= 0) {
        capacity = MAX_BUFFER_SIZE;
    }
    buffer_ = std::make_unique<uint8_t[]>(capacity);
    if (!buffer_) {
        CLOGE("buffer malloc failed");
        return;
    }
    capacity_ = capacity;
    Init(pos);
}

Cache::~Cache()
{
    CLOGD("~Cache in");
    buffer_.reset();
    capacity_ = 0;
    startPos_ = 0;
    currPos_ = 0;
    endPos_ = 0;
    nextEndPos_ = 0;
}

void Cache::Reset(int64_t pos)
{
    Init(pos);
}

bool Cache::IsMatch(int64_t pos)
{
    std::unique_lock<std::mutex> lock(dataMutex_);
    if (pos < startPos_ || pos >= nextEndPos_) {
        return false;
    }
    UpdateUsedTimeLocked();
    return true;
}

bool Cache::IsValid()
{
    return (buffer_ != nullptr);
}

int Cache::IsNeedReqData(int64_t &start, int64_t &end)
{
    std::unique_lock<std::mutex> lock(dataMutex_);
    int64_t cachedBytes = endPos_ - startPos_;
    // 1.already requested  2.curr cached buffer is enough
    if (nextEndPos_ != endPos_ || (endPos_ - currPos_) >= PAUSE_REQUEST_WATER_LINE) {
        return NO_NEED_REQ;
    }
    // buffer cached complete, < should not actually happen, it is just for protection
    if (capacity_ <= cachedBytes) {
        start = endPos_;
        return NEED_REQ_IN_NEXT_CACHE;
    }
    int64_t length =
        (capacity_ - cachedBytes) > SINGLE_REQUEST_MAX_SIZE ? SINGLE_REQUEST_MAX_SIZE : (capacity_ - cachedBytes);
    if (startPos_ == endPos_) {
        length = FIRST_REQUEST_SIZE;
    }
    start = endPos_;
    end = endPos_ + length;
    nextEndPos_ = end;
    std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now().time_since_epoch();
    lastRequestTime_ = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    return NEED_REQ_IN_CURR_CACHE;
}

int64_t Cache::GetUsedTime()
{
    std::unique_lock<std::mutex> lock(dataMutex_);
    return lastUsedTime_;
}

void Cache::Init(int64_t pos)
{
    std::unique_lock<std::mutex> lock(dataMutex_);
    startPos_ = pos;
    currPos_ = pos;
    endPos_ = pos;
    nextEndPos_ = pos;
    UpdateUsedTimeLocked();
}

void Cache::UpdateUsedTimeLocked()
{
    std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now().time_since_epoch();
    lastUsedTime_ = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
}

int64_t Cache::Read(uint8_t *data, uint32_t length, int64_t pos)
{
    std::unique_lock<std::mutex> lock(dataMutex_);
    if (data == nullptr || buffer_ == nullptr || length == 0) {
        CLOGE("data/buffer_ is null or length is 0");
        return 0;
    }
    if (pos >= nextEndPos_ || pos < startPos_) {
        CLOGE("has no expected data pos:%{public}" PRId64 " startPos_:%{public}" PRId64 " nextEndPos_:%{public}" PRId64,
            pos, startPos_, nextEndPos_);
        return 0;
    }
    UpdateUsedTimeLocked();
    if (pos >= endPos_) {
        // data req has send to server, wait for server rsp, timeout 100ms
        dataCond_.wait_for(lock, std::chrono::milliseconds(WAIT_DATA_TIME_OUT_MS));
        if (pos >= endPos_) {
            CLOGE("wait for data from server timeout pos:%{public}" PRId64 " endPos_:%{public}" PRId64, pos, endPos_);
            std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now().time_since_epoch();
            int64_t now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
            if (now > lastRequestTime_ && (now - lastRequestTime_) >= REQUEST_RETRY_TIME_INTERVAL_MS) {
                nextEndPos_ = endPos_; // need Re-request data
            }
            return 0;
        }
    }
    int64_t readBytes = length > (endPos_ - pos) ? (endPos_ - pos) : length;
    errno_t ret = memcpy_s(data, readBytes, buffer_.get() + (pos - startPos_), readBytes);
    if (ret != EOK) {
        CLOGE("memcpy failed ret:%{public}d readBytes:%{public}" PRId64 " pos:%{public}" PRId64
            " startPos_:%{public}" PRId64,
            ret, readBytes, pos, startPos_);
        return 0;
    }
    currPos_ = pos;
    dataCond_.notify_all();
    return readBytes;
}

bool Cache::Write(const uint8_t *data, int64_t offset, int64_t length)
{
    std::unique_lock<std::mutex> lock(dataMutex_);
    if (offset != endPos_) {
        return false;
    }
    int64_t remainLen = capacity_ - (endPos_ - startPos_);
    if (remainLen <= 0 || length <= 0 || buffer_ == nullptr || data == nullptr) {
        CLOGE("remainLen/length is 0 or buffer/data is null");
        return false;
    }
    int64_t writeBytes = length > remainLen ? remainLen : length;
    errno_t ret = memcpy_s(buffer_.get() + endPos_ - startPos_, writeBytes, data, writeBytes);
    if (ret != EOK) {
        CLOGE("memcpy failed ret = %{public}d, writeBytes:%{public}" PRId64 " startPos_:%{public}" PRId64
            " endPos_:%{public}" PRId64,
            ret, writeBytes, startPos_, endPos_);
        nextEndPos_ = endPos_; // need Re-request
        return false;
    }
    endPos_ += writeBytes;
    nextEndPos_ = endPos_;
    CLOGD("writeBytes:%{public}" PRId64 " length:%{public}" PRId64 " startPos_:%{public}" PRId64
        " endPos_:%{public}" PRId64,
        writeBytes, length, startPos_, endPos_);
    dataCond_.notify_all();
    return true;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
