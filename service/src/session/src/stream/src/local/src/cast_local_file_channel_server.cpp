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
 * Description: Cast local file channel server impl
 * Author: liaoxin
 * Create: 2023-04-20
 */

#include "cast_local_file_channel_server.h"

#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <algorithm>
#include <mutex>
#include <cinttypes>

#include <securec.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "cast_engine_log.h"
#include "cast_local_file_channel_common.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Localfile-Server");

// DSoftbus, sendByte limit max data 2M at one time. Reserve 1KB for http header
static const int64_t MAX_READ_LEN = 2 * 1024 * 1024 - 1024;

CastLocalFileChannelServer::CastLocalFileChannelServer()
{
    CLOGD("in");
}

CastLocalFileChannelServer::~CastLocalFileChannelServer()
{
    CLOGD("in");

    ClearAllMapInfo();
}

void CastLocalFileChannelServer::ClearAllMapInfo()
{
    std::lock_guard<std::mutex> lock(mapLock_);
    for (auto it = fileMap_.begin(); it != fileMap_.end(); it++) {
        CLOGD("close file: %s", (it->second).encodedUrl.c_str());
        // Close opened fd
        if ((it->second).fd != INVALID_VALUE) {
            close((it->second).fd);
            (it->second).fd = INVALID_VALUE;
        }
    }
    fileMap_.clear();
}

std::shared_ptr<IChannelListener> CastLocalFileChannelServer::GetChannelListener()
{
    return shared_from_this();
}

void CastLocalFileChannelServer::AddChannel(std::shared_ptr<Channel> channel)
{
    CLOGI("in");
    std::unique_lock<std::mutex> lock(chLock_);
    channel_ = channel;
    CLOGI("out");
}

void CastLocalFileChannelServer::RemoveChannel(std::shared_ptr<Channel> channel)
{
    CLOGI("in");
    std::unique_lock<std::mutex> lock(chLock_);
    channel_ = nullptr;
    CLOGI("out");
}

void CastLocalFileChannelServer::AddFileInfoToMap(const std::string encodedId, const struct LocalFileInfo &data)
{
    std::lock_guard<std::mutex> lock(mapLock_);
    fileMap_.insert({ encodedId, data });
}

bool CastLocalFileChannelServer::AddLocalFileInfo(MediaInfo &mediaInfo)
{
    CLOGD("id %{public}s url %s", mediaInfo.mediaId.c_str(), mediaInfo.mediaUrl.c_str());

    // Only process local file or local fd
    if (!IsLocalUrl(mediaInfo.mediaUrl)) {
        CLOGD("Skip remote url");
        return true;
    }

    CLOGD("local url");

    // Encode url. Normal base64 is ok.
    std::string encodedFileId;
    if (!Utils::Base64Encode(mediaInfo.mediaUrl, encodedFileId)) {
        CLOGE("base64 encode fail");
        return false;
    }

    CLOGD("encoded url %s", encodedFileId.c_str());

    LocalFileInfo data;
    data.encodedUrl = encodedFileId;
    if (!IsLocalFile(mediaInfo.mediaUrl)) {
        // local fd
        data.fd = ConvertFileId(mediaInfo.mediaUrl);
        data.localFile = "";
        data.fileLen = GetFileLengthByFd(data.fd);
    } else {
        // local file url
        CLOGE("not support local file url");
        return false;
    }
    CLOGD("encoded url %s, Local fd: %{public}d, len: %{public}" PRId64, encodedFileId.c_str(), data.fd, data.fileLen);

    // Add to local map for feature use.
    AddFileInfoToMap(encodedFileId, data);

    // Modify mediaUrl with encoded string and updata mediaSize.
    mediaInfo.mediaUrl = encodedFileId;
    mediaInfo.mediaSize = static_cast<size_t>(data.fileLen);
    return true;
}

void CastLocalFileChannelServer::ClearAllLocalFileInfo()
{
    CLOGI("in");
    ClearAllMapInfo();
}

void CastLocalFileChannelServer::OnDataReceived(const uint8_t *buffer, unsigned int length, long timeCost)
{
    if (!buffer || length == 0) {
        CLOGE("Invalid param buffer length %{public}u", length);
        return;
    }

    ProcessRequestData(buffer, length);
}

void CastLocalFileChannelServer::ProcessRequestData(const uint8_t *buffer, int length)
{
    CLOGD("request len %{public}d", length);

    // Valid and Parse http header
    std::map<std::string, std::string> request;
    if (!ParseHttpRequest(buffer, length, request)) {
        CLOGE("Invalid http header");
        return;
    }

    // Get request param and valid. URI/Range is checked in ParseHttpRequest
    if (request[HTTP_REQ_METHOD].compare("GET") != 0) {
        CLOGE("Not support request method %{public}s", request[HTTP_REQ_METHOD].c_str());
        return;
    }

    // Response
    int64_t start = 0;
    int64_t end = 0;
    bool ret = ParseStringToInt64(request[HTTP_REQ_RANGE_START], start);
    ret = ret && ParseStringToInt64(request[HTTP_REQ_RANGE_END], end);
    if (!ret || start <= INVALID_END_POS) {
        CLOGE("Invalid request param, start:%{public}" PRId64 ", end: %{public}" PRId64, start, end);
        return;
    }

    ResponseFileRequest(request[HTTP_REQ_URI], start, end);
}

struct CastLocalFileChannelServer::LocalFileInfo CastLocalFileChannelServer::FindLocalFileInfo(
    const std::string &encodedUri)
{
    std::lock_guard<std::mutex> lock(mapLock_);
    auto it = fileMap_.find(encodedUri);
    if (it != fileMap_.end()) {
        return it->second;
    }

    return {};
}

int64_t CastLocalFileChannelServer::GetFileLengthByFd(int fd)
{
    // This lseek may cause performance issues. Record the consumption time and print.

    off64_t fileLen = lseek64(fd, 0, SEEK_END);
    if (fileLen < 0) {
        CLOGE("file seek error, errno %{public}s", strerror(errno));
        return -1;
    }
    int64_t ret = static_cast<int64_t>(fileLen);
    return ret;
}

int64_t CastLocalFileChannelServer::GetFileLengthByFileName(const std::string &file)
{
    struct stat filestatus;
    if (stat(file.c_str(), &filestatus) < 0) {
        CLOGE("file %s stat fail, errno %{public}s", file.c_str(), strerror(errno));
        return 0;
    }
    return filestatus.st_size;
}

int64_t CastLocalFileChannelServer::FindFileLengthByUri(const std::string &encodeUri)
{
    std::lock_guard<std::mutex> lock(mapLock_);
    auto it = fileMap_.find(encodeUri);
    if (it == fileMap_.end()) {
        return 0;
    }
    return (it->second).fileLen;
}

void CastLocalFileChannelServer::ResponseFileLengthRequest(const std::string &uri, int64_t fileLen)
{
    if (fileLen <= 0 || !channel_) {
        return;
    }

    std::string rsp("HTTP/1.1 200 OK\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 0\r\n"
        "Content-Range: bytes ");
    rsp.append(std::to_string(0) + "-" + std::to_string(0) + "/" + std::to_string(fileLen) + "\r\n");
    rsp.append("Content-Disposition: attachment; filename=" + uri + "\r\n\r\n");

    CLOGI("filelen %{public}" PRId64, fileLen);

    // Not support data encrypt yet
    int len = static_cast<int>(rsp.size());

    SendData(reinterpret_cast<uint8_t *>(const_cast<char *>(rsp.data())), len);
}

int CastLocalFileChannelServer::ReadFileData(const struct LocalFileInfo &data, int64_t start, int64_t sendLen,
    uint8_t *ptr)
{
    int readLen = 0;
    int fd = data.fd;

    if (fd == INVALID_VALUE) {
        CLOGE("Invalid file info");
        return -1;
    }

    readLen = ReadFileDataByFd(fd, start, sendLen, ptr);

    return readLen;
}

void CastLocalFileChannelServer::ResponseFileDataRequest(const std::string &uri, int64_t fileLen, int64_t start,
    int64_t end)
{
    int64_t newEnd = end;
    // If end is not in the request
    if (newEnd <= 0 || !channel_) {
        newEnd = fileLen;
    }
    newEnd = std::min(fileLen, std::min(newEnd, start + MAX_READ_LEN));
    if (newEnd <= start) {
        CLOGE("Invalid pos %{public}" PRId64 "-%{public}" PRId64, newEnd, start);
        return;
    }

    int sendLen = static_cast<int>(std::max(static_cast<int64_t>(0), newEnd - start));

    // Make response header
    std::string rsp("HTTP/1.1 200 OK\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: ");
    rsp.append(std::to_string(sendLen) + "\r\n");
    rsp.append("Content-Range: bytes " + std::to_string(start) + "-" + std::to_string(newEnd) + "/" +
        std::to_string(fileLen) + "\r\n");
    rsp.append("Content-Disposition: attachment; filename=" + uri + "\r\n\r\n");

    // Alloca response buffer
    size_t offset = rsp.size();
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(sendLen + offset);
    if (!buffer) {
        CLOGE("malloc buffer[%{public}d] fail", sendLen);
        return;
    }
    // Copy http header to buffer
    if (memcpy_s(buffer.get(), sendLen + offset, rsp.data(), rsp.size()) != EOK) {
        CLOGE("memcpy_s fail");
        return;
    }

    LocalFileInfo data = FindLocalFileInfo(uri);
    uint8_t *ptr = buffer.get() + offset;
    int readLen = ReadFileData(data, start, sendLen, ptr);
    if (readLen > 0) {
        // Send response
        SendData(buffer.get(), sendLen + offset);
        CLOGD("send out start:%{public}" PRId64 " len:%{public}d", start, sendLen);
    }
}

void CastLocalFileChannelServer::ResponseFileRequest(const std::string &uri, int64_t start, int64_t end)
{
    if (uri.empty()) {
        return;
    }

    int64_t fileLen = FindFileLengthByUri(uri);
    if (fileLen <= 0) {
        return;
    }

    if (start == 0 && end == 0) {
        ResponseFileLengthRequest(uri, fileLen);
    } else {
        ResponseFileDataRequest(uri, fileLen, start, end);
    }
}

void CastLocalFileChannelServer::SendData(const uint8_t *buffer, int length)
{
    if (!buffer || length <= 0) {
        return;
    }
    std::shared_ptr<Channel> channel;
    {
        std::unique_lock<std::mutex> lock(chLock_);
        if (!channel_) {
            CLOGE("channel is not created.");
            return;
        }
        channel = channel_;
    }

    channel->Send(buffer, length);
}

int CastLocalFileChannelServer::ReadFileDataByFd(int fd, int64_t start, int sendLen, uint8_t *buffer)
{
    // seek to start, read data
    if (start != lseek64(fd, start, SEEK_SET)) {
        CLOGE("lseek64 fail, start:%{public}" PRId64 " errno = %{public}s", start, strerror(errno));
        return 0;
    }

    int nread = 0;
    int total = 0;
    nread = read(fd, buffer + total, sendLen - total);
    while (total < sendLen && nread > 0) {
        total += nread;
        nread = read(fd, buffer + total, sendLen - total);
    };

    return total;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
