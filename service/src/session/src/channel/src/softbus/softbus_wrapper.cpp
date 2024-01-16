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
 * Description: softbus wrapper interface.
 * Author: sunhong
 * Create: 2022-01-22
 */

#include "softbus_wrapper.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("CastEngine-SoftBusWrapper");

std::mutex SoftBusWrapper::mutex_;
std::map<int, std::string> SoftBusWrapper::sessionIdToNameMap_ {};

SoftBusWrapper::SoftBusWrapper()
{
    attribute_.linkTypeNum = MAX_LINK_TYPE_NUM;
    attribute_.linkType[0] = LINK_TYPE_WIFI_P2P;     // Select WIFI P2P first
    attribute_.linkType[1] = LINK_TYPE_WIFI_WLAN_5G; // Then WIFI 5G
    attribute_.linkType[2] = LINK_TYPE_WIFI_WLAN_2G; // Then WIFI 2G
    attribute_.dataType = TYPE_BYTES;
}

SoftBusWrapper::~SoftBusWrapper() {}

int SoftBusWrapper::StartSoftBusService(const std::string pkgName, const std::string &sessionName,
    ISessionListener *listener)
{
    CLOGD("StartSoftBusService Enter, pkgName = %{public}s, sessionName = %{public}s.", pkgName.c_str(),
        sessionName.c_str());

    return CreateSessionServer(pkgName.c_str(), sessionName.c_str(), listener);
}

void SoftBusWrapper::StopService(const std::string pkgName, const std::string &sessionName)
{
    CLOGD("In, pkgName = %{public}s, sessionName = %{public}s.", pkgName.c_str(), sessionName.c_str());

    int ret = RemoveSessionServer(pkgName.c_str(), sessionName.c_str());
    if (ret != RET_OK) {
        CLOGE("Remove Session Server Failed, pkgName = %{public}s, sessionName = %{public}s, ret = %{public}d.",
            pkgName.c_str(), sessionName.c_str(), ret);
    }
}

int SoftBusWrapper::OpenSoftBusSession(const std::string &peerNetworkId, const std::string groupId) const
{
    std::string peerSessionName = mySessionName_;

    CLOGD("OpenSoftBusSession In, MySessionName = %{public}s, peerNetworkId = %{public}s, GroupId = %{public}s,"
        "attribute_.dataType = %{public}d, streamType = %{public}d.",
        mySessionName_.c_str(), peerNetworkId.c_str(), groupId.c_str(), attribute_.dataType,
        attribute_.attr.streamAttr.streamType);

    int sessionId = OpenSession(mySessionName_.c_str(), peerSessionName.c_str(), peerNetworkId.c_str(),
        groupId.c_str(), &attribute_);

    return sessionId;
}

void SoftBusWrapper::CloseSoftBusSession() const
{
    CLOGD("In, softBusSessionId_ = %{public}d.", softBusSessionId_);
    CloseSession(softBusSessionId_);

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessionIdToNameMap_.find(softBusSessionId_);
    if (it != sessionIdToNameMap_.end()) {
        sessionIdToNameMap_.erase(it);
    }
}

std::string SoftBusWrapper::GetSoftBusMySessionName(int sessionId)
{
    char cSessionName[MAX_SESSIONNAME_LEN] = {0};
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessionIdToNameMap_.find(sessionId);
        if (it != sessionIdToNameMap_.end()) {
            return it->second;
        }
    }
    int ret = GetMySessionName(sessionId, cSessionName, sizeof(cSessionName));
    if (ret == RET_OK) {
        return std::string(cSessionName);
    }
    CLOGE("OUT, ret = %{public}d, cSessionName = %{public}s, sessionId = %{public}d.", ret, cSessionName, sessionId);
    return std::string("");
}

std::string SoftBusWrapper::GetSoftBusPeerSessionName(int sessionId)
{
    char cSessionName[MAX_SESSIONNAME_LEN] = {0};
    int ret = GetPeerSessionName(sessionId, cSessionName, sizeof(cSessionName));
    if (ret == RET_OK) {
        return std::string(cSessionName);
    }
    CLOGE("OUT, ret = %{public}d, cSessionName = %{public}s, sessionId = %{public}d.", ret, cSessionName, sessionId);
    return std::string("");
}

std::string SoftBusWrapper::GetSoftBusPeerDeviceId(int sessionId)
{
    char cDeviceId[MAX_PEERDEVICEID_LEN] = {0};
    int ret = GetPeerDeviceId(sessionId, cDeviceId, sizeof(cDeviceId));
    if (ret == RET_OK) {
        return std::string(cDeviceId);
    }
    CLOGE("Out, ret = %{public}d, cDeviceId = %{public}s, sessionId = %{public}d.", ret, cDeviceId, sessionId);
    return std::string("");
}

int SoftBusWrapper::SendSoftBusBytes(const uint8_t *data, unsigned int len) const
{
    std::string sessionName = GetSoftBusPeerSessionName(softBusSessionId_);
    CLOGD("In, len = %{public}d, sessionName = %{public}s, softBusSessionId = %{public}d.", len, sessionName.c_str(),
        softBusSessionId_);
    return SendBytes(softBusSessionId_, data, len);
}

int SoftBusWrapper::SendSoftBusStream(const uint8_t *data, unsigned int len) const
{
    StreamData ext = {};
    StreamFrameInfo frameInfo = {};

    StreamData streamData = { reinterpret_cast<char *>(const_cast<uint8_t *>(data)), len };

    return SendStream(softBusSessionId_, &streamData, &ext, &frameInfo);
}

int SoftBusWrapper::SendSoftBusFiles(const char *sFileList[], const char *dFileList[], uint32_t fileCnt) const
{
    return SendFile(softBusSessionId_, sFileList, dFileList, fileCnt);
}

int SoftBusWrapper::GetSessionType()
{
    return sessionType_;
}

void SoftBusWrapper::SetSessionType(int sessionType)
{
    sessionType_ = sessionType;
}

void SoftBusWrapper::SetAttrbute(int dataType)
{
    CLOGD("SetAttrbute In, dataType = %{public}d.", dataType);

    attribute_.dataType = dataType;

    if (dataType == TYPE_STREAM) {
        attribute_.attr.streamAttr.streamType = COMMON_VIDEO_STREAM;
        CLOGD("SetAttrbute In, dataType = %{public}d, streamType = %{public}d.", attribute_.dataType,
            attribute_.attr.streamAttr.streamType);
    }
}

void SoftBusWrapper::SetSessionId(int sessionId)
{
    softBusSessionId_ = sessionId;

    std::lock_guard<std::mutex> lock(mutex_);
    sessionIdToNameMap_.insert(std::pair<int, std::string> { sessionId, mySessionName_ });
}

void SoftBusWrapper::SetMySessionName(const std::string &sessionName)
{
    mySessionName_ = sessionName;
}

void SoftBusWrapper::SetPeerSessionName(const std::string &sessionName)
{
    peerSessionName_ = sessionName;
}

void SoftBusWrapper::SetPeerDeviceId(const std::string &peerDeviceId)
{
    peerDeviceId_ = peerDeviceId;
}

void SoftBusWrapper::SetGroupId(const std::string &groupId)
{
    groupId_ = groupId;
}

std::string SoftBusWrapper::GetSpecMySessionName() const
{
    return mySessionName_;
}

std::string SoftBusWrapper::GetSpecGroupId() const
{
    return groupId_;
}

int SoftBusWrapper::GetSpecSessionId() const
{
    return softBusSessionId_;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS