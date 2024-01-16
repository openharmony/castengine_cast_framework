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
#include "rtsp_param_info.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
double ParamInfo::GetVersion() const
{
    return version_;
}

void ParamInfo::SetVersion(double version)
{
    this->version_ = version;
}

const EncryptionParamInfo &ParamInfo::GetEncryptionParamInfo()
{
    return encryptionParamInfo_;
}

void ParamInfo::SetEncryptionParamInfo(EncryptionParamInfo &encryptionParam)
{
    this->encryptionParamInfo_ = encryptionParam;
}

const DeviceTypeParamInfo &ParamInfo::GetDeviceTypeParamInfo()
{
    return deviceType_;
}

void ParamInfo::SetDeviceTypeParamInfo(DeviceTypeParamInfo &deviceTypeParam)
{
    this->deviceType_ = deviceTypeParam;
}

const RemoteControlParamInfo &ParamInfo::GetRemoteControlParamInfo()
{
    return remoteControlParamInfo_;
}

void ParamInfo::SetRemoteControlParamInfo(RemoteControlParamInfo &remoteControlParam)
{
    this->remoteControlParamInfo_ = remoteControlParam;
}

const AudioProperty &ParamInfo::GetAudioProperty() const
{
    return audioProperty_;
}

void ParamInfo::SetAudioProperty(AudioProperty &audioProperty)
{
    this->audioProperty_ = audioProperty;
}

const VideoProperty &ParamInfo::GetVideoProperty() const
{
    return videoProperty_;
}

void ParamInfo::SetVideoProperty(VideoProperty &videoProperty)
{
    this->videoProperty_ = videoProperty;
}

const WindowProperty &ParamInfo::GetWindowProperty()
{
    return windowProperty_;
}

void ParamInfo::SetWindowProperty(WindowProperty &windowProperty)
{
    this->windowProperty_ = windowProperty;
}

const TransferParamInfo &ParamInfo::GetTransferParamInfo()
{
    return transferParamInfo_;
}

void ParamInfo::SetTransferParamInfo(TransferParamInfo &transferParam)
{
    this->transferParamInfo_ = transferParam;
}

VtpType ParamInfo::GetSupportVtpOpt() const
{
    return supportVtpOpt_;
}

void ParamInfo::SetSupportVtpOpt(VtpType supportVtpOpt)
{
    this->supportVtpOpt_ = supportVtpOpt;
}

const std::set<int> &ParamInfo::GetFeatureSet()
{
    return featureSet_;
}

void ParamInfo::SetFeatureSet(std::set<int> featureSet)
{
    this->featureSet_ = featureSet;
}

const std::string &ParamInfo::GetPlayerControllerCapability()
{
    return playerControllerCapability_;
}

void ParamInfo::SetPlayerControllerCapability(const std::string &capability)
{
    this->playerControllerCapability_ = capability;
}

const std::string ParamInfo::GetMediaCapability()
{
    return mediaCapability_;
}

void ParamInfo::SetMediaCapability(const std::string &capability)
{
    this->mediaCapability_ = capability;
}

ProjectionMode ParamInfo::GetProjectionMode()
{
    return projectionMode_;
}

void ParamInfo::SetProjectionMode(ProjectionMode projectionMode)
{
    this->projectionMode_ = projectionMode;
}
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS