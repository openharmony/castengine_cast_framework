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
 * Description: softbus connection.
 * Author: sunhong
 * Create: 2022-01-22
 */

#ifndef SOFTBUSCONNECTION_H
#define SOFTBUSCONNECTION_H

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "../../include/connection.h"
#include "softbus_wrapper.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class SoftBusConnection : public Connection, public Channel, public std::enable_shared_from_this<SoftBusConnection> {
public:
    using Connection::channelRequest_;

    SoftBusConnection();
    ~SoftBusConnection() override;

    static std::pair<bool, std::shared_ptr<SoftBusConnection>> GetConnection(int sessionId);
    int StartConnection(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener) override;
    int StartListen(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener) override;
    void CloseConnection() override;
    bool Send(const uint8_t *buf, int bufLen) override;
    SoftBusWrapper &GetSoftBus();
    bool GetActivelyOpenFlag() const;
    void SetActivelyOpenFlag(bool isActivelyOpen);
    bool GetPassiveCloseFlag() const;
    void SetPassiveCloseFlag(bool isPassiveClose);
    static std::unordered_map<std::string, std::shared_ptr<SoftBusConnection>> connectionMap_;
    static std::mutex connectionMapMtx_;
    static IFileSendListener fileSendListener_;
    static IFileReceiveListener fileReceiveListener_;
private:
    static int OnConnectionSessionOpened(int sessionId, int result);
    static void OnConnectionSessionClosed(int sessionId);
    static void OnConnectionMessageReceived(int sessionId, const void *data, unsigned int dataLen);
    static void OnConnectionBytesReceived(int sessionId, const void *data, unsigned int dataLen);
    static void OnConnectionStreamReceived(int sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param);
    static void OnConnectionSessionEvent(int sessionId, int eventId, int tvCount, const QosTv *tvList);

    // Softbus file send callback function
    static int OnSendFileProcess(int sessionId, uint64_t bytesUpload, uint64_t bytesTotal);
    static int OnSendFileFinished(int sessionId, const char *firstFile);
    static void OnFileTransError(int sessionId);

    // Softbus file recv callback function
    static int OnReceiveFileStarted(int sessionId, const char *files, int fileCnt);
    static int OnReceiveFileProcess(int sessionId, const char *firstFile, uint64_t bytesUpload, uint64_t bytesTotal);
    static void OnReceiveFileFinished(int sessionId, const char *files, int fileCnt);

    int SetupSession(std::shared_ptr<IChannelListener> channelListener, std::shared_ptr<SoftBusConnection> hold);
    void StashConnectionInfo(const ChannelRequest &request);
    int GetSessionType(ModuleType moduleType) const;
    std::string CreateSessionName(ModuleType moduleType, int sessionId);

    static const std::string PACKAGE_NAME;
    static const std::string AUTH_SESSION_NAME_FACTOR;
    static const std::string RTSP_SESSION_NAME_FACTOR;
    static const std::string RTCP_SESSION_NAME_FACTOR;
    static const std::string VIDEO_SESSION_NAME_FACTOR;
    static const std::string AUDIO_SESSION_NAME_FACTOR;
    static const std::string CONTROL_SESSION_NAME_FACTOR;
    static const std::string STREAM_SESSION_NAME_FACTOR;
    static const std::string FILES_SESSION_NAME_FACTOR;
    static const std::string BYTES_SESSION_NAME_FACTOR;
    static const std::string SESSION_NAME_PREFIX;
    static const int RET_ERR = -1;
    static const int RET_OK = 0;

    bool isActivelyOpen_;
    bool isPassiveClose_;
    ISessionListener sessionListener_ = { OnConnectionSessionOpened, OnConnectionSessionClosed,
        OnConnectionBytesReceived, OnConnectionMessageReceived, OnConnectionStreamReceived, OnConnectionSessionEvent };
    SoftBusWrapper softbus_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
