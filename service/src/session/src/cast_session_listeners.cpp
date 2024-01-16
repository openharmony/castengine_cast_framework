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
 * Description: Cast Session implement realization.
 * Author: zhangge
 * Create: 2022-07-19
 */

#include "cast_engine_log.h"
#include "cast_session_impl.h"
#include "cast_engine_dfx.h"
#include "softbus_wrapper.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Session-Listeners");

namespace {

void RTSPWriteWrap(const std::string& funcName,
    ProtocolType protocolType,
    int sessionID)
{
    auto sceneType = GetBIZSceneType(static_cast<int>(protocolType));
    auto localSessName = SoftBusWrapper::GetSoftBusMySessionName(sessionID);
    auto peerSessName = SoftBusWrapper::GetSoftBusPeerSessionName(sessionID);

    if (protocolType == ProtocolType::COOPERATION) {
        HiSysEventWriteWrap(funcName, {
            {"BIZ_SCENE", static_cast<int32_t>(BIZSceneType::COOPERATION)},
            {"BIZ_STATE", static_cast<int32_t>(BIZStateType::BIZ_STATE_BEGIN)},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::COOPERATION_ESTABLISH_RTSP_CHANNEL)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_SUCCESS)},
            {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
            {"TO_CALL_PKG", DSOFTBUS_NAME},
            {"LOCAL_SESS_NAME", localSessName},
            {"PEER_SESS_NAME", peerSessName},
            {"PEER_UDID", ""}});
    } else {
        HiSysEventWriteWrap(funcName, {
            {"BIZ_SCENE", sceneType},
            {"BIZ_STAGE", static_cast<int32_t>(BIZSceneStage::ESTABLISH_RTSP_CHANNEL)},
            {"STAGE_RES", static_cast<int32_t>(StageResType::STAGE_RES_SUCCESS)},
            {"ERROR_CODE", CAST_RADAR_SUCCESS}}, {
            {"TO_CALL_PKG", DSOFTBUS_NAME},
            {"LOCAL_SESS_NAME", localSessName},
            {"PEER_SESS_NAME", peerSessName},
            {"PEER_UDID", ""}});
    }
}
}

void CastSessionImpl::RtspListenerImpl::OnSetup(const ParamInfo &param, int mediaPort, int remoteControlPort,
    const std::string &deviceId)
{
    CLOGD("Setup in, media port:%d, remote control port:%d, device id:%s", mediaPort, remoteControlPort,
        deviceId.c_str());

    auto session = session_.promote();
    if (!session) {
        CLOGE("Setup in, session is null");
        return;
    }

    session->rtspParamInfo_ = param;
    if (session->IsStreamMode()) {
        session->property_.protocolType = ProtocolType::CAST_PLUS_STREAM;
    }
    session->SendCastMessage(Message(MessageId::MSG_SETUP, mediaPort, remoteControlPort, NO_DELAY, deviceId));
}

bool CastSessionImpl::RtspListenerImpl::OnPlay(const ParamInfo &param, int port, const std::string &deviceId)
{
    CLOGD("Play in, port: %d", port);
    auto session = session_.promote();
    if (!session) {
        CLOGE("Play in, session is null");
        return false;
    }

    session->SendCastMessage(Message(MessageId::MSG_PLAY_REQ, port, UNUSED_VALUE, NO_DELAY, deviceId));
    return true;
}

bool CastSessionImpl::RtspListenerImpl::OnPause()
{
    CLOGD("Pause in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("Pause in, session is null");
        return false;
    }

    session->SendCastMessage(MessageId::MSG_PAUSE_REQ, UNUSED_VALUE, UNUSED_VALUE);
    return true;
}

void CastSessionImpl::RtspListenerImpl::OnTearDown()
{
    CLOGD("Tear down in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("Tear down in, session is null");
        return;
    }

    session->SendCastMessage(
        Message(MessageId::MSG_ERROR, ERR_CODE, MODULE_TYPE_STRING[static_cast<int>(ModuleType::RTSP)]));
}

void CastSessionImpl::RtspListenerImpl::OnError(int errCode)
{
    CLOGD("On error");
    auto session = session_.promote();
    if (!session) {
        CLOGE("Session is null");
        return;
    }
    session->SendCastMessage(
        Message(MessageId::MSG_ERROR, errCode, MODULE_TYPE_STRING[static_cast<int>(ModuleType::RTSP)]));
}

void CastSessionImpl::RtspListenerImpl::NotifyTrigger(int trigger)
{
    CLOGD("Trigger in: %d", trigger);
    auto session = session_.promote();
    if (!session) {
        CLOGE("Session is null");
        return;
    }

    session->SendCastMessage(MessageId::MSG_PROCESS_TRIGGER_REQ, trigger, UNUSED_VALUE);
}

void CastSessionImpl::RtspListenerImpl::NotifyEventChange(int moduleId, int event, const std::string &param)
{
    CLOGD("in, moduleId:%{public}d, event:%{public}d, param: %s", moduleId, event, param.c_str());
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return;
    }
    session->ProcessRtspEvent(moduleId, event, param);
}

void CastSessionImpl::ConnectManagerListenerImpl::NotifySessionEvent(const std::string &deviceId, int result)
{
    CLOGD("NotifySessionEvent in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return;
    }
    switch (result) {
        case ConnectEvent::AUTH_START:
            session->SendCastMessage(Message(MessageId::MSG_AUTH, deviceId));
            break;
        case ConnectEvent::AUTH_SUCCESS:
            session->SendCastMessage(Message(MessageId::MSG_CONNECT, deviceId));
            break;
        case ConnectEvent::CONNECT_START:
            session->SendCastMessage(Message(MessageId::MSG_CONNECT, deviceId));
            break;
        case ConnectEvent::DISCONNECT_START:
            session->SendCastMessage(Message(MessageId::MSG_DISCONNECT, deviceId));
            break;
        default:
            CLOGW("unsupported result: %d", result);
            break;
    }
}

bool CastSessionImpl::CastStreamListenerImpl::SendActionToPeers(int action, const std::string &param)
{
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return false;
    }
    return session->SendCastMessage(Message(MessageId::MSG_STREAM_SEND_ACTION_EVENT_TO_PEERS, action, param));
}

CastSessionImpl::CastStreamListenerImpl::~CastStreamListenerImpl()
{
    CLOGD("~CastStreamListenerImpl");
}

void CastSessionImpl::CastStreamListenerImpl::OnRenderReady(bool isReady)
{
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return;
    }
    session->SendCastMessage(Message(MessageId::MSG_PEER_RENDER_READY, isReady ? RENDER_READY : NOT_RENDER_READY));
}

void CastSessionImpl::CastStreamListenerImpl::OnEvent(EventId eventId, const std::string &data)
{
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return;
    }
    session->OnEvent(eventId, data);
}

int CastSessionImpl::RtspListenerImpl::StartMediaVtp(const ParamInfo &param)
{
    return INVALID_PORT;
}

void CastSessionImpl::RtspListenerImpl::ProcessStreamMode(const ParamInfo &param, const std::string &deviceId)
{
    auto session = session_.promote();
    if (!session) {
        CLOGE("ProcessStreamMode, session is null");
        return;
    }
    session->rtspParamInfo_ = param;
    session->SendCastMessage(Message(MessageId::MSG_PEER_RENDER_READY, deviceId));
    return;
}

void CastSessionImpl::RtspListenerImpl::NotifyModuleCustomParamsNegotiation(const std::string &mediaParams,
    const std::string &controllerParams)
{
    auto session = session_.promote();
    if (!session) {
        CLOGE("Session_ is null.");
        return;
    }

    if (session->property_.endType == EndType::CAST_SOURCE) {
        session->rtspControl_->ModuleCustomParamsNegotiationDone();
        std::string negotiationParams = session->streamManager_->HandleCustomNegotiationParams(controllerParams);
        session->rtspControl_->SetNegotiatedPlayerControllerCapability(negotiationParams);
    }
}

bool CastSessionImpl::RtspListenerImpl::NotifyEvent(int event)
{
    return true;
}

bool CastSessionImpl::RtspListenerImpl::OnPlayerReady(const ParamInfo &clientParam, const std::string &deviceId,
    int readyFlag)
{
    CLOGD("OnPlayerReady in readyFlag:%{public}d", readyFlag);
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return false;
    }
    if (readyFlag != RENDER_READY) {
        CLOGE("player is not ready");
        session->SendCastMessage(Message(MessageId::MSG_ERROR));
        return false;
    }
    session->SendCastMessage(Message(MessageId::MSG_PEER_RENDER_READY, readyFlag, deviceId));
    return true;
}

void CastSessionImpl::ChannelManagerListenerImpl::OnChannelCreated(std::shared_ptr<Channel> channel)
{
    CLOGD("Channel created event in");
    auto session = session_.promote();
    if (channel == nullptr || session == nullptr) {
        CLOGE("Channel or session is null");
        return;
    }

    auto remoteDeviceId = channel->GetRequest().remoteDeviceInfo.deviceId;
    auto deviceInfo = session->FindRemoteDevice(remoteDeviceId);
    if (deviceInfo == nullptr) {
        CLOGE("Remote device is null");
        return;
    }

    auto sessionID = channel->GetRequest().remoteDeviceInfo.sessionId;

    auto streamManager = session->StreamManagerGetter();
    ModuleType moduleType = channel->GetRequest().moduleType;
    switch (moduleType) {
        case ModuleType::RTSP:
            session->rtspControl_->AddChannel(channel, deviceInfo->remoteDevice);

            RTSPWriteWrap(__func__, session->property_.protocolType, sessionID);

            break;
        case ModuleType::STREAM:
            if (streamManager) {
                streamManager->AddChannel(channel);
            }
            break;
        default:
            break;
    }
}

void CastSessionImpl::ChannelManagerListenerImpl::OnChannelOpenFailed(ChannelRequest &channelRequest,
    const int errorCode)
{
    CLOGD("Channel open faied event in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("Session is null");
        return;
    }

    session->SendCastMessage(
        Message(MessageId::MSG_ERROR, errorCode, MODULE_TYPE_STRING[static_cast<int>(channelRequest.moduleType)]));
}

void CastSessionImpl::ChannelManagerListenerImpl::OnChannelError(std::shared_ptr<Channel> channel, const int errorCode)
{
    CLOGD("Channel error in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is null");
        return;
    }

    session->SendCastMessage(Message(MessageId::MSG_ERROR, errorCode,
        MODULE_TYPE_STRING[static_cast<int>(channel->GetRequest().moduleType)]));
}

void CastSessionImpl::ChannelManagerListenerImpl::OnChannelRemoved(std::shared_ptr<Channel> channel)
{
    CLOGD("Channel removed event in");
    auto session = session_.promote();
    if (!channel || !session) {
        CLOGE("Channel or session is null");
        return;
    }

    auto streamManager = session->StreamManagerGetter();
    ModuleType moduleType = channel->GetRequest().moduleType;
    switch (moduleType) {
        case ModuleType::RTSP:
            session->rtspControl_->RemoveChannel(channel);
            break;
        case ModuleType::STREAM:
            if (streamManager) {
                streamManager->RemoveChannel(channel);
            }
            break;
        default:
            CLOGW("Invalid module type:%{public}d", static_cast<int>(moduleType));
            break;
    }

    if (session->IsStreamMode() && session->IsSink()) {
        CLOGI("Support to continue playing resources after channel disconnected accidently");
        session->OnEvent(EventId::STEAM_DEVICE_DISCONNECTED, "Connection is disconnected unexpectedly.");
        return;
    }
    session->SendCastMessage(MessageId::MSG_ERROR);
    return;
}

void CastSessionImpl::ChannelManagerListenerImpl::SetMediaChannel(ModuleType moduleType)
{
    if (moduleType == ModuleType::AUDIO) {
        mediaChannelState_ |= AUDIO_CHANNEL_CONNECTED;
        return;
    }

    if (moduleType == ModuleType::VIDEO) {
        mediaChannelState_ |= VIDEO_CHANNEL_CONNECTED;
    }
    CLOGI("SetMediaChannel mediaChannelState_ is %d, moduleType is %d", mediaChannelState_, moduleType);
}

bool CastSessionImpl::ChannelManagerListenerImpl::IsMediaChannelReady()
{
    auto session = session_.promote();
    if (!session) {
        CLOGE("Session is null");
        return false;
    }
    
    CLOGI("protocolType %d", session->property_.protocolType);
    if (session->property_.protocolType == ProtocolType::CAST_PLUS_MIRROR ||
        session->property_.protocolType == ProtocolType::CAST_PLUS_STREAM ||
        session->property_.protocolType == ProtocolType::CAST_COOPERATION) {
        CLOGI("mediaState %d, both %d", mediaChannelState_, VIDEO_CHANNEL_CONNECTED | AUDIO_CHANNEL_CONNECTED);
        return mediaChannelState_ == (VIDEO_CHANNEL_CONNECTED | AUDIO_CHANNEL_CONNECTED);
    }

    CLOGI("Only Cast Video.");
    return mediaChannelState_ == VIDEO_CHANNEL_CONNECTED;
}

} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS