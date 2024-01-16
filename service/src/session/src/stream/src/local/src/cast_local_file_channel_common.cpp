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
 * Description: Cast local file channel common impl
 * Author: liaoxin
 * Create: 2023-04-20
 */

#include "cast_local_file_channel_common.h"

#include <cinttypes>
#include <map>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "cast_engine_common.h"
#include "cast_engine_log.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Localfile-Common");

namespace {
const std::string COMMON_SEPARATOR = ";";
const std::string MSG_SEPARATOR = "\r\n";
const std::string BLANK_SEPARATOR = " ";
const std::string HTTP_HEADER_END_SEPARATOR = "\r\n\r\n";
const std::string HTTP_HEADER_ENDX_SEPARATOR = "\n\n";
const std::string HTTP_HEADER_RANGE = "Range";
const std::string CONTENT_TYPE_TEXT = "Content-Type";
const std::string CONTENT_LENGTH = "Content-Length";
const std::string CONTENT_RANGE = "Content-Range";
const std::string CONTENT_DISPOSITION = "Content-Disposition";
const std::string STATUS_OK_STR = "200 OK";

const int RANGE_START_IDX = 1;
const int RANGE_END_IDX = 2;
const int HTTP_REQ_METHOD_IDX = 0;
const int HTTP_REQ_URI_IDX = 1;
const int HTTP_RSP_PRO_IDX = 0;
const int HTTP_RSP_CODE_IDX = 1;
const int HTTP_REQ_CNT = 2;
const int HTTP_RSP_CNT = 2;
const int HTTP_MIN_HEADER = 2;

bool IsRemoteUrl(const std::string &url)
{
    return (url.find("http") == 0);
}

bool IsAllDigtal(const std::string &url)
{
    for (const auto &c : url) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

std::optional<size_t> CheckAndGetHttpHeader(const uint8_t *buffer, unsigned int length,
    std::vector<std::string> &hdrList)
{
    std::string http(reinterpret_cast<const char *>(buffer), length);
    auto pos = http.find(HTTP_HEADER_END_SEPARATOR);
    size_t dataOffset = pos;
    if (pos == std::string::npos) {
        // Tolerance
        pos = http.find(HTTP_HEADER_ENDX_SEPARATOR);
        if (pos == std::string::npos) {
            CLOGE("cannot find http header");
            return std::nullopt;
        }

        dataOffset += HTTP_HEADER_ENDX_SEPARATOR.length();
    } else {
        dataOffset += HTTP_HEADER_END_SEPARATOR.length();
    }

    http.resize(pos);
    Utils::SplitString(http, hdrList, MSG_SEPARATOR);
    if (hdrList.size() < HTTP_MIN_HEADER) {
        CLOGE("Http header list %{public}zu", hdrList.size());
        return std::nullopt;
    }

    return dataOffset;
}

bool ParseHttpRangeHeader(std::string &range, int64_t &start, int64_t &end)
{
    if (range.empty()) {
        return false;
    }
    /* Parse Range
        Range: <unit>=<range-start>-
        Range: <unit>=<range-start>-<range-end>
        Range: <unit>=<range-start>-<range-end>, <range-start>-<range-end> # not support
        Range: <unit>=<range-start>-<range-end>, <range-start>-<range-end>, <range-start>-<range-end> # not support
    */
    std::regex regex("bytes=(\\d+)-(\\d+)?");
    std::smatch matches;
    if (!std::regex_search(range, matches, regex)) {
        return false;
    }

    if (!ParseStringToInt64(matches[RANGE_START_IDX].str(), start) || start <= INVALID_END_POS) {
        CLOGE("Invalid parsing or invalid start pos:%{public}" PRId64, start);
        return false;
    }

    if (matches[RANGE_END_IDX].str().empty()) {
        end = INVALID_END_POS;
        return true;
    }

    if (!ParseStringToInt64(matches[RANGE_END_IDX].str(), end)) {
        CLOGE("Invalid parsing");
        return false;
    }

    return true;
}

void ParseHttpHeaderList(std::vector<std::string> &hdrList, std::map<std::string, std::string> &request)
{
    // Parse other header
    for (size_t index = 1; index < hdrList.size(); index++) {
        // split with ": ". A ":" may used as prefix in http/2, skip this
        std::size_t pos = hdrList[index].find(": ");
        if (pos != std::string::npos) {
            std::string value = hdrList[index].substr(pos + std::string(": ").size());
            request.insert({ hdrList[index].substr(0, pos), Utils::Trim(value) });
        }
    }
}
}

bool IsLocalFile(const std::string &url)
{
    const std::regex regex("^/(.+)$");
    std::smatch matches;
    return std::regex_search(url, matches, regex);
}

bool IsLocalUrl(const std::string &url)
{
    if (url.empty()) {
        return false;
    }
    /* 3 types
     * remote url: start with "http"
     * local file: start with '/'
     * fd: number
     */
    if (IsRemoteUrl(url)) {
        return false;
    }

    if (IsLocalFile(url)) {
        return true;
    }

    return IsAllDigtal(url);
}

int ConvertFileId(const std::string &fileId)
{
    int fd = INVALID_VALUE;

    if (fileId.size() == 0) {
        CLOGE("fileId len 0");
        return INVALID_VALUE;
    }

    char *nextPtr = nullptr;
    fd = static_cast<int>(strtol(fileId.c_str(), &nextPtr, DECIMALISM));
    if (errno == ERANGE) {
        CLOGE("out of range");
        return INVALID_VALUE;
    } else if (*nextPtr != '\0') {
        CLOGE("invalid param");
        return INVALID_VALUE;
    }

    return fd;
}

bool ParseStringToInt64(const std::string &str, int64_t &val)
{
    errno = 0;
    val = std::strtoll(str.c_str(), nullptr, DECIMALISM);
    if (errno == ERANGE) {
        return false;
    }

    return true;
}

bool ParseHttpRequest(const uint8_t *buffer, int length, std::map<std::string, std::string> &request)
{
    std::vector<std::string> hdrList;
    if (!CheckAndGetHttpHeader(buffer, length, hdrList)) {
        return false;
    }

    // Process http request line
    std::vector<std::string> reqList;
    Utils::SplitString(hdrList[0], reqList, BLANK_SEPARATOR);
    if (reqList.size() < HTTP_REQ_CNT) {
        return false;
    }
    request.insert({ HTTP_REQ_METHOD, reqList[HTTP_REQ_METHOD_IDX] });
    request.insert({ HTTP_REQ_URI, reqList[HTTP_REQ_URI_IDX] });

    // Parse other header
    ParseHttpHeaderList(hdrList, request);

    // Parse "Range", it is necessary
    if (request.count(HTTP_HEADER_RANGE) == 0) {
        return false;
    }
    int64_t start;
    int64_t end;
    if (!ParseHttpRangeHeader(request[HTTP_HEADER_RANGE], start, end)) {
        return false;
    }
    request.insert({ HTTP_REQ_RANGE_START, std::to_string(start) });
    request.insert({ HTTP_REQ_RANGE_END, std::to_string(end) });

    return true;
}

bool ParseHttpResponse(const uint8_t *buffer, int length, std::map<std::string, std::string> &response,
    size_t &dataOffset)
{
    std::vector<std::string> hdrList;
    auto offset = CheckAndGetHttpHeader(buffer, length, hdrList);
    if (!offset) {
        return false;
    }

    // Process http response line
    std::vector<std::string> rspList;
    Utils::SplitString(hdrList[0], rspList, BLANK_SEPARATOR);
    if (rspList.size() < HTTP_RSP_CNT) {
        return false;
    }
    response.insert({ HTTP_RSP_PROTOCOL, rspList[HTTP_RSP_PRO_IDX] });
    response.insert({ HTTP_RSP_CODE, rspList[HTTP_RSP_CODE_IDX] });

    // Parse other header
    ParseHttpHeaderList(hdrList, response);

    // Check necessary headers
    if (response.find(CONTENT_LENGTH) == response.end()) {
        return false;
    }
    if (response.find(CONTENT_RANGE) == response.end()) {
        return false;
    }

    response.insert({ HTTP_RSP_CONTENT_LENGTH, response[CONTENT_LENGTH] });
    // Extract Content-Range: start/end/filelen
    std::regex regexRange("bytes (\\d+)-(\\d*)/(\\d*)");
    std::smatch matches;
    if (!std::regex_search(response[CONTENT_RANGE], matches, regexRange)) {
        return false;
    }
    response.insert({ HTTP_RSP_CONTENT_RANGE_START, matches[1].str() });
    response.insert({ HTTP_RSP_CONTENT_RANGE_END, matches[2].str() });
    response.insert({ HTTP_RSP_CONTENT_RANGE_TOTAL, matches[3].str() });
    // Extract Content-Disposition
    if (response.find(CONTENT_DISPOSITION) == response.end()) {
        return false;
    }
    std::regex regexFile("filename[^;=\\n]*=((['\"]).*?\\2|[^;\\n]*)");
    if (!std::regex_search(response[CONTENT_DISPOSITION], matches, regexFile)) {
        return false;
    }
    response.insert({ HTTP_RSP_CONTENT_DISPOSITION, matches[1].str() });

    dataOffset = *offset;

    return true;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
