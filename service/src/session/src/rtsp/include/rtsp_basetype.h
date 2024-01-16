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
 * Description: rtsp constant
 * Author: dingkang
 * Create: 2022-01-24
 */

#ifndef LIBCASTENGINE_RTSP_BASETYPE_H
#define LIBCASTENGINE_RTSP_BASETYPE_H

#include <string>
#include <vector>
#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
enum class ActionType {
    SETUP,
    PLAY,
    PAUSE,
    TEARDOWN,
    VIDEO_START,
    VIDEO_STOP,
    SEND_EVENT_CHANGE
};

const std::vector<std::string> ACTION_TYPE_STR = {
    "SETUP", "PLAY", "PAUSE", "TEARDOWN", "VIDEO_START", "VIDEO_STOP", "SEND_EVENT_CHANGE"
};

enum class WaitResponse {
    WAITING_RSP_NONE = 0x0,
    WAITING_RSP_OPT_M1 = 0x01,
    WAITING_RSP_OPT_M2 = 0x02,
    WAITING_RSP_GET_PARAM_M3 = 0x03,
    WAITING_RSP_SET_PARAM_M4 = 0x04,
    WAITING_RSP_SET_PARAM_M5 = 0x05,
    WAITING_RSP_SETUP_M6 = 0x06,
    WAITING_RSP_PLAY_M7 = 0x07,
    WAITING_RSP_TEARDOWN_M8 = 0x08,
    WAITING_RSP_PAUSE_M9 = 0x09,
    WAITING_RSP_KA = 0x0A,
    WAITING_RSP_ANNOUNCE = 0x0B
};

static const std::string COMMON_SEPARATOR = ";";
static const std::string MSG_SEPARATOR = "\r\n";
static const std::string CONTENT_TYPE_TEXT = "Content-Type: text/parameters";
static const std::string CONTENT_LENGTH = "Content-Length: ";
static const std::string RTSP_DEFAULT_VERSION = " RTSP/1.0";
static const std::string RTSP_DEFAULT_VERSION_HDR = "RTSP/1.0 ";
static const std::string DATA = "Date: ";
static const std::string STRING_CSEQ = "Cseq: ";
static const std::string STATUS_OK_STR = "200 OK";

static const std::string MODULE_ID = "module_id";
static const std::string EVENT = "event";
static const std::string PARAM = "param";

static const int MIN_LINE_LENGTH = 3;
static const int MIN_SPLIT_LENGTH = 1;
static const int DEFAULT_LEN = 1;
static const int STATUS_OK = 200;
static const int DATE_ARRAY_LEN = 64;
static const int ONE_ENCAP_ITEM_LEN = 512;
static const int KEEP_NEG_TIMEOUT_INTERVAL = 10000;

static const int VIDEO_GOP_IPPP = -1;
static const int VIDEO_FPS_MIN = 20;
static const int VIDEO_BITRATE_MIN = 500000;
static const int VIDEO_BITRATE_MAX = 20000000;
static const int VIDEO_FPS_60 = 60;
static const int VIDEO_GOP_MIN = 30;
static const int VIDEO_GOP_MAX = 600;
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // LIBCASTENGINE_RTSP_BASETYPE_H
