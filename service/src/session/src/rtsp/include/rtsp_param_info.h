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
 * Description: rtsp parameter class
 * Author: dingkang
 * Create: 2022-01-24
 */
#ifndef LIBCASTENGINE_RTSP_PARAM_INFO_H
#define LIBCASTENGINE_RTSP_PARAM_INFO_H

#include <set>
#include <string>

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
struct DeviceTypeParamInfo {
    DeviceType localDeviceType;
    SubDeviceType localDeviceSubtype;
    DeviceType remoteDeviceType;
    SubDeviceType remoteDeviceSubtype;
};

struct RemoteControlParamInfo {
    bool isSupportUibc{ false };
    bool isSupportGeneric{ false };
    bool isSupportHidc{ false };
    bool isSupportVendor{ false };
    std::vector<std::string> genericList;
    std::vector<std::string> hidcList;
    std::vector<std::string> vendorList;
};

struct EncryptionParamInfo {
    uint32_t dataChannelAlgId;
    uint32_t controlChannelAlgId;
};

// castSession transmit info
struct TransferParamInfo {
    std::string appId;
    std::string projectionMode;
    uint32_t asymmetricSupport;
    uint32_t maxResolutionSize;
};

enum class VtpType {
    VTP_NOT_SUPPORT_VIDEO,
    VTP_SUPPORT_VIDEO,
    VTP_SUPPORT_VIDEO_AUDIO
};

enum class ProjectionMode {
    MIRROR,
    STREAM
};

class ParamInfo {
public:
    ParamInfo() {};
    ParamInfo(const ParamInfo &) = default;
    ParamInfo &operator=(const ParamInfo &) = default;
    ~ParamInfo() {};

    double GetVersion() const;
    void SetVersion(double version);
    const EncryptionParamInfo &GetEncryptionParamInfo();
    void SetEncryptionParamInfo(EncryptionParamInfo &encryptionParam);
    const DeviceTypeParamInfo &GetDeviceTypeParamInfo();
    void SetDeviceTypeParamInfo(DeviceTypeParamInfo &deviceTypeParam);
    const RemoteControlParamInfo &GetRemoteControlParamInfo();
    void SetRemoteControlParamInfo(RemoteControlParamInfo &remoteControlParam);
    const AudioProperty &GetAudioProperty() const;
    void SetAudioProperty(AudioProperty &audioProperty);
    const VideoProperty &GetVideoProperty() const;
    void SetVideoProperty(VideoProperty &videoProperty);
    const WindowProperty &GetWindowProperty();
    void SetWindowProperty(WindowProperty &windowProperty);
    const TransferParamInfo &GetTransferParamInfo();
    void SetTransferParamInfo(TransferParamInfo &transferParam);
    VtpType GetSupportVtpOpt() const;
    void SetSupportVtpOpt(VtpType supportVtpOpt);
    const std::set<int> &GetFeatureSet();
    void SetFeatureSet(std::set<int> featureSet);
    const std::string &GetPlayerControllerCapability();
    void SetPlayerControllerCapability(const std::string &capability);
    const std::string GetMediaCapability();
    void SetMediaCapability(const std::string &capability);
    ProjectionMode GetProjectionMode();
    void SetProjectionMode(ProjectionMode projectionMode);

    // channel feature
    static const int FEATURE_BASE = 0;
    static const int FEATURE_TRIGGER_REQUEST_IDR_FRAME = FEATURE_BASE + 1;
    static const int FEATURE_STOP_VTP = FEATURE_BASE + 101;
    static const int FEATURE_KEEP_ALIVE = FEATURE_BASE + 102;
    static const int FEATURE_SEND_EVENT_CHANGE = FEATURE_BASE + 103;

    // remote control feature
    static const int FEATURE_FINE_STYLUS = FEATURE_BASE + 201;
    static const int FEATURE_SOURCE_MOUSE = FEATURE_BASE + 202;
    static const int FEATURE_SOURCE_MOUSE_HISTORY = FEATURE_BASE + 203;

private:
    double version_ = 1.0;
    VtpType supportVtpOpt_{ VtpType::VTP_NOT_SUPPORT_VIDEO };
    std::string playerControllerCapability_;
    std::string mediaCapability_;
    std::set<int> featureSet_;
    AudioProperty audioProperty_{};
    VideoProperty videoProperty_;
    WindowProperty windowProperty_{};
    DeviceTypeParamInfo deviceType_{};
    EncryptionParamInfo encryptionParamInfo_{};
    TransferParamInfo transferParamInfo_{};
    RemoteControlParamInfo remoteControlParamInfo_{};
    ProjectionMode projectionMode_{ ProjectionMode::MIRROR };
};
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // LIBCASTENGINE_RTSP_PARAM_INFO_H
