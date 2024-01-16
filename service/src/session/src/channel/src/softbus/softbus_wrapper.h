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

#ifndef D_SOFTBUS_WRAPPER_H
#define D_SOFTBUS_WRAPPER_H

#include <map>
#include <mutex>
#include <string>
#include "session.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class SoftBusWrapper {
public:
    SoftBusWrapper();
    ~SoftBusWrapper();

    static std::string GetSoftBusMySessionName(int sessionId);
    static std::string GetSoftBusPeerSessionName(int sessionId);
    static std::string GetSoftBusPeerDeviceId(int sessionId);
    static int StartSoftBusService(const std::string pkgName, const std::string &sessionName,
        ISessionListener *listener);
    static void StopService(const std::string pkgName, const std::string &sessionName);
    int OpenSoftBusSession(const std::string &peerNetworkId, const std::string groupId) const;
    void CloseSoftBusSession() const;
    int SendSoftBusBytes(const uint8_t *data, unsigned int len) const;
    int SendSoftBusStream(const uint8_t *data, unsigned int len) const;
    int SendSoftBusFiles(const char *sFileList[], const char *dFileList[], uint32_t fileCnt) const;
    int GetSessionType();
    void SetSessionType(int sessionType);
    void SetAttrbute(int dataType);
    void SetSessionId(int sessionId);
    void SetMySessionName(const std::string &sessionName);
    void SetPeerSessionName(const std::string &sessionName);
    void SetPeerDeviceId(const std::string &peerDeviceId);
    void SetGroupId(const std::string &groupId);
    std::string GetSpecMySessionName() const;
    std::string GetSpecGroupId() const;
    int GetSpecSessionId() const;

private:
    static constexpr int RET_ERR = -1;
    static constexpr int RET_OK = 0;
    static constexpr int MAX_LINK_TYPE_NUM = 3;
    static constexpr unsigned int MAX_SESSIONNAME_LEN = 256;
    static constexpr unsigned int MAX_PEERDEVICEID_LEN = 64;
    static std::mutex mutex_;
    static std::map<int, std::string> sessionIdToNameMap_;

    int sessionType_{ TYPE_BYTES };
    int softBusSessionId_{ 0 };
    std::string mySessionName_;
    std::string peerSessionName_;
    std::string peerDeviceId_;
    std::string groupId_;
    SessionAttribute attribute_{};
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif