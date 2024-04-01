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
 * Description: Cast local file channel client impl
 * Author: liaoxin
 * Create: 2023-04-20
 */

#include "cast_local_file_channel_client.h"

#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <mutex>
#include <cinttypes>

#include "cast_engine_log.h"
#include "cast_local_file_channel_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Localfile-Client");

static const int CREATE_CHANNEL_TIMEOUT = 100 * 1000;

CastLocalFileChannelClient::CastLocalFileChannelClient(std::shared_ptr<ICastStreamManagerServer> callback)
{
    CLOGD("in");
    callback_ = callback;
}

CastLocalFileChannelClient::~CastLocalFileChannelClient()
{
    CLOGD("in");
}

std::shared_ptr<IChannelListener> CastLocalFileChannelClient::GetChannelListener()
{
    CLOGD("in");
    return shared_from_this();
}

void CastLocalFileChannelClient::NotifyCreateChannel()
{
    CLOGI("in");

    {
        std::unique_lock<std::mutex> lock(chLock_);
        if (channel_) {
            CLOGE("channel is exist");
            return;
        }
    }

    auto targetCallback = callback_.lock();
    if (!targetCallback) {
        CLOGE("callback is null");
        return;
    }
    targetCallback->NotifyPeerCreateChannel();
    CLOGI("out");
}

void CastLocalFileChannelClient::WaitCreateChannel()
{
    CLOGI("in");

    std::unique_lock<std::mutex> lock(chLock_);
    if (channel_) {
        CLOGI("channel is already created");
        return;
    }
    auto start = std::chrono::steady_clock::now();
    auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(start - start);
    while (!channel_ && cost.count() < CREATE_CHANNEL_TIMEOUT) {
        cond_.wait_for(lock, std::chrono::milliseconds(CREATE_CHANNEL_TIMEOUT - cost.count()));
        cost = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    }
    if (!channel_) {
        CLOGE("channel create timeout");
    }
    CLOGI("out");
}

void CastLocalFileChannelClient::AddChannel(std::shared_ptr<Channel> channel)
{
    CLOGI("in");
    std::unique_lock<std::mutex> lock(chLock_);
    channel_ = channel;
    cond_.notify_all();
}

void CastLocalFileChannelClient::RemoveChannel(std::shared_ptr<Channel> channel)
{
    CLOGI("in");
    std::unique_lock<std::mutex> lock(chLock_);
    channel_ = nullptr;
}

void CastLocalFileChannelClient::RequestByteData(int64_t start, int64_t end, const std::string &fileId)
{
    std::shared_ptr<Channel> channel;
    {
        std::unique_lock<std::mutex> lock(chLock_);
        if (!channel_) {
            CLOGE("channel is not created.");
            return;
        }
        channel = channel_;
    }
    // Make http request header
    std::string req("GET ");
    req.append(fileId);
    req.append(" HTTP/1.1\r\nRange: bytes=" + std::to_string(start) + "-" + std::to_string(end) + "\r\n\r\n");

    CLOGD("request data: %s len %{public}" PRId64 "-%{public}" PRId64, fileId.c_str(), start, end);

    channel->Send(reinterpret_cast<uint8_t *>(const_cast<char *>(req.data())), req.size());
}

int64_t CastLocalFileChannelClient::RequestFileLength(const std::string &fileId)
{
    return 0;
}

void CastLocalFileChannelClient::AddDataListener(std::shared_ptr<IDataListener> dataListener)
{
    if (!dataListener) {
        CLOGE("listener is null");
        return;
    }
    std::lock_guard<std::mutex> lock(listenerLock_);
    dataListeners_.push_back(dataListener);
}

void CastLocalFileChannelClient::RemoveDataListener(std::shared_ptr<IDataListener> dataListener)
{
    if (!dataListener) {
        CLOGE("listener is null");
        return;
    }
    std::lock_guard<std::mutex> lock(listenerLock_);
    dataListeners_.remove(dataListener);
}

bool CastLocalFileChannelClient::ProcessServerResponse(const uint8_t *buffer, unsigned int length,
    std::map<std::string, std::string> &response, size_t &dataOffset)
{
    bool ret = ParseHttpResponse(buffer, length, response, dataOffset);
    if (!ret) {
        CLOGE("http response header parse error");
        return false;
    }

    if (response[HTTP_RSP_CODE].compare("200") != 0) {
        CLOGE("http status code %{public}s", response[HTTP_RSP_CODE].c_str());
        return false;
    }

    return true;
}

void CastLocalFileChannelClient::OnDataReceived(const uint8_t *buffer, unsigned int length, long timeCost)
{
    CLOGD("buffer length %{public}u", length);

    // Parse http header
    size_t dataOffset = 0;
    std::map<std::string, std::string> response;
    bool ret = ProcessServerResponse(buffer, length, response, dataOffset);
    if (!ret) {
        CLOGE("buffer length %{public}u", length);
        return;
    }

    int64_t contentLen = 0;
    int64_t start = 0;
    ret = ParseStringToInt64(response[HTTP_RSP_CONTENT_LENGTH], contentLen);
    ret = ret && ParseStringToInt64(response[HTTP_RSP_CONTENT_RANGE_START], start);
    if (!ret || contentLen <= 0 || contentLen > static_cast<int64_t>(length - dataOffset) || start <= INVALID_END_POS) {
        CLOGE("Invalid response, len:%{public}" PRId64 ", start: %{public}" PRId64, contentLen, start);
        return;
    }

    const std::string &fileName = response[HTTP_RSP_CONTENT_DISPOSITION];
    CLOGD("headerLen %{public}zu URL %s start %{public}" PRId64 " content %{public}" PRId64, dataOffset,
        fileName.c_str(), start, contentLen);

    // Notify data to listener
    std::lock_guard<std::mutex> lock(listenerLock_);
    for (auto it = dataListeners_.begin(); it != dataListeners_.end(); it++) {
        bool ret = (*it)->OnBytesReceived(fileName, buffer + dataOffset, start, contentLen);
        if (ret) {
            CLOGD("data uploaded");
            break;
        }
    }
    CLOGD("End");
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
