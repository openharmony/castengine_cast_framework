/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Cast Session implement realization.
 * Author: lijianzhao
 * Create: 2022-01-25
 */

#include "cast_session_impl.h"

#include <array>

#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "cast_device_data_manager.h"
#include "connection_manager.h"
#include "ipc_skeleton.h"
#include "json/json.h"
#include "permission.h"
#include "cast_stream_manager_client.h"
#include "cast_stream_manager_server.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-SessionImpl");

using CastSessionRtsp::ActionType;
using CastSessionRtsp::DeviceTypeParamInfo;
using CastSessionRtsp::VtpType;

const std::array<std::string, static_cast<size_t>(CastSessionImpl::MessageId::MSG_ID_MAX)>
    CastSessionImpl::MESSAGE_ID_STRING = {
        "MSG_CONNECT",
        "MSG_SETUP",
        "MSG_SETUP_SUCCESS",
        "MSG_SETUP_FAILED",
        "MSG_SETUP_DONE",
        "MSG_PLAY",
        "MSG_PAUSE",
        "MSG_PLAY_REQ",
        "MSG_PAUSE_REQ",
        "MSG_DISCONNECT",
        "MSG_CONNECT_TIMEOUT",
        "MSG_PROCESS_TRIGGER_REQ",
        "MSG_UPDATE_VIDEO_SIZE",
        "MSG_STREAM_RECV_ACTION_EVENT_FROM_PEERS",
        "MSG_STREAM_SEND_ACTION_EVENT_TO_PEERS",
        "MSG_PEER_RENDER_READY",
        "MSG_ERROR",
        "MSG_SET_CAST_MODE",
        "MSG_READY_TO_PLAYING",
    };
std::atomic<int> CastSessionImpl::idCount_ = rand();

CastSessionImpl::CastSessionImpl(const CastSessionProperty &property, const CastLocalDevice &localDevice)
    : localDevice_(localDevice), property_(property)
{
    CLOGD("In");
}

CastSessionImpl::~CastSessionImpl()
{
    CLOGD("~CastSessionImpl in");
    StopSafty(false);
    ThreadJoin();
    CLOGD("~CastSessionImpl out");
}

void CastSessionImpl::SetServiceCallbackForRelease(const std::function<void(int)> &callback)
{
    std::lock_guard<std::mutex> lock(serviceCallbackMutex_);
    serviceCallback_ = callback;
}

int32_t CastSessionImpl::RegisterListener(sptr<ICastSessionListenerImpl> listener)
{
    CLOGD("Start to register session listener");
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    listeners_[IPCSkeleton::GetCallingPid()] = listener;
    cond_.notify_all();
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::UnregisterListener()
{
    CLOGD("In");
    std::unique_lock<std::mutex> lock(mutex_);
    listeners_.erase(IPCSkeleton::GetCallingPid());
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::AddDevice(const CastRemoteDevice &remoteDevice)
{
    CLOGI("sessionId_ %{public}d", sessionId_);
    CastInnerRemoteDevice remote = {
        .deviceId = remoteDevice.deviceId,
        .deviceName = remoteDevice.deviceName,
        .deviceType = remoteDevice.deviceType,
        .subDeviceType = remoteDevice.subDeviceType,
        .ipAddress = remoteDevice.ipAddress,
        .sessionId = sessionId_,
        .channelType = remoteDevice.channelType,
    };
    if (!AddDevice(remote)) {
        return CAST_ENGINE_ERROR;
    }
    if (!ConnectionManager::GetInstance().ConnectDevice(remote)) {
        return CAST_ENGINE_ERROR;
    }
    Permission::SaveMirrorAppInfo();
    return CAST_ENGINE_SUCCESS;
}

bool CastSessionImpl::AddDevice(const CastInnerRemoteDevice &remoteDevice)
{
    if (!Permission::CheckPidPermission()) {
        return false;
    }

    CLOGI("In: session state: %{public}s", SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (listeners_.empty()) {
            int timeout = 5; // unit: s
            sptr<CastSessionImpl> self(this);
            bool result =
                cond_.wait_for(lock, std::chrono::seconds(timeout), [self] { return !self->listeners_.empty(); });
            if (!result) {
                CLOGE("Wait the listener timeout");
                return false;
            }
        }
    }
    if (!CastDeviceDataManager::GetInstance().UpdateDevice(remoteDevice)) {
        return false;
    }

    if (!AddRemoteDevice(CastRemoteDeviceInfo { remoteDevice, DeviceState::DISCONNECTED })) {
        return false;
    }

    SendCastMessage(Message(MessageId::MSG_CONNECT, remoteDevice.deviceId));
    return true;
}

int32_t CastSessionImpl::RemoveDevice(const std::string &deviceId)
{
    CLOGI("In: session state: %{public}s", SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }

    SendCastMessage(Message(MessageId::MSG_DISCONNECT, deviceId));
    return CAST_ENGINE_SUCCESS;
}

bool CastSessionImpl::ReleaseSessionResources(pid_t pid)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.erase(pid);
        if (!listeners_.empty()) {
            return false;
        }
        for (auto &deviceInfo : remoteDeviceList_) {
            RemoveDevice(deviceInfo.remoteDevice.deviceId);
        }
    }
    Stop();
    return true;
}

int32_t CastSessionImpl::CreateMirrorPlayer(sptr<IMirrorPlayerImpl> &mirrorPlayer)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    auto player = MirrorPlayerGetter();
    mirrorPlayer = player;
    return CAST_ENGINE_SUCCESS;
}

bool CastSessionImpl::DestroyMirrorPlayer()
{
    std::unique_lock<std::mutex> lock(mirrorMutex_);
    mirrorPlayer_ = nullptr;
    return true;
}

int32_t CastSessionImpl::CreateStreamPlayer(sptr<IStreamPlayerIpc> &streamPlayer)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    auto streamManager = CreateStreamPlayerManager();
    if (!streamManager) {
        CLOGE("streamManager is null");
        return CAST_ENGINE_ERROR;
    }
    streamPlayer = streamManager->CreateStreamPlayer([this]() { DestroyStreamPlayer(); });
    return CAST_ENGINE_SUCCESS;
}

std::shared_ptr<ICastStreamManager> CastSessionImpl::CreateStreamPlayerManager()
{
    std::unique_lock<std::mutex> lock(streamMutex_);
    if (!streamManager_) {
        if (property_.endType == EndType::CAST_SOURCE) {
            streamManager_ = std::make_shared<CastStreamManagerClient>(std::make_shared<CastStreamListenerImpl>(this));
        } else {
            streamManager_ = std::make_shared<CastStreamManagerServer>(std::make_shared<CastStreamListenerImpl>(this));
        }
    }
    return streamManager_;
}

bool CastSessionImpl::DestroyStreamPlayer()
{
    std::unique_lock<std::mutex> lock(streamMutex_);
    streamManager_ = nullptr;
    return true;
}

int32_t CastSessionImpl::Release()
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto &deviceInfo : remoteDeviceList_) {
            RemoveDevice(deviceInfo.remoteDevice.deviceId);
        }
    }
    std::lock_guard<std::mutex> lock(serviceCallbackMutex_);
    if (!serviceCallback_) {
        CLOGE("serviceCallback is null");
        return CAST_ENGINE_ERROR;
    }
    serviceCallback_(sessionId_);
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::StartAuth(const AuthInfo &authInfo)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    static_cast<void>(authInfo);
    return CAST_ENGINE_ERROR;
}

int32_t CastSessionImpl::GetSessionId(std::string &sessionId)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    sessionId = std::to_string(sessionId_);
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::Play(const std::string &deviceId)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    CLOGI("Session state: %{public}s", SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());
    SendCastMessage(Message(MessageId::MSG_PLAY, deviceId));
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::Pause(const std::string &deviceId)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    CLOGI("Session state: %{public}s", SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());
    SendCastMessage(Message(MessageId::MSG_PAUSE, deviceId));
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::GetDeviceState(const std::string &deviceId, DeviceState &deviceState)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    auto deviceInfo = FindRemoteDevice(deviceId);
    DeviceState state = DeviceState::DISCONNECTED;
    if (deviceInfo != nullptr) {
        state = deviceInfo->deviceState;
    }
    CLOGI("device state: %{public}s", DEVICE_STATE_STRING[static_cast<int>(state)].c_str());
    deviceState = state;
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::SetSessionProperty(const CastSessionProperty &property)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }

    if (property_.protocolType != property.protocolType || property_.endType != property.endType) {
        CLOGE("Wrong protocol type:%d or end type:%d", property.protocolType, property.endType);
        return CAST_ENGINE_ERROR;
    }
    property_ = property;

    CLOGD("video: width-%{public}u, height-%{public}u, fps-%{public}u; audio: sample_rate-%{public}u, "
        "channels-%{public}u ",
        property.videoProperty.videoWidth, property.videoProperty.videoHeight, property.videoProperty.fps,
        property.audioProperty.sampleRate, property.audioProperty.channelConfig);
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::SetSurface(sptr<IBufferProducer> producer)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    CLOGD("SetSurface in");
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::DeliverInputEvent(const OHRemoteControlEvent &event)
{
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    CLOGD("DeliverInputEvent in.");
    if (sessionState_ != SessionState::PLAYING) {
        CLOGE("DeliverInputEvent failed, not playing.");
        return ERR_SESSION_STATE_NOT_MATCH;
    }
    return CAST_ENGINE_SUCCESS;
}

void CastSessionImpl::SetLocalDevice(const CastLocalDevice &localDevice)
{
    localDevice_ = localDevice;
}

bool CastSessionImpl::TransferTo(std::shared_ptr<BaseState> state)
{
    if (IsAllowTransferState(state->GetStateId())) {
        CLOGD("Transfer to %{public}s", SESSION_STATE_STRING[static_cast<int>(state->GetStateId())].c_str());
        TransferState(state);
        return true;
    }
    return false;
}

bool CastSessionImpl::Init()
{
    CLOGI("Session state: %{public}s", SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());

    srand(static_cast<unsigned int >(time(nullptr)));
    sessionId_ = rand();
    channelManagerListener_ = std::make_shared<ChannelManagerListenerImpl>(this);
    channelManager_ = std::make_shared<ChannelManager>(sessionId_, channelManagerListener_);

    rtspListener_ = std::make_shared<RtspListenerImpl>(this);
    rtspControl_ = IRtspController::GetInstance(rtspListener_, property_.protocolType, property_.endType);

    defaultState_ = std::make_shared<DefaultState>(SessionState::DEFAULT, this, nullptr);
    disconnectedState_ = std::make_shared<DisconnectedState>(SessionState::DISCONNECTED, this, defaultState_);
    connectingState_ = std::make_shared<ConnectingState>(SessionState::CONNECTING, this, defaultState_);
    connectedState_ = std::make_shared<ConnectedState>(SessionState::CONNECTED, this, defaultState_);
    disconnectingState_ = std::make_shared<DisconnectingState>(SessionState::DISCONNECTING, this, defaultState_);
    pausedState_ = std::make_shared<PausedState>(SessionState::PAUSED, this, connectedState_);
    playingState_ = std::make_shared<PlayingState>(SessionState::PLAYING, this, connectedState_);
    streamState_ = std::make_shared<StreamState>(SessionState::STREAM, this, connectedState_);
    TransferTo(disconnectedState_);

    return true;
}

void CastSessionImpl::Stop()
{
    CLOGD("Start to stop session");
    // remove msg connect timeout to prevent waiting too long for the thread to stop
    RemoveMessage(Message(static_cast<int>(MessageId::MSG_CONNECT_TIMEOUT)));
    StopSafty(true);
    ThreadJoin();
    CLOGD("End to stop session");
}

void CastSessionImpl::InitRtspParamInfo(std::shared_ptr<CastRemoteDeviceInfo> remoteDeviceInfo)
{
    rtspParamInfo_.SetVideoProperty(property_.videoProperty);
    rtspParamInfo_.SetVersion(CAST_VERSION);
    rtspParamInfo_.SetSupportVtpOpt((remoteDeviceInfo->remoteDevice.channelType != ChannelType::SOFT_BUS) ?
        VtpType::VTP_SUPPORT_VIDEO :
        VtpType::VTP_NOT_SUPPORT_VIDEO);
    if (property_.protocolType == ProtocolType::CAST_PLUS_MIRROR ||
        property_.protocolType == ProtocolType::CAST_PLUS_STREAM) {
        rtspParamInfo_.SetAudioProperty(property_.audioProperty);
    }

    if (property_.protocolType == ProtocolType::CAST_PLUS_STREAM) {
        rtspParamInfo_.SetProjectionMode(CastSessionRtsp::ProjectionMode::STREAM);
    }
    auto streamManager = CreateStreamPlayerManager();
    if (streamManager) {
        rtspParamInfo_.SetPlayerControllerCapability(streamManager->GetStreamPlayerCapability());
    }

    const auto &remote = remoteDeviceInfo->remoteDevice;
    DeviceTypeParamInfo param = {
        .localDeviceType = localDevice_.deviceType,
        .localDeviceSubtype = localDevice_.subDeviceType,
        .remoteDeviceType = remote.deviceType,
        .remoteDeviceSubtype = remote.subDeviceType,
    };
    rtspParamInfo_.SetDeviceTypeParamInfo(param);
    rtspParamInfo_.SetFeatureSet(std::set<int> { ParamInfo::FEATURE_STOP_VTP, ParamInfo::FEATURE_FINE_STYLUS,
        ParamInfo::FEATURE_SOURCE_MOUSE, ParamInfo::FEATURE_SOURCE_MOUSE_HISTORY,
        ParamInfo::FEATURE_SEND_EVENT_CHANGE });
}

std::string CastSessionImpl::GetCurrentRemoteDeviceId()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (remoteDeviceList_.empty()) {
        CLOGE("failed, list is empty.");
        return "";
    }
    return remoteDeviceList_.front().remoteDevice.deviceId;
}

bool CastSessionImpl::ProcessConnect(const Message &msg)
{
    auto remoteDeviceInfo = FindRemoteDevice(msg.strArg_);
    if (remoteDeviceInfo == nullptr) {
        CLOGE("remote device is null");
        return false;
    }
    InitRtspParamInfo(remoteDeviceInfo);
    if (!rtspControl_->Start(rtspParamInfo_, remoteDeviceInfo->remoteDevice.sessionKey,
        remoteDeviceInfo->remoteDevice.sessionKeyLength)) {
        CLOGE("Rtsp start failed, session state: %{public}s",
            SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());
        rtspControl_->Action(ActionType::TEARDOWN);
        return false;
    }

    const auto &remote = remoteDeviceInfo->remoteDevice;
    CLOGD("DeviceName = %s, deviceId = %s, sessionId = %{public}d.", remote.deviceName.c_str(), remote.deviceId.c_str(),
        remoteDeviceInfo->remoteDevice.sessionId);

    auto request = BuildChannelRequest(remote.deviceId, false, ModuleType::RTSP);
    if (request == nullptr) {
        CLOGE("Rtsp start failed, session state: %{public}s",
            SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());
        return false;
    }
    int deviceSessionId = channelManager_->CreateChannel(*request, rtspControl_->GetChannelListener());
    UpdateRemoteDeviceSessionId(remote.deviceId, deviceSessionId);

    CLOGD("Out: deviceName = %s, deviceId = %s, sessionId = %{public}d.", remote.deviceName.c_str(),
        remote.deviceId.c_str(), remoteDeviceInfo->remoteDevice.sessionId);
    return true;
}

bool CastSessionImpl::SetupRemoteControl(const CastInnerRemoteDevice &remote)
{
    return true;
}

bool CastSessionImpl::IsVtpUsed(ChannelType type)
{
    return (type != ChannelType::SOFT_BUS) && (rtspParamInfo_.GetSupportVtpOpt() != VtpType::VTP_NOT_SUPPORT_VIDEO);
}

bool CastSessionImpl::IsChannelClient(ChannelType type)
{
    if (property_.endType == EndType::CAST_SOURCE) {
        return IsVtpUsed(type);
    }

    return !IsVtpUsed(type);
}

bool CastSessionImpl::IsChannelNeeded(ChannelType type)
{
    return (property_.endType != EndType::CAST_SINK) || !IsVtpUsed(type);
}

std::pair<int, int> CastSessionImpl::GetMediaPort(ChannelType type, int port)
{
    if (!IsChannelClient(type)) {
        return (property_.protocolType == ProtocolType::CAST_PLUS_MIRROR ||
            property_.protocolType == ProtocolType::CAST_PLUS_STREAM) ?
            std::pair<int, int> { INVALID_PORT, INVALID_PORT } :
            std::pair<int, int> { INVALID_PORT, UNNEEDED_PORT };
    }

    if (property_.protocolType == ProtocolType::CAST_PLUS_MIRROR ||
        property_.protocolType == ProtocolType::CAST_PLUS_STREAM) {
        if (!IsVtpUsed(type)) {
            // audio port is same as video base on tcp protocol, softbus don't care about the port.
            return { port, port };
        }

        int videoPort = static_cast<int>((static_cast<unsigned int>(port) >> SOCKET_PORT_BITS) & SOCKET_PORT_MASK);
        int audioPort = static_cast<int>(static_cast<unsigned int>(port) & SOCKET_PORT_MASK);
        return { videoPort, audioPort };
    }
    return { port, UNNEEDED_PORT };
}

std::optional<int> CastSessionImpl::SetupMedia(const CastInnerRemoteDevice &remote, ChannelType type, int ports)
{
    mediaState_ = ModuleState::STARTING;

    auto [videoPort, audioPort] = GetMediaPort(type, ports);
    // need 2 channels: video channel and audio channel.
    std::array<std::pair<int, ModuleType>, 2> portsInfo { { { videoPort, ModuleType::VIDEO },
        { audioPort, ModuleType::AUDIO } } };

    for (auto &tmp : portsInfo) {
        // Audio channel is not needed in the pc/pad cooperation.
        // For a channel server, the port is -1 before creating the channel.
        if (tmp.first == UNNEEDED_PORT || (tmp.first == INVALID_PORT && !IsChannelNeeded(type))) {
            continue;
        }

        auto request = BuildChannelRequest(remote.deviceId, IsVtpUsed(type), tmp.second);
        if (request == nullptr) {
            CLOGE("rtsp start failed, session state: %{public}s",
                SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());
            return std::nullopt;
        }
    }

    return portsInfo[0].first;
}

bool CastSessionImpl::ProcessSetUp(const Message &msg)
{
    CLOGD("Media ports: %d, rc ports: %d id %s", msg.arg1_, msg.arg2_, msg.strArg_.c_str());

    auto deviceInfo = FindRemoteDevice(msg.strArg_);
    if (deviceInfo == nullptr) {
        CLOGE("Remote device is null");
        rtspControl_->Action(ActionType::TEARDOWN);
        return false;
    }

    const auto &remote = deviceInfo->remoteDevice;
    auto channelType = remote.channelType;
    CLOGD("DeviceName = %s, deviceId = %s, sessionId = %{public}d, channelType = %{public}d.",
        remote.deviceName.c_str(), remote.deviceId.c_str(), remote.sessionId, channelType);
    if (!SetupRemoteControl(remote)) {
        rtspControl_->Action(ActionType::TEARDOWN);
        return false;
    }

    int mediaPort = msg.arg1_;
    auto port = SetupMedia(remote, channelType, mediaPort);
    if (port == std::nullopt) {
        rtspControl_->Action(ActionType::TEARDOWN);
        return false;
    }

    if (property_.endType == EndType::CAST_SOURCE) {
        rtspControl_->SetupPort(*port, mediaPort, INVALID_PORT);
    }

    return true;
}

bool CastSessionImpl::ProcessSetUpSuccess(const Message &msg)
{
    int moduleId = msg.arg1_;
    CLOGD("Module Id:%{public}d, media state:%{public}hhu, remote control state:%{public}hhu, session state:"
        "%{public}s",
        moduleId, mediaState_, remoteCtlState_, SESSION_STATE_STRING[static_cast<int>(sessionState_)].c_str());

    if (moduleId == MODULE_ID_MEDIA && mediaState_ == ModuleState::STARTING) {
        mediaState_ = ModuleState::START_SUCCESS;
    }

    if (moduleId == MODULE_ID_RC && remoteCtlState_ == ModuleState::STARTING) {
        remoteCtlState_ = ModuleState::START_SUCCESS;
    }

    return (mediaState_ == ModuleState::START_SUCCESS) &&
        (remoteCtlState_ == ModuleState::IDLE || remoteCtlState_ == ModuleState::START_SUCCESS);
}

bool CastSessionImpl::ProcessPause(const Message &msg)
{
    CLOGD("In");
    rtspControl_->Action(ActionType::PAUSE);
    return true;
}

bool CastSessionImpl::ProcessPauseReq(const Message &msg)
{
    CLOGD("In");
    return true;
}

bool CastSessionImpl::ProcessPlay(const Message &msg)
{
    CLOGD("In");
    return rtspControl_->Action(ActionType::PLAY);
}

bool CastSessionImpl::ProcessPlayReq(const Message &msg)
{
    CLOGD("ProcessPlayReq vtp port:%d", msg.arg1_);
    if (property_.endType == EndType::CAST_SINK) {
        return true;
    } else {
        return true;
    }
}

bool CastSessionImpl::ProcessDisconnect(const Message &msg)
{
    CLOGD("In");
    rtspControl_->Action(ActionType::TEARDOWN);
    channelManager_->DestroyAllChannels();
    return true;
}

bool CastSessionImpl::ProcessError(const Message &msg)
{
    CLOGD("In");
    bool result = ProcessDisconnect(msg);
    std::lock_guard<std::mutex> lock(mutex_);
    auto &devices = remoteDeviceList_;
    for (auto it = devices.begin(); it != devices.end();) {
        ChangeDeviceStateLocked(DeviceState::DISCONNECTED, it->remoteDevice.deviceId);
        ConnectionManager::GetInstance().DisconnectDevice(it->remoteDevice.deviceId);
        devices.erase(it++);
    }

    return result;
}

bool CastSessionImpl::ProcessUpdateVideoSize(const Message &msg)
{
    return true;
}

bool CastSessionImpl::ProcessStateEvent(MessageId msgId, const Message &msg)
{
    if (stateProcessor_[msgId] == nullptr) {
        CLOGE("%{public}s' processor is null", MESSAGE_ID_STRING[msgId].c_str());
        return false;
    }

    return (this->*stateProcessor_[msgId])(msg);
}

bool CheckJsonMemberType(Json::Value rootValue)
{
    if (!rootValue.isMember(KEY_BUNDLE_NAME) || !rootValue[KEY_BUNDLE_NAME].isString()) {
        CLOGE("parse bundle name failed");
        return false;
    }
    if (!rootValue.isMember(KEY_PID) || !rootValue[KEY_PID].isInt()) {
        CLOGE("parse pid failed");
        return false;
    }
    if (!rootValue.isMember(KEY_APP_MIN_COMPATIBLE_VERSION) ||
        !rootValue[KEY_APP_MIN_COMPATIBLE_VERSION].isInt()) {
        CLOGE("parse app min compatible version failed");
        return false;
    }
    if (!rootValue.isMember(KEY_APP_TARGET_VERSION) || !rootValue[KEY_APP_TARGET_VERSION].isInt()) {
        CLOGE("parse app target version failed");
        return false;
    }
    return true;
}

bool CastSessionImpl::ProcessSetCastMode(const Message &msg)
{
    CastMode mode = static_cast<CastMode>(msg.arg1_);
    switch (mode) {
        case CastMode::MIRROR_CAST:
            break;
        default:
            break;
    }

    return true;
}

std::shared_ptr<ChannelRequest> CastSessionImpl::BuildChannelRequest(const std::string &remoteDeviceId,
    bool isSupportVtp, ModuleType moduleType)
{
    auto deviceInfo = FindRemoteDevice(remoteDeviceId);
    if (deviceInfo == nullptr) {
        CLOGE("Remote device is null");
        return nullptr;
    }

    const auto &remote = deviceInfo->remoteDevice;
    bool isReceiver = !(property_.endType == EndType::CAST_SOURCE &&
        (moduleType == ModuleType::VIDEO || moduleType == ModuleType::AUDIO));

    return std::make_shared<ChannelRequest>(moduleType, isReceiver, localDevice_, remote, property_);
}

std::shared_ptr<CastRemoteDeviceInfo> CastSessionImpl::FindRemoteDevice(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return FindRemoteDeviceLocked(deviceId);
}

std::shared_ptr<CastRemoteDeviceInfo> CastSessionImpl::FindRemoteDeviceLocked(const std::string &deviceId)
{
    for (auto &deviceInfo : remoteDeviceList_) {
        if (deviceInfo.remoteDevice.deviceId == deviceId) {
            return std::make_shared<CastRemoteDeviceInfo>(deviceInfo);
        }
    }
    return nullptr;
}

void CastSessionImpl::UpdateRemoteDeviceStateLocked(const std::string &deviceId, DeviceState state)
{
    for (auto &deviceInfo : remoteDeviceList_) {
        if (deviceInfo.remoteDevice.deviceId == deviceId) {
            deviceInfo.deviceState = state;
            return;
        }
    }
}

void CastSessionImpl::UpdateRemoteDeviceSessionId(const std::string &deviceId, int sessionId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &deviceInfo : remoteDeviceList_) {
        if (deviceInfo.remoteDevice.deviceId == deviceId) {
            deviceInfo.remoteDevice.sessionId = sessionId;
            return;
        }
    }
}

void CastSessionImpl::RemoveRemoteDevice(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = remoteDeviceList_.begin(); it != remoteDeviceList_.end(); it++) {
        if (it->remoteDevice.deviceId == deviceId) {
            CLOGI("Start to remove remote device:%s", deviceId.c_str());
            ConnectionManager::GetInstance().DisconnectDevice(deviceId);
            remoteDeviceList_.erase(it);
            return;
        }
    }
}

bool CastSessionImpl::AddRemoteDevice(const CastRemoteDeviceInfo &remoteDeviceInfo)
{
    const auto &remote = remoteDeviceInfo.remoteDevice;
    if (FindRemoteDevice(remote.deviceId) != nullptr) {
        CLOGW("Remote device(%s) has existed", remote.deviceName.c_str());
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    remoteDeviceList_.push_back(remoteDeviceInfo);
    return true;
}

// Reserved for 1->N scenarios
bool CastSessionImpl::IsAllowTransferState(SessionState desiredState) const
{
    return true;
}

void CastSessionImpl::ChangeDeviceState(DeviceState state, const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ChangeDeviceStateLocked(state, deviceId);
}

void CastSessionImpl::ChangeDeviceStateLocked(DeviceState state, const std::string &deviceId)
{
    auto deviceInfo = FindRemoteDeviceLocked(deviceId);
    if (!deviceInfo) {
        CLOGE("does not exist this device, deviceId = %s.", deviceId.c_str());
        return;
    }

    CLOGD("New state:%{public}s, old state:%{public}s, device id:%s",
        DEVICE_STATE_STRING[static_cast<int>(state)].c_str(),
        DEVICE_STATE_STRING[static_cast<int>(deviceInfo->deviceState)].c_str(), deviceId.c_str());
    if (state == deviceInfo->deviceState) {
        return;
    }

    UpdateRemoteDeviceStateLocked(deviceId, state);

    for (const auto &[pid, listener] : listeners_) {
        listener->OnDeviceState(DeviceStateInfo { state, deviceId });
    }
}

void CastSessionImpl::OnEvent(EventId eventId, const std::string &data)
{
    if (listeners_.empty()) {
        CLOGE("OnEvent failed because listeners_ is empty!");
        return;
    }
    for (const auto &[pid, listener] : listeners_) {
        listener->OnEvent(eventId, data);
    }
}

bool CastSessionImpl::ProcessTriggerReq(const Message &msg)
{
    return false;
}

void CastSessionImpl::ProcessRtspEvent(int moduleId, int event, const std::string &param)
{
    switch (moduleId) {
        case MODULE_ID_CAST_STREAM:
            SendCastMessage(Message(MessageId::MSG_STREAM_RECV_ACTION_EVENT_FROM_PEERS, event, param));
            break;
        case MODULE_ID_CAST_SESSION:
            if (event == static_cast<int>(CastSessionRemoteEventId::READY_TO_PLAYING)) {
                SendCastMessage(Message(MessageId::MSG_READY_TO_PLAYING));
            }
            break;
        default:
            break;
    }
}

bool CastSessionImpl::IsSupportFeature(const std::set<int> &featureSet, int supportFeature)
{
    return !featureSet.empty() && featureSet.find(supportFeature) != featureSet.end();
}

bool CastSessionImpl::IsConnected() const
{
    return sessionState_ == SessionState::PLAYING || sessionState_ == SessionState::PAUSED ||
        sessionState_ == SessionState::CONNECTED || sessionState_ == SessionState::STREAM;
}

bool CastSessionImpl::SendEventChange(int moduleId, int event, const std::string &param)
{
    CLOGI("Module id %{public}d send event %{public}d", moduleId, event);
    if (!IsConnected() || !rtspControl_) {
        CLOGE("Send event change fail, state is not ready %{public}hhu", sessionState_);
        return false;
    }

    if (!IsSupportFeature(rtspControl_->GetNegotiatedFeatureSet(), ParamInfo::FEATURE_SEND_EVENT_CHANGE)) {
        CLOGE("The feature is not in the feature set.");
        return false;
    }

    return rtspControl_->SendEventChange(moduleId, event, param);
}

std::shared_ptr<ICastStreamManager> CastSessionImpl::StreamManagerGetter()
{
    std::lock_guard<std::mutex> lock(streamMutex_);
    return streamManager_;
}

sptr<IMirrorPlayerImpl> CastSessionImpl::MirrorPlayerGetter()
{
    std::lock_guard<std::mutex> lock(mirrorMutex_);
    return mirrorPlayer_;
}

bool CastSessionImpl::IsStreamMode()
{
    std::lock_guard<std::mutex> lock(streamMutex_);
    return rtspParamInfo_.GetProjectionMode() == CastSessionRtsp::ProjectionMode::STREAM;
}

std::string CastSessionImpl::GetPlayerControllerCapability()
{
    std::lock_guard<std::mutex> lock(streamMutex_);
    return rtspParamInfo_.GetPlayerControllerCapability();
}

bool CastSessionImpl::IsSink()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return property_.endType == EndType::CAST_SINK;
}

int CastSessionImpl::CreateStreamChannel()
{
    CLOGD("in");
    auto request = BuildChannelRequest(GetCurrentRemoteDeviceId(), false, ModuleType::STREAM);
    if (request == nullptr) {
        CLOGE("build channel request failed");
        return INVALID_PORT;
    }

    const auto streamManager =  StreamManagerGetter();
    if (channelManager_ == nullptr || streamManager == nullptr) {
        CLOGE("channelManager_ or streamManager is null");
        return INVALID_PORT;
    }
    int port = channelManager_->CreateChannel(*request, streamManager->GetChannelListener());
    if (port == INVALID_PORT) {
        CLOGE("create stream channel failed");
        return INVALID_PORT;
    }
    if (property_.endType == EndType::CAST_SOURCE) {
        SendEventChange(MODULE_ID_CAST_STREAM, ICastStreamManager::MODULE_EVENT_ID_STREAM_CHANNEL,
            std::to_string(port));
    }
    return port;
}

void CastSessionImpl::SendCastRenderReadyOption(int isReady)
{
    std::shared_ptr<IRtspController> rtspControl;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        rtspControl = rtspControl_;
        if (!IsConnected() || !rtspControl || property_.endType != EndType::CAST_SINK) {
            CLOGE("Send render ready failed");
            return;
        }
    }
    rtspControl->SendCastRenderReadyOption(isReady);
}

int32_t CastSessionImpl::NotifyEvent(EventId eventId, std::string &jsonParam)
{
    if (!Permission::CheckPidPermission()) {
        return CAST_ENGINE_ERROR;
    }
    switch (eventId) {
        default:
            break;
    }
    return CAST_ENGINE_SUCCESS;
}

int32_t CastSessionImpl::SetCastMode(CastMode mode, std::string &jsonParam)
{
    CLOGD("in, mode = %d, param = %s", static_cast<int>(mode), jsonParam.c_str());
    if (!Permission::CheckPidPermission()) {
        return ERR_NO_PERMISSION;
    }
    if (!SendCastMessage(Message(MessageId::MSG_SET_CAST_MODE, static_cast<int>(mode), jsonParam))) {
        return CAST_ENGINE_ERROR;
    }
    return CAST_ENGINE_SUCCESS;
}

void CastSessionImpl::OnEventInner(sptr<CastSessionImpl> session, EventId eventId, const std::string &jsonParam)
{
    std::unique_lock<std::mutex> lock(mutex_);
    for (const auto &[pid, listener] : session->listeners_) {
        listener->OnEvent(eventId, jsonParam);
    }
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
