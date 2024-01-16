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
 * Description: rtsp package
 * Author: dingkang
 * Create: 2022-01-24
 */

#ifndef LIBCASTENGINE_RTSP_PACKAGE_H
#define LIBCASTENGINE_RTSP_PACKAGE_H

#include <string>

#include "rtsp_basetype.h"
#include "rtsp_parse.h"
#include "rtsp_param_info.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
class RtspEncap {
public:
    RtspEncap() {}
    ~RtspEncap() {}

    static std::string EncapAnnounce(const std::string &algStr, int curSeq, int version);
    static std::string EncapRequestOption(int curSeq);
    static std::string EncapRequestGetParameter(ParamInfo &param, int curSeq);
    static std::string EncapSetParameterM4Request(ParamInfo &negParam, double version, const std::string &ip, int seq);
    static std::string EncapActionRequest(ActionType actionType, double version, int curSeq);
    static std::string EncapSetupRequest(int cseq, const std::string &uri, int port);
    static std::string EncapPlayRequest(int cseq, const std::string &uri, int port);
    static std::string EncapTearDownRequest(int cseq, const std::string &uri);
    static std::string EncapPauseRequest(int cseq, const std::string &uri);
    static std::string EncapKeepAliveRequest(int curSeq, double version);
    static std::string EncapEventChangeRequest(int moduleId, int event, const std::string &param, double version,
        int curSeq);
    static std::string EncapSetParamRequestM11(int cseq, const std::string &uri, int trigger);
    static std::string EncapCastRenderReadyRequest(int cseq, const std::string &uri, int readyFlag);
    static std::string EncapCommonResponse(RtspParse &request, const std::string &statusCode);
    static std::string EncapSetupResponse(ParamInfo &param, int cseq, int serverPort, int remotectlPort, int rtcpPort);
    static std::string EncapResponseOption(double version, int curSeq);
    static std::string EncapResponseGetParamM3(ParamInfo &clientParam, RtspParse &request, int seq);
    static void EncapResponseGetParamM3Body(ParamInfo &clientParam, const std::string &getParam, std::string &body);
    static void EncapUibc(std::string &body, ParamInfo &negParam);
    static void EncapFeature(std::string &body, ParamInfo &negParam);
    static std::string SetVideoAndAudioCodecsParameter(ParamInfo &negParam);
    static std::string SetAudioParameter(ParamInfo &negParam);
    static void SetAnotherParameter(ParamInfo &negParam, double version, const std::string &ip, std::string &body);

private:
    static std::string AddRequestHeaders(int curSeq);
    static std::string AddResponseHeaders(const std::string &statusCode, int curSeq);
    static std::string GetNowDate();
    static std::string GetMediaCapability(ParamInfo &inputParam);
    static std::string GetPlayerControllerCapability(ParamInfo &inputParam);
    static std::string GetInputCategoryList(const RemoteControlParamInfo &paramInfo);
    static void EncapGenericCapList(std::string &body, const RemoteControlParamInfo &paramInfo);
    static void EncapHidcCapList(std::string &body, const RemoteControlParamInfo &paramInfo);
    static void EncapVendorCapList(std::string &body, const RemoteControlParamInfo &paramInfo);
};
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
