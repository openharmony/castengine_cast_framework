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
 * Create: 2022-01-28
 */

#include "rtsp_channel_manager.h"

#include "cast_engine_log.h"
#include "encrypt_decrypt.h"
#include "rtsp_basetype.h"
#include "securec.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
DEFINE_CAST_ENGINE_LABEL("Cast-Rtsp-Net-Manager");

void RtspChannelManager::ChannelListener::OnDataReceived(const uint8_t *buffer, unsigned int length, long timeCost)
{
    CLOGD("==============Received data length %{public}u timeCost %{public}ld================", length, timeCost);

    if (!((channelManager_->algorithmId_ > 0) &&
        !Utils::IsArrayAllZero(channelManager_->sessionKeys_, SESSION_KEY_LENGTH))) {
        CLOGD("==============Not Authed Recv Msg ================");
        CLOGD("Algorithm id %{public}d, length %{public}u.", channelManager_->algorithmId_, length);
        channelManager_->OnData(buffer, length);
    } else {
        unsigned int realPktlen = length - EncryptDecrypt::AES_IV_LEN;
        std::unique_ptr<uint8_t[]> decryContent = std::make_unique<uint8_t[]>(realPktlen);
        PacketData outputData = { decryContent.get(), 0 };
        bool isSucc =
            EncryptDecrypt::GetInstance().DecryptData(channelManager_->algorithmId_, channelManager_->sessionKeys_,
            channelManager_->sessionKeyLength_, { buffer, static_cast<int>(length) }, outputData);
        if (!isSucc) {
            CLOGE("ERROR: decode fail or len [%{public}d],expect[%{public}u]", outputData.length, length);
            return;
        }
        CLOGD("==============Authed Recv Msg ================, decryContent length %{public}u", length);
        channelManager_->OnData(decryContent.get(), realPktlen);
    }
}

RtspChannelManager::RtspChannelManager(RtspListenerInner *listener, ProtocolType protocolType)
    : listener_(listener), protocolType_(protocolType)
{
    channelListener_ = std::make_shared<ChannelListener>(this);
    CLOGI("Out, ProtocolType:%{public}d", protocolType_);
}

RtspChannelManager::~RtspChannelManager()
{
    CLOGI("In.");
    memset_s(sessionKeys_, SESSION_KEY_LENGTH, 0, SESSION_KEY_LENGTH);
    channelListener_ = nullptr;
    StopSafty(false);
    ThreadJoin();
}

std::shared_ptr<IChannelListener> RtspChannelManager::GetChannelListener()
{
    return channelListener_;
}

void RtspChannelManager::AddChannel(std::shared_ptr<Channel> channel, const CastInnerRemoteDevice &device)
{
    if (channel_ != nullptr) {
        CLOGE("channel exists!");
    }
    channel_ = channel;

    bool isSoftbus = channel->GetRequest().linkType == ChannelLinkType::SOFT_BUS;
    CLOGD("LinkType %{public}d listener_ is %{public}d", isSoftbus, listener_ == nullptr);
    listener_->OnPeerReady(isSoftbus);
}

void RtspChannelManager::RemoveChannel(std::shared_ptr<Channel> channel)
{
    channel_ = nullptr;
}

void RtspChannelManager::StartSession(const uint8_t *sessionKey, uint32_t sessionKeyLength)
{
    isSessionActive_ = true;
    if (memcpy_s(sessionKeys_, sessionKeyLength, sessionKey, sessionKeyLength) != 0) {
        CLOGE("SessionKey Copy Error!");
    }
    sessionKeyLength_ = sessionKeyLength;
}

void RtspChannelManager::StopSession()
{
    CLOGD("Stop session.");
    if (isSessionActive_) {
        memset_s(sessionKeys_, SESSION_KEY_LENGTH, 0, SESSION_KEY_LENGTH);
        isSessionActive_ = false;
        listener_->OnPeerGone();
    }
    RemoveMessage(Message(static_cast<int>(RtspState::MSG_NEG_TIMEOUT)));
}

void RtspChannelManager::OnConnected(ChannelLinkType channelLinkType)
{
    if (listener_ == nullptr) {
        CLOGE("listener is null.");
        return;
    }
    bool isSoftbus = channelLinkType == ChannelLinkType::SOFT_BUS;
    CLOGI("IsSoftbus %{public}d.", isSoftbus);
    listener_->OnPeerReady(isSoftbus);
}

void RtspChannelManager::OnData(const uint8_t *data, unsigned int length)
{
    std::string str(reinterpret_cast<const char *>(data), length);
    CLOGD("In, %{public}s %{public}s", (str.find("RTSP/") == 0) ? "Response...\r\n" : "Request...\r\n", str.c_str());
    if (listener_ == nullptr) {
        CLOGE("listener is null.");
        return;
    }
    RtspParse msg;
    RtspParse::ParseMsg(str, msg);
    if (Utils::StartWith(str, "RTSP/")) {
        listener_->OnResponse(msg);
    } else {
        listener_->OnRequest(msg);
    }
}

void RtspChannelManager::OnError(const std::string &errorCode)
{
    CLOGI("In, %{public}s.", errorCode.c_str());
    listener_->OnPeerGone();
}

void RtspChannelManager::OnClosed(const std::string &errorCode)
{
    CLOGI("OnClosed %{public}s.", errorCode.c_str());
    listener_->OnPeerGone();
}

bool RtspChannelManager::SendData(const std::string &dataFrame)
{
    auto channel = channel_;
    if (channel == nullptr) {
        CLOGE("SendData, channel is nullptr.");
        return false;
    }
    size_t pktlen = dataFrame.size();
    std::unique_ptr<uint8_t[]> encryptContent = std::make_unique<uint8_t[]>(pktlen + EncryptDecrypt::AES_IV_LEN);
    PacketData outputData = { encryptContent.get(), 0 };
    if (channel->GetRequest().linkType == ChannelLinkType::SOFT_BUS ||
        Utils::IsArrayAllZero(sessionKeys_, SESSION_KEY_LENGTH) || algorithmId_ <= 0) {
        errno_t ret = memcpy_s(encryptContent.get(), pktlen + EncryptDecrypt::AES_IV_LEN, dataFrame.c_str(), pktlen);
        if (ret != EOK) {
            CLOGE("ERROR: memory copy error:%{public}d", ret);
            return false;
        }

        outputData.length = static_cast<int>(pktlen);
        CLOGD("SendData, get data finish.");
    } else {
        bool ret = EncryptDecrypt::GetInstance().EncryptData(algorithmId_, sessionKeys_, sessionKeyLength_,
            { reinterpret_cast<const uint8_t *>(dataFrame.c_str()), pktlen }, outputData);
        if (!ret || (outputData.length != static_cast<int>(pktlen) + static_cast<int>(EncryptDecrypt::AES_IV_LEN))) {
            CLOGE("Encrypt data failed, dataLength: %{public}d, pktlen: %{public}zu", outputData.length, pktlen);
            return false;
        }
        CLOGD("SendData, encrypt data finish.");
    }

    CLOGD("SendData, outputData.length %{public}d pktlen %{public}zu send buffer %{public}s.", outputData.length,
        pktlen, encryptContent.get());
    return channel->Send(encryptContent.get(), outputData.length);
}

bool RtspChannelManager::SendRtspData(const std::string &request)
{
    CLOGD("Send rtsp data.");
    if (request.empty()) {
        CLOGE("request string is null");
        return false;
    }

    if (!isSessionActive_) {
        CLOGE("IsSessionActive_ %{public}d SendRtspData... %{public}zu", isSessionActive_, request.length());
        return false;
    }

    return SendData(request);
}

void RtspChannelManager::CfgNegTimeout(bool isClear)
{
    CLOGI("In, %{public}d.", isClear);
    if (isClear) {
        RemoveMessage(Message(static_cast<int>(RtspState::MSG_NEG_TIMEOUT)));
        return;
    }

    SendCastMessageDelayed(static_cast<int>(RtspState::MSG_NEG_TIMEOUT), KEEP_NEG_TIMEOUT_INTERVAL); // 10s
}

void RtspChannelManager::SetNegAlgorithmId(int algorithmId)
{
    algorithmId_ = algorithmId;
    CLOGI("SetNegAlgorithmId algorithmId %{public}d.", algorithmId);
}

void RtspChannelManager::HandleMessage(const Message &msg)
{
    switch (static_cast<RtspState>(msg.what_)) {
        case RtspState::MSG_RTSP_START:
        case RtspState::MSG_RTSP_DATA:
        case RtspState::MSG_RTSP_CLOSE:
        case RtspState::MSG_SEND_KA:
        case RtspState::MSG_KA_TIMEOUT:
        case RtspState::MSG_NEG_TIMEOUT:
            CLOGE("NEG timeout.");
            if (listener_ != nullptr) {
                listener_->OnPeerGone();
            }
            break;
        default:
            break;
    }
}
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS