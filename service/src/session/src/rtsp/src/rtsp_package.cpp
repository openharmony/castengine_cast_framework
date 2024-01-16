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
 * Create: 2022-01-28
 */

#include "rtsp_package.h"
#include "cast_engine_log.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
DEFINE_CAST_ENGINE_LABEL("Cast-Rtsp-Package");

std::string RtspEncap::AddRequestHeaders(int curSeq)
{
    CLOGD("In, surSeq %{public}d.", curSeq);
    std::string header;
    header.append(DATA)
        .append(GetNowDate())
        .append(MSG_SEPARATOR)
        .append(STRING_CSEQ)
        .append(std::to_string(curSeq))
        .append(MSG_SEPARATOR);

    return header;
}

std::string RtspEncap::AddResponseHeaders(const std::string &statusCode, int curSeq)
{
    CLOGD("In, statusCode %{public}s curSeq %{public}d.", statusCode.c_str(), curSeq);
    std::string header;
    header.append(RTSP_DEFAULT_VERSION_HDR)
        .append(statusCode)
        .append(MSG_SEPARATOR)
        .append(DATA)
        .append(GetNowDate())
        .append(MSG_SEPARATOR)
        .append("Server: localhost")
        .append(MSG_SEPARATOR)
        .append(STRING_CSEQ)
        .append(std::to_string(curSeq))
        .append(MSG_SEPARATOR);

    return header;
}

std::string RtspEncap::EncapAnnounce(const std::string &algStr, int curSeq, int version)
{
    CLOGD("In, curSeq %{public}d version %{public}d.", curSeq, version);
    std::string body;
    body.append("encrypt_description: ")
        .append("encrypt_list=")
        .append(algStr)
        .append(COMMON_SEPARATOR)
        .append("version=")
        .append(std::to_string(version))
        .append(MSG_SEPARATOR);

    std::string request;
    request.append("ANNOUNCE * RTSP/1.0")
        .append(MSG_SEPARATOR)
        .append(AddRequestHeaders(curSeq))
        .append(MSG_SEPARATOR)
        .append(CONTENT_TYPE_TEXT)
        .append(MSG_SEPARATOR)
        .append(CONTENT_LENGTH)
        .append(std::to_string(body.length()))
        .append(MSG_SEPARATOR)
        .append(MSG_SEPARATOR)
        .append(body);
    return request;
}

std::string RtspEncap::EncapRequestOption(int curSeq)
{
    CLOGD("In, curSeq %{public}d.", curSeq);
    std::string request;
    request.append("OPTIONS * RTSP/1.0")
        .append(MSG_SEPARATOR)
        .append(AddRequestHeaders(curSeq))
        .append(MSG_SEPARATOR)
        .append("Require: com.huawei.hisight1.0")
        .append(MSG_SEPARATOR)
        .append(MSG_SEPARATOR);
    return request;
}

std::string RtspEncap::EncapResponseOption(double version, int curSeq)
{
    CLOGD("In, curSeq %{public}d version %{public}lf.", curSeq, version);
    std::string response;
    response.append("RTSP/1.0 200 OK")
        .append(MSG_SEPARATOR)
        .append(AddResponseHeaders(STATUS_OK_STR, curSeq))
        .append(MSG_SEPARATOR)
        .append("Public: ")
        .append("com.huawei.hisight")
        .append(std::to_string(version))
        .append(" ,SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER")
        .append(MSG_SEPARATOR)
        .append(MSG_SEPARATOR);
    return response;
}

std::string RtspEncap::EncapRequestGetParameter(ParamInfo &param, int curSeq)
{
    CLOGD("In, curSeq %{public}d.", curSeq);
    std::string body;
    body.append("his_version")
        .append(MSG_SEPARATOR)
        .append("his_video_formats")
        .append(MSG_SEPARATOR)
        .append("his_audio_codecs")
        .append(MSG_SEPARATOR)
        .append("his_audio_formats")
        .append(MSG_SEPARATOR);
    if (param.GetRemoteControlParamInfo().isSupportUibc) {
        body.append("his_uibc_capability").append(MSG_SEPARATOR);
    }
    if (!param.GetFeatureSet().empty()) {
        body.append("his_feature").append(MSG_SEPARATOR);
    }
    if (param.GetSupportVtpOpt() != VtpType::VTP_NOT_SUPPORT_VIDEO) {
        body.append("his_vtp").append(MSG_SEPARATOR);
    }
    body.append("his_device_type")
        .append(MSG_SEPARATOR)
        .append("his_player_controller_capability")
        .append(MSG_SEPARATOR)
        .append("his_media_capability")
        .append(MSG_SEPARATOR);

    std::string request;
    request.append("GET_PARAMETER rtsp://localhost/hisight")
        .append(std::to_string(param.GetVersion()))
        .append(RTSP_DEFAULT_VERSION)
        .append(MSG_SEPARATOR)
        .append(AddRequestHeaders(curSeq))
        .append(CONTENT_TYPE_TEXT)
        .append(MSG_SEPARATOR)
        .append(CONTENT_LENGTH)
        .append(std::to_string(body.length()))
        .append(MSG_SEPARATOR)
        .append(MSG_SEPARATOR);
    request.append(body);

    return request;
}

void RtspEncap::EncapFeature(std::string &body, ParamInfo &negParam)
{
    CLOGD("Encap GetParam response his_feature.");
    body.append("his_feature: ");
    body.append("input_feature_set=");

    std::set<int>::iterator featureIndex = negParam.GetFeatureSet().begin();
    std::set<int>::iterator endIterator = negParam.GetFeatureSet().end();
    while (featureIndex != endIterator) {
        std::string featureStr = std::to_string(*featureIndex);
        CLOGI("Encapsulate feature set %{public}s", featureStr.c_str());
        body.append(featureStr);
        featureIndex++;
        body.append((featureIndex != endIterator) ? ", " : "");
    }

    body.append(COMMON_SEPARATOR).append(MSG_SEPARATOR);
}

void RtspEncap::EncapResponseGetParamM3Body(ParamInfo &clientParam, const std::string &getParam, std::string &body)
{
    if (getParam.find("his_version") != getParam.npos) {
        body.append("his_version: ").append(std::to_string(clientParam.GetVersion())).append(MSG_SEPARATOR);
    }
    if (getParam.find("his_video_formats") != getParam.npos) {
        body.append("his_video_formats: ").append(SetVideoAndAudioCodecsParameter(clientParam)).append(MSG_SEPARATOR);
    }
    if (getParam.find("his_audio_formats") != getParam.npos) {
        body.append(SetAudioParameter(clientParam)).append(MSG_SEPARATOR);
    }
    if (getParam.find("his_uibc_capability") != getParam.npos) {
        EncapUibc(body, clientParam);
    }
    if (getParam.find("his_vtp") != getParam.npos) {
        CLOGI("GetParamResponse support vtp flag : %{public}d", clientParam.GetSupportVtpOpt());
        if (clientParam.GetSupportVtpOpt() != VtpType::VTP_NOT_SUPPORT_VIDEO) {
            body.append("his_vtp: ").append((clientParam.GetSupportVtpOpt() == VtpType::VTP_SUPPORT_VIDEO) ?
                "supported" : "supportAV").append(MSG_SEPARATOR);
        }
    }

    if (getParam.find("his_feature") != getParam.npos) {
        EncapFeature(body, clientParam);
    }

    if (getParam.find("his_device_type") != getParam.npos) {
        // sink端local device为source端remote device
        DeviceTypeParamInfo deviceTypeParam = clientParam.GetDeviceTypeParamInfo();
        body.append("his_device_type: ").append("device_type ")
            .append(std::to_string(std::underlying_type_t<DeviceType>(deviceTypeParam.localDeviceType)))
            .append(COMMON_SEPARATOR).append("subtype ")
            .append(std::to_string(std::underlying_type_t<SubDeviceType>(deviceTypeParam.localDeviceSubtype)))
            .append(COMMON_SEPARATOR).append(MSG_SEPARATOR);
    }

    if (getParam.find("his_player_controller_capability") != getParam.npos) {
        std::string playerControllerCapability = GetPlayerControllerCapability(clientParam);
        if (!playerControllerCapability.empty()) {
            body.append(playerControllerCapability);
        }
    }

    if (getParam.find("his_media_capability") != getParam.npos) {
        std::string mediaCapability = GetMediaCapability(clientParam);
        if (!mediaCapability.empty()) {
            body.append(mediaCapability);
        }
    }
}

std::string RtspEncap::EncapResponseGetParamM3(ParamInfo &clientParam, RtspParse &request, int seq)
{
    CLOGD("Firstline %{public}s GetUnMatchedStr %{public}s", request.GetFirstLine().c_str(),
        request.GetUnMatchedStr().c_str());
    std::string getParam = request.GetUnMatchedStr();
    std::string body;
    EncapResponseGetParamM3Body(clientParam, getParam, body);

    std::string response;
    response.append(AddResponseHeaders(STATUS_OK_STR, seq))
        .append(CONTENT_TYPE_TEXT)
        .append(MSG_SEPARATOR)
        .append(CONTENT_LENGTH)
        .append(std::to_string(body.length()))
        .append(MSG_SEPARATOR)
        .append(MSG_SEPARATOR)
        .append(body)
        .append(MSG_SEPARATOR);
    CLOGD("EncapResponseGetParamM3. response %{public}s, body %{public}s", response.c_str(), body.c_str());
    return response;
}

std::string RtspEncap::GetInputCategoryList(const RemoteControlParamInfo &paramInfo)
{
    if (paramInfo.isSupportGeneric && paramInfo.isSupportHidc) {
        return "GENERIC/HIDC";
    }
    if (paramInfo.isSupportGeneric) {
        return "GENERIC";
    }
    if (paramInfo.isSupportHidc) {
        return "HIDC";
    }

    CLOGE("don't support generic and hidc.");
    return {};
}

namespace {
void EncapSpecialList(std::string &body, const std::vector<std::string> &list, const std::string &type)
{
    size_t index = 0;
    for (const auto &param : list) {
        CLOGD("Encapsulated %{public}s param is %{public}s", type.c_str(), param.c_str());
        body.append(param).append((index++ != (list.size() - 1)) ? ", " : "");
    }
    body.append(COMMON_SEPARATOR);
}
} // namespace

void RtspEncap::EncapGenericCapList(std::string &body, const RemoteControlParamInfo &paramInfo)
{
    if (!paramInfo.isSupportGeneric) {
        return;
    }

    body.append("generic_cap_list=");
    EncapSpecialList(body, paramInfo.genericList, "generic");
}

void RtspEncap::EncapHidcCapList(std::string &body, const RemoteControlParamInfo &paramInfo)
{
    if (!paramInfo.isSupportHidc) {
        return;
    }

    body.append("hidc_cap_list=");
    EncapSpecialList(body, paramInfo.hidcList, "hidc");
}

void RtspEncap::EncapVendorCapList(std::string &body, const RemoteControlParamInfo &paramInfo)
{
    if (!paramInfo.isSupportVendor) {
        return;
    }

    body.append("vendor_cap_list=");
    EncapSpecialList(body, paramInfo.vendorList, "vendor");
}

void RtspEncap::EncapUibc(std::string &body, ParamInfo &negParam)
{
    CLOGD("Encap Uibc.");
    RemoteControlParamInfo remoteControlParam = negParam.GetRemoteControlParamInfo();
    if (!remoteControlParam.isSupportGeneric && !remoteControlParam.isSupportHidc) {
        CLOGE("encapUibc inner error.");
        return;
    }
    body.append("his_uibc_capability: ").append("input_category_list=");
    auto list = GetInputCategoryList(remoteControlParam);
    if (!list.empty()) {
        body.append(list);
    }
    body.append(COMMON_SEPARATOR);

    EncapGenericCapList(body, remoteControlParam);
    EncapHidcCapList(body, remoteControlParam);
    EncapVendorCapList(body, remoteControlParam);
    body.append(MSG_SEPARATOR);
}

void RtspEncap::SetAnotherParameter(ParamInfo &negParam, double version, const std::string &ip, std::string &body)
{
    if (!negParam.GetFeatureSet().empty()) {
        EncapFeature(body, negParam);
    }
    body.append("his_presentation_URL: ")
        .append("rtsp://")
        .append(ip)
        .append("/hisight")
        .append(std::to_string(version))
        .append("/streamid=0 none")
        .append(MSG_SEPARATOR);

    body.append("his_version: ").append(std::to_string(version)).append(MSG_SEPARATOR);

    CLOGI("Encap SupportUibc %{public}d SupportVtp %{public}d ", negParam.GetRemoteControlParamInfo().isSupportUibc,
        negParam.GetSupportVtpOpt());
    if (negParam.GetRemoteControlParamInfo().isSupportUibc) {
        EncapUibc(body, negParam);
    }
    DeviceTypeParamInfo deviceTypeParam = negParam.GetDeviceTypeParamInfo();
    body.append("his_device_type: ")
        .append("source_device_type ")
        .append(std::to_string(std::underlying_type_t<DeviceType>(deviceTypeParam.localDeviceType)))
        .append(COMMON_SEPARATOR)
        .append("source_subtype ")
        .append(std::to_string(std::underlying_type_t<SubDeviceType>(deviceTypeParam.localDeviceSubtype)))
        .append(COMMON_SEPARATOR)
        .append("device_type ")
        .append(std::to_string(std::underlying_type_t<DeviceType>(deviceTypeParam.remoteDeviceType)))
        .append(COMMON_SEPARATOR)
        .append("subtype ")
        .append(std::to_string(std::underlying_type_t<SubDeviceType>(deviceTypeParam.remoteDeviceSubtype)))
        .append(COMMON_SEPARATOR)
        .append(MSG_SEPARATOR);
    // his_extended_field projection_mode app_id todo
    if (negParam.GetSupportVtpOpt() != VtpType::VTP_NOT_SUPPORT_VIDEO) {
        body.append("his_vtp: ")
            .append((negParam.GetSupportVtpOpt() == VtpType::VTP_SUPPORT_VIDEO) ? "supported" : "supportAV")
            .append(MSG_SEPARATOR);
    }
    body.append("his_extended_field: ")
        .append("projection_mode")
        .append(negParam.GetProjectionMode() == ProjectionMode::STREAM ? "MEDIA_RESOURCE" : "MIRROR")
        .append(COMMON_SEPARATOR)
        .append(MSG_SEPARATOR);
    body.append("his_media_capability: ")
        .append(negParam.GetMediaCapability())
        .append(MSG_SEPARATOR)
        .append("his_player_controller_capability: ")
        .append(negParam.GetPlayerControllerCapability())
        .append(MSG_SEPARATOR);
}

std::string RtspEncap::EncapSetParameterM4Request(ParamInfo &negParam, double version, const std::string &ip, int seq)
{
    CLOGD("Encap SetParameter M4 request.");
    std::string body;
    body.append(SetVideoAndAudioCodecsParameter(negParam));
    body.append(SetAudioParameter(negParam));

    SetAnotherParameter(negParam, version, ip, body);

    std::string request;
    request.append("SET_PARAMETER rtsp://localhost/hisight")
        .append(std::to_string(version))
        .append(RTSP_DEFAULT_VERSION)
        .append(MSG_SEPARATOR);
    request.append(AddRequestHeaders(seq)).append(CONTENT_TYPE_TEXT).append(MSG_SEPARATOR);
    request.append(CONTENT_LENGTH).append(std::to_string(body.length())).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);
    request.append(body).append(MSG_SEPARATOR);
    return request;
}

std::string RtspEncap::GetPlayerControllerCapability(ParamInfo &inputParam)
{
    CLOGI("In, player controller capability: %{public}s", inputParam.GetPlayerControllerCapability().c_str());

    if (inputParam.GetPlayerControllerCapability().empty()) {
        return "";
    }

    return "his_player_controller_capability: " + inputParam.GetPlayerControllerCapability() + COMMON_SEPARATOR +
        MSG_SEPARATOR;
}

std::string RtspEncap::GetMediaCapability(ParamInfo &inputParam)
{
    CLOGI("In, media capability: %{public}s", inputParam.GetMediaCapability().c_str());
    if (inputParam.GetMediaCapability().empty()) {
        return "";
    }

    return "his_media_capability: " + inputParam.GetMediaCapability() + COMMON_SEPARATOR + MSG_SEPARATOR;
}

std::string RtspEncap::SetVideoAndAudioCodecsParameter(ParamInfo &negParam)
{
    std::string body;
    body.append("his_video_formats: ")
        .append("codecs ")
        .append(std::to_string(static_cast<int>(negParam.GetVideoProperty().codecType)))
        .append(COMMON_SEPARATOR)
        .append("fps ")
        .append(std::to_string(negParam.GetVideoProperty().fps))
        .append(COMMON_SEPARATOR)
        .append("gop ")
        .append(std::to_string(negParam.GetVideoProperty().gop))
        .append(COMMON_SEPARATOR)
        .append("bitrate ")
        .append(std::to_string(negParam.GetVideoProperty().bitrate))
        .append(COMMON_SEPARATOR)
        .append("vbr-min ")
        .append(std::to_string(negParam.GetVideoProperty().minBitrate))
        .append(COMMON_SEPARATOR)
        .append("vbr-max ")
        .append(std::to_string(negParam.GetVideoProperty().maxBitrate))
        .append(COMMON_SEPARATOR)
        .append("dpi ")
        .append(std::to_string(negParam.GetVideoProperty().dpi))
        .append(COMMON_SEPARATOR)
        .append("scr-w ")
        .append(std::to_string(negParam.GetWindowProperty().width))
        .append(COMMON_SEPARATOR)
        .append("scr-h ")
        .append(std::to_string(negParam.GetWindowProperty().height))
        .append(COMMON_SEPARATOR)
        .append("color-standard ")
        .append(std::to_string(static_cast<int>(negParam.GetVideoProperty().colorStandard)))
        .append(COMMON_SEPARATOR)
        .append("width ")
        .append(std::to_string(negParam.GetVideoProperty().videoWidth))
        .append(COMMON_SEPARATOR)
        .append("height ")
        .append(std::to_string(negParam.GetVideoProperty().videoHeight))
        .append(MSG_SEPARATOR);
    CLOGI("Set video format, width %{public}d height %{public}d color-standard %{public}d",
        negParam.GetVideoProperty().videoWidth, negParam.GetVideoProperty().videoHeight,
        negParam.GetVideoProperty().colorStandard);
    if (negParam.GetAudioProperty().codec > 0) {
        body.append("his_audio_codecs: ")
            .append(std::to_string(negParam.GetAudioProperty().codec))
            .append(MSG_SEPARATOR);
    }
    CLOGI("Sink format %{public}s.", body.c_str());
    return body;
}

std::string RtspEncap::SetAudioParameter(ParamInfo &negParam)
{
    std::string body;
    body.append("his_audio_formats: ")
        .append("sample-rate ")
        .append(std::to_string(negParam.GetAudioProperty().sampleRate))
        .append(COMMON_SEPARATOR)
        .append("sample-bit-width ")
        .append(std::to_string(negParam.GetAudioProperty().sampleBitWidth))
        .append(COMMON_SEPARATOR)
        .append("channel-config ")
        .append(std::to_string(negParam.GetAudioProperty().channelConfig))
        .append(COMMON_SEPARATOR)
        .append("bitrate ")
        .append(std::to_string(negParam.GetAudioProperty().bitrate))
        .append(MSG_SEPARATOR);

    CLOGI("Sink format %{public}s.", body.c_str());

    return body;
}

