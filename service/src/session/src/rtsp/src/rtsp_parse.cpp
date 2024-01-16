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
 * Description: rtsp parse class
 * Author: dingkang
 * Create: 2022-01-28
 */

#include "rtsp_parse.h"

#include "cast_engine_log.h"
#include "rtsp_basetype.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
DEFINE_CAST_ENGINE_LABEL("Cast-Rtsp-Parse");

int RtspParse::GetSeq()
{
    std::unordered_map<std::string, std::string>::const_iterator got = headers_.find("cseq");
    if (got == headers_.end()) {
        sequence_ = 0;
    } else {
        sequence_ = ParseIntSafe(got->second);
    }

    return sequence_;
}

void RtspParse::ParseMsg(const std::string &str, RtspParse &msg)
{
    CLOGD("In %{public}s", str.c_str());
    std::string unmatched;
    std::vector<std::string> spiltStrings;
    Utils::SplitString(str, spiltStrings, MSG_SEPARATOR);
    if (spiltStrings.size() <= MIN_SPLIT_LENGTH) {
        CLOGE("Invalid request msg %{public}s length %{public}zu", str.c_str(), spiltStrings.size());
        return;
    }

    msg.firstLine_ = spiltStrings[0];
    msg.statusCode_ = (msg.firstLine_.find(STATUS_OK_STR) != std::string::npos) ? STATUS_OK : 0;

    // Parsing headers of the request
    for (size_t index = 1; index < spiltStrings.size(); index++) {
        if (spiltStrings[index].length() <= MIN_LINE_LENGTH) {
            continue;
        }

        std::string subStrL;
        std::string subStrR;
        auto dotPos = spiltStrings[index].find(":");
        if (dotPos == std::string::npos) {
            unmatched.append(Utils::Trim(spiltStrings[index]));
            continue;
        } else {
            subStrL = spiltStrings[index].substr(0, dotPos);
            subStrR = spiltStrings[index].substr(dotPos + 1);
            CLOGD("Parse msg subStrL %{public}s subStrR %{public}s", subStrL.c_str(), subStrR.c_str());
        }

        if ((subStrL.length() == 0) || (subStrR.length() == 0)) {
            CLOGD("Parsed Length error %{public}zu", subStrL.length());
            continue;
        }
        CLOGD("Parse msg headers_ %{public}s %{public}s", Utils::Trim(Utils::ToLower(subStrL)).c_str(),
            Utils::Trim(subStrR).c_str());
        msg.headers_.insert(std::make_pair(Utils::Trim(Utils::ToLower(subStrL)), Utils::Trim(subStrR)));
    }
    msg.unmatchedString_ = unmatched;
    if (msg.unmatchedString_.length() > 0) {
        CLOGD("parsed Header's unmatched str = %{public}s", msg.unmatchedString_.c_str());
    }
    CLOGD("FirstLine_ %{public}s", msg.firstLine_.c_str());
}

/*
    statement:
    1. INVALID_VALUE(-1) is global error value
    2. strtol suppport "+2abc" out vlue 2, here is exception
 */
int RtspParse::ParseIntSafe(const std::string &str)
{
    if (str.size() == 0) {
        return INVALID_VALUE;
    }

    char *nextPtr = nullptr;
    long result = strtol(str.c_str(), &nextPtr, DECIMALISM);
    if (errno == ERANGE) {
        CLOGE("Parse int out of range");
        return INVALID_VALUE;
    } else if (*nextPtr != '\0') {
        CLOGE("Parse int error, invalid parament");
        return INVALID_VALUE;
    }
    return static_cast<int>(result);
}

uint32_t RtspParse::ParseUint32Safe(const std::string &str)
{
    return static_cast<uint32_t>(ParseIntSafe(str));
}

/*
    statement:
    1. INVALID_VALUE(-1) is global error value
    2. strtod suppport "+2.0abc" out vlue 2.0, here is exception
 */
double RtspParse::ParseDoubleSafe(const std::string &str)
{
    if (str.size() == 0) {
        return INVALID_VALUE;
    }

    char *nextPtr = nullptr;
    double result = strtod(str.c_str(), nullptr);
    if (errno == ERANGE) {
        CLOGE("Parse double out of range");
        return INVALID_VALUE;
    } else if (*nextPtr != '\0') {
        CLOGE("Parse double error, invalid parament");
        return INVALID_VALUE;
    }

    return result;
}

std::string RtspParse::GetTargetStr(const std::string &srcStr, const std::string &specificStr,
    const std::string &endStr)
{
    auto strPos = srcStr.find(specificStr);
    if (strPos == std::string::npos) {
        CLOGD("GetTargetStr [%{public}s] not find ", specificStr.c_str());
        return "";
    }
    std::string leftStr = srcStr.substr(strPos + specificStr.length());
    if (leftStr.length() == 0) {
        CLOGD("GetStr [%{public}s] Error, left string not enough.", specificStr.c_str());
        return "";
    }
    if (endStr.empty()) {
        return Utils::Trim(leftStr);
    }

    strPos = leftStr.find(endStr);
    if (strPos == std::string::npos) {
        CLOGD("GetTarget [%{public}s] left string not have endStr %{public}s", specificStr.c_str(), endStr.c_str());
    }

    std::string strResult = leftStr.substr(0, strPos);
    if (strResult.length() == 0) {
        CLOGD("GetTargetStr [%{public}s] Error, target string len is 0. ", specificStr.c_str());
        return "";
    }
    return Utils::Trim(strResult);
}
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