// source->sink SetParameter(his_trigger_method:PLAY\PAUSE...)
// source<-sink PLAY\PAUSE...)
std::string RtspEncap::EncapActionRequest(ActionType actionType, double version, int curSeq)
{
    CLOGD("Encap Action request.");
    std::string request;
    request.append("SET_PARAMETER rtsp://localhost/hisight")
        .append(std::to_string(version))
        .append(RTSP_DEFAULT_VERSION)
        .append(MSG_SEPARATOR)
        .append(AddRequestHeaders(curSeq))
        .append(CONTENT_TYPE_TEXT)
        .append(MSG_SEPARATOR);

    std::string body;
    body.append("his_trigger_method: ").append(ACTION_TYPE_STR[static_cast<int>(actionType)]).append(MSG_SEPARATOR);

    request.append(CONTENT_LENGTH)
        .append(std::to_string(body.length()))
        .append(MSG_SEPARATOR)
        .append(MSG_SEPARATOR)
        .append(body)
        .append(MSG_SEPARATOR);

    return request;
}

std::string RtspEncap::EncapKeepAliveRequest(int curSeq, double version)
{
    CLOGD("Encap KeepAlive request.");
    std::string request;
    request.append("GET_PARAMETER rtsp://localhost/hisight")
        .append(std::to_string(version))
        .append(RTSP_DEFAULT_VERSION)
        .append(MSG_SEPARATOR);
    request.append(AddRequestHeaders(curSeq));
    return request;
}

std::string RtspEncap::EncapCommonResponse(RtspParse &request, const std::string &statusCode)
{
    CLOGD("Encap Common response.");
    int seqNumber = INVALID_VALUE;
    std::string cseq = request.GetHeader()["cseq"];
    if (!cseq.empty()) {
        seqNumber = RtspParse::ParseIntSafe(Utils::Trim(cseq));
    }

    return AddResponseHeaders(statusCode, seqNumber) + MSG_SEPARATOR;
}

std::string RtspEncap::EncapSetupRequest(int cseq, const std::string &uri, int port)
{
    CLOGD("Encap Setup request.");
    std::string request;
    if (uri.length() > 0) {
        request.append("SETUP ").append(uri).append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    } else {
        request.append("SETUP ").append("*").append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    }
    request.append("CSeq: ").append(std::to_string(cseq)).append(MSG_SEPARATOR);
    request.append("Transport: RTP/AVP/UDP;unicast;client_port=").append(std::to_string(port)).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);

    return request;
}

std::string RtspEncap::EncapSetupResponse(ParamInfo &param, int cseq, int serverPort, int remotectlPort, int rtcpPort)
{
    CLOGD("Encap Setup response.");
    if (cseq <= 0) {
        return "";
    }

    std::string response;
    /* begin to init streaming. and get the source port. */
    response.append("Transport: ")
        .append("RTP/AVP/TCP")
        .append(COMMON_SEPARATOR)
        .append("unicast")
        .append(COMMON_SEPARATOR)
        .append("rtcp_port=")
        .append(std::to_string(rtcpPort))
        .append(COMMON_SEPARATOR)
        .append("server_port=")
        .append(std::to_string(serverPort))
        .append(COMMON_SEPARATOR)
        .append("remotectl_port=")
        .append(std::to_string(remotectlPort))
        .append(MSG_SEPARATOR);

    int len = static_cast<int>(response.length());
    std::string resp = RTSP_DEFAULT_VERSION_HDR + STATUS_OK_STR + MSG_SEPARATOR + DATA + GetNowDate() + MSG_SEPARATOR +
        "Server: localhost" + MSG_SEPARATOR + STRING_CSEQ + std::to_string(cseq) + MSG_SEPARATOR + CONTENT_TYPE_TEXT +
        MSG_SEPARATOR + CONTENT_LENGTH + std::to_string(len) + MSG_SEPARATOR + response;

    return resp;
}

std::string RtspEncap::EncapPlayRequest(int cseq, const std::string &uri, int port)
{
    CLOGD("Encap Play request.");
    std::string request;
    if (uri.length() > 0) {
        request.append("PLAY ").append(uri).append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    } else {
        request.append("PLAY ").append("*").append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    }
    request.append("CSeq: ").append(std::to_string(cseq)).append(MSG_SEPARATOR);
    request.append("Transport: RTP/AVP/UDP;unicast;client_port=").append(std::to_string(port)).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);

    return request;
}

std::string RtspEncap::EncapTearDownRequest(int cseq, const std::string &uri)
{
    CLOGD("Encap TearDown request.");
    std::string request;
    request.append("TEARDOWN ").append(uri).append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    request.append("CSeq: ").append(std::to_string(cseq)).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);

    return request;
}

std::string RtspEncap::EncapPauseRequest(int cseq, const std::string &uri)
{
    CLOGD("Encap Pause request.");
    std::string request;
    request.append("PAUSE ").append(uri).append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    request.append("CSeq: ").append(std::to_string(cseq)).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);

    return request;
}

std::string RtspEncap::EncapEventChangeRequest(int moduleId, int event, const std::string &param, double version,
    int curSeq)
{
    CLOGI("Encap event change request");
    std::string request;
    request.append("SET_PARAMETER rtsp://localhost/hisight")
        .append(std::to_string(version))
        .append(RTSP_DEFAULT_VERSION)
        .append(MSG_SEPARATOR);
    request.append(AddRequestHeaders(curSeq)).append(CONTENT_TYPE_TEXT).append(MSG_SEPARATOR);

    std::string body;
    body.append("his_trigger_method: ").append("SEND_EVENT_CHANGE").append(MSG_SEPARATOR);
    body.append("module_id: ").append(std::to_string(moduleId)).append(MSG_SEPARATOR);
    body.append("event: ").append(std::to_string(event)).append(MSG_SEPARATOR);
    body.append("param: ").append(param).append(MSG_SEPARATOR);

    request.append(CONTENT_LENGTH).append(std::to_string(body.length())).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);
    request.append(body).append(MSG_SEPARATOR);
    return request;
}

// trigger_method  ActionType
std::string RtspEncap::EncapSetParamRequestM11(int cseq, const std::string &uri, int trigger)
{
    CLOGD("Encap SetParam M11 request.");
    std::string request;
    request.append("SET_PARAMETER ").append(uri).append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    request.append("CSeq: ").append(std::to_string(cseq)).append(MSG_SEPARATOR);
    request.append("Trigger: ").append(std::to_string(trigger)).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);
    return request;
}

std::string RtspEncap::EncapCastRenderReadyRequest(int cseq, const std::string &uri, int readyFlag)
{
    CLOGD("Encap CastRenderReady request.");
    std::string request;
    request.append("RENDER_READY ").append(uri).append(RTSP_DEFAULT_VERSION).append(MSG_SEPARATOR);
    request.append("CSeq: ").append(std::to_string(cseq)).append(MSG_SEPARATOR);
    request.append("readyflag: ").append(std::to_string(readyFlag)).append(MSG_SEPARATOR);
    request.append(MSG_SEPARATOR);
    return request;
}

std::string RtspEncap::GetNowDate()
{
    struct tm nowTime;
    time_t timep = time(nullptr);
    if (localtime_r(&timep, &nowTime) == nullptr) {
        return "";
    }

    char tmp[DATE_ARRAY_LEN] = {0};
    if (strftime(tmp, sizeof(tmp), "%Y-%m-%{public}d %H:%M:%{public}s", &nowTime) == 0) {
        return "";
    }

    return std::string(tmp);
}
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS