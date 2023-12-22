/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Cast session implement class.
 * Author: zhangge
 * Create: 2022-07-19
 */

#ifndef CAST_SESSION_IMPL_H
#define CAST_SESSION_IMPL_H

#include <array>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <optional>
#include <set>
#include <utility>

#include "cast_engine_common.h"
#include "cast_session_common.h"
#include "cast_session_impl_stub.h"
#include "channel.h"
#include "channel_info.h"
#include "channel_manager.h"
#include "channel_manager_listener.h"
#include "channel_request.h"
#include "message.h"
#include "i_rtsp_controller.h"
#include "oh_remote_control_event.h"
#include "rtsp_listener.h"
#include "rtsp_param_info.h"
#include "state_machine.h"
#include "i_cast_stream_listener.h"
#include "i_cast_stream_manager.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using CastSessionRtsp::IRtspController;
using CastSessionRtsp::IRtspListener;
using CastSessionRtsp::ParamInfo;

class CastSessionImpl : public StateMachine,
    public CastSessionImplStub,
    public std::enable_shared_from_this<CastSessionImpl> {
public:
    CastSessionImpl(const CastSessionProperty &property, const CastLocalDevice &localDevice);
    ~CastSessionImpl() override;
    void SetServiceCallbackForRelease(const std::function<void(int)> &callback);
    int32_t RegisterListener(sptr<ICastSessionListenerImpl> listener) override;
    int32_t UnregisterListener() override;
    int32_t AddDevice(const CastRemoteDevice &remoteDevice) override;
    bool AddDevice(const CastInnerRemoteDevice &remoteDevice) override;
    int32_t RemoveDevice(const std::string &deviceId) override;
    int32_t StartAuth(const AuthInfo &authInfo) override;
    int32_t GetSessionId(std::string &sessionId) override;
    int32_t GetDeviceState(const std::string &deviceId, DeviceState &deviceState) override;
    int32_t SetSessionProperty(const CastSessionProperty &property) override;
    bool ReleaseSessionResources(pid_t pid) override;
    int32_t CreateMirrorPlayer(sptr<IMirrorPlayerImpl> &mirrorPlayer) override;
    int32_t CreateStreamPlayer(sptr<IStreamPlayerIpc> &streamPlayer) override;
    std::shared_ptr<ICastStreamManager> CreateStreamPlayerManager();
    bool DestroyMirrorPlayer();
    bool DestroyStreamPlayer();
    void Stop() override;
    int32_t Release() override;
    bool Init();
    int32_t NotifyEvent(EventId eventId, std::string &jsonParam) override;
    int32_t SetCastMode(CastMode mode, std::string &jsonParam) override;

    int32_t Play(const std::string &deviceId);
    int32_t Pause(const std::string &deviceId);
    int32_t SetSurface(sptr<IBufferProducer> producer);
    int32_t DeliverInputEvent(const OHRemoteControlEvent &event);

private:
    enum MessageId : int {
        MSG_CONNECT,
        MSG_SETUP,
        MSG_SETUP_SUCCESS,
        MSG_SETUP_FAILED,
        MSG_SETUP_DONE,
        MSG_PLAY,
        MSG_PAUSE,
        MSG_PLAY_REQ,
        MSG_PAUSE_REQ,
        MSG_DISCONNECT,
        MSG_CONNECT_TIMEOUT,
        MSG_PROCESS_TRIGGER_REQ,
        MSG_UPDATE_VIDEO_SIZE,
        MSG_STREAM_RECV_ACTION_EVENT_FROM_PEERS,
        MSG_STREAM_SEND_ACTION_EVENT_TO_PEERS,
        MSG_PEER_RENDER_READY,
        MSG_ERROR,
        MSG_SET_CAST_MODE,
        MSG_READY_TO_PLAYING,
        MSG_ID_MAX,
    };

    static const std::array<std::string, static_cast<size_t>(MessageId::MSG_ID_MAX)> MESSAGE_ID_STRING;

    enum class ModuleState : uint8_t {
        IDLE,
        STARTING,
        START_SUCCESS,
        START_FAILED
    };

    enum class CastSessionRemoteEventId : uint8_t {
        SWITCH_TO_MIRROR = 2,
        READY_TO_PLAYING,
    };
    class ChannelManagerListenerImpl;
    class RtspListenerImpl;
    class CastStreamListenerImpl;
    class BaseState;
    class DefaultState;
    class DisconnectedState;
    class ConnectingState;
    class ConnectedState;
    class PausedState;
    class PlayingState;
    class DisconnectingState;
    class StreamState;

    static constexpr int MODULE_ID_MEDIA = 1001;
    static constexpr int MODULE_ID_RC = 1002;
    static constexpr int MODULE_ID_CAST_STREAM = 1009;
    static constexpr int MODULE_ID_CAST_SESSION = 2000;
    static constexpr double CAST_VERSION = 1.1;
    static constexpr int TIMEOUT_CONNECT = 60 * 1000;
    static constexpr int INVALID_PORT = -1;
    static constexpr int UNUSED_VALUE = -1;
    static constexpr int NO_DELAY = 0;
    static constexpr int UNNEEDED_PORT = -2;
    static constexpr unsigned int SOCKET_PORT_BITS = 16;
    static constexpr unsigned int SOCKET_PORT_MASK = 0xFFFF;
    static constexpr int ERR_CODE = -1;
    static constexpr int ID_INC_STEP = 1;
    static constexpr int RENDER_READY = 1;
    static constexpr int NOT_RENDER_READY = 0;

    void SetLocalDevice(const CastLocalDevice &localDevice);
    std::shared_ptr<CastRemoteDeviceInfo> FindRemoteDevice(const std::string &deviceId);
    std::shared_ptr<CastRemoteDeviceInfo> FindRemoteDeviceLocked(const std::string &deviceId);
    void UpdateRemoteDeviceStateLocked(const std::string &deviceId, DeviceState state);
    void UpdateRemoteDeviceSessionId(const std::string &deviceId, int sessionId);
    void ChangeDeviceState(DeviceState state, const std::string &deviceId);
    void ChangeDeviceStateLocked(DeviceState state, const std::string &deviceId);
    void OnEvent(EventId eventId, const std::string &data);
    bool TransferTo(std::shared_ptr<BaseState> state);
    bool IsAllowTransferState(SessionState desiredState) const;
    std::string GetCurrentRemoteDeviceId();
    bool ProcessConnect(const Message &msg);
    bool ProcessSetUp(const Message &msg);
    bool ProcessSetUpSuccess(const Message &msg);
    bool ProcessPlay(const Message &msg);
    bool ProcessPlayReq(const Message &msg);
    bool ProcessPause(const Message &msg);
    bool ProcessPauseReq(const Message &msg);
    bool ProcessDisconnect(const Message &msg);
    bool ProcessError(const Message &msg);
    bool ProcessTriggerReq(const Message &msg);
    bool ProcessUpdateVideoSize(const Message &msg);
    bool ProcessStateEvent(MessageId msgId, const Message &msg);
    bool IsSupportFeature(const std::set<int> &featureSet, int supportFeature);
    bool IsConnected() const;
    bool SendEventChange(int moduleId, int event, const std::string &param);
    void RemoveRemoteDevice(const std::string &deviceId);
    bool AddRemoteDevice(const CastRemoteDeviceInfo &remoteDeviceInfo);
    std::shared_ptr<ChannelRequest> BuildChannelRequest(const std::string &remoteDeviceId, bool isSupportVtp,
        ModuleType moduleType);
    bool SetupRemoteControl(const CastInnerRemoteDevice &remote);
    bool IsVtpUsed(ChannelType type);
    bool IsChannelClient(ChannelType type);
    bool IsChannelNeeded(ChannelType type);
    std::pair<int, int> GetMediaPort(ChannelType type, int port);
    std::optional<int> SetupMedia(const CastInnerRemoteDevice &remote, ChannelType type, int ports);
    void InitRtspParamInfo(std::shared_ptr<CastRemoteDeviceInfo> remoteDeviceInfo);
    std::shared_ptr<ICastStreamManager> StreamManagerGetter();
    sptr<IMirrorPlayerImpl> MirrorPlayerGetter();
    void ProcessRtspEvent(int moduleId, int event, const std::string &param);
    bool ProcessSetCastMode(const Message &msg);

    bool IsStreamMode();
    std::string GetPlayerControllerCapability();
    bool IsSink();
    int CreateStreamChannel();
    void SendCastRenderReadyOption(int isReady);
    void OnEventInner(sptr<CastSessionImpl> session, EventId eventId, const std::string &jsonParam);

    std::shared_ptr<DefaultState> defaultState_;
    std::shared_ptr<DisconnectedState> disconnectedState_;
    std::shared_ptr<ConnectingState> connectingState_;
    std::shared_ptr<ConnectedState> connectedState_;
    std::shared_ptr<PausedState> pausedState_;
    std::shared_ptr<PlayingState> playingState_;
    std::shared_ptr<DisconnectingState> disconnectingState_;

    bool isMirrorPlaying_ { false };
    std::shared_ptr<StreamState> streamState_;

    static std::atomic<int> idCount_;

    std::function<void(int)> serviceCallback_;
    std::map<pid_t, sptr<ICastSessionListenerImpl>> listeners_;
    sptr<IMirrorPlayerImpl> mirrorPlayer_;
    std::shared_ptr<ICastStreamManager> streamManager_;
    int sessionId_{ -1 };
    std::list<CastRemoteDeviceInfo> remoteDeviceList_;
    CastLocalDevice localDevice_;
    CastSessionProperty property_;
    ParamInfo rtspParamInfo_;
    SessionState sessionState_{ SessionState::DISCONNECTED };

    std::shared_ptr<ChannelManager> channelManager_;
    std::shared_ptr<ChannelManagerListenerImpl> channelManagerListener_;
    std::shared_ptr<IRtspController> rtspControl_;
    std::shared_ptr<RtspListenerImpl> rtspListener_;

    std::mutex mutex_;
    std::mutex mirrorMutex_;
    std::mutex streamMutex_;
    std::mutex serviceCallbackMutex_;
    std::condition_variable cond_;
    std::mutex dataTransMutex_;
    CastMode castMode_ = CastMode::MIRROR_CAST;
    ModuleState mediaState_{ ModuleState::IDLE };
    ModuleState remoteCtlState_{ ModuleState::IDLE };

    using StateProcessor = bool (CastSessionImpl::*)(const Message &msg);
    std::array<StateProcessor, static_cast<size_t>(MessageId::MSG_ID_MAX)> stateProcessor_{
        &CastSessionImpl::ProcessConnect,         // MSG_CONNECT
        &CastSessionImpl::ProcessSetUp,           // MSG_SETUP
        &CastSessionImpl::ProcessSetUpSuccess,    // MSG_SETUP_SUCCESS
        nullptr,                                  // MSG_SETUP_FAILED
        nullptr,                                  // MSG_SETUP_DONE
        &CastSessionImpl::ProcessPlay,            // MSG_PLAY
        &CastSessionImpl::ProcessPause,           // MSG_PAUSE
        &CastSessionImpl::ProcessPlayReq,         // MSG_PLAY_REQ
        &CastSessionImpl::ProcessPauseReq,        // MSG_PAUSE_REQ
        &CastSessionImpl::ProcessDisconnect,      // MSG_DISCONNECT
        &CastSessionImpl::ProcessDisconnect,      // MSG_CONNECT_TIMEOUT
        &CastSessionImpl::ProcessTriggerReq,      // MSG_PROCESS_TRIGGER_REQ
        &CastSessionImpl::ProcessUpdateVideoSize, // MSG_UPDATE_VIDEO_SIZE
        nullptr,                                  // MSG_STREAM_RECV_ACTION_EVENT_FROM_PEERS
        nullptr,                                  // MSG_STREAM_SEND_ACTION_EVENT_TO_PEERS
        nullptr,                                  // MSG_PEER_RENDER_READY
        &CastSessionImpl::ProcessError,           // MSG_ERROR
    };
};

class CastSessionImpl::ChannelManagerListenerImpl : public IChannelManagerListener {
public:
    explicit ChannelManagerListenerImpl(sptr<CastSessionImpl> session) : session_(session) {}

    void OnChannelCreated(std::shared_ptr<Channel> channel) override;
    void OnChannelOpenFailed(ChannelRequest &channelRequest, int errorCode) override;
    void OnChannelRemoved(std::shared_ptr<Channel> channel) override;
    void OnChannelError(std::shared_ptr<Channel> channel, int errorCode) override;
    bool IsMediaChannelReady();
    void SetMediaChannel(ModuleType moduleType);

private:
    const static unsigned int UNCONNECTED_STATE = 0;
    const static unsigned int VIDEO_CHANNEL_CONNECTED = 1;
    const static unsigned int AUDIO_CHANNEL_CONNECTED = 2;

    wptr<CastSessionImpl> session_;
    unsigned int mediaChannelState_{ UNCONNECTED_STATE };
};

class CastSessionImpl::RtspListenerImpl : public IRtspListener {
public:
    explicit RtspListenerImpl(sptr<CastSessionImpl> session) : session_(session) {}

    void OnSetup(const ParamInfo &param, int mediaPort, int remoteControlPort, const std::string &deviceId) override;
    bool OnPlay(const ParamInfo &param, int port, const std::string &deviceId) override;
    bool OnPause() override;
    void OnTearDown() override;
    void OnError(int errCode) override;
    bool OnPlayerReady(const ParamInfo &clientParam, const std::string &deviceId, int readyFlag) override;

    void NotifyTrigger(int trigger) override;
    void NotifyEventChange(int moduleId, int event, const std::string &param) override;
    void NotifyModuleCustomParamsNegotiation(const std::string &mediaParams,
        const std::string &controllerParams) override;
    bool NotifyEvent(int event) override;

    int StartMediaVtp(const ParamInfo &param) override;
    void ProcessStreamMode(const ParamInfo &param, const std::string &deviceId) override;

private:
    wptr<CastSessionImpl> session_;
};

class CastSessionImpl::CastStreamListenerImpl : public ICastStreamListener {
public:
    explicit CastStreamListenerImpl(sptr<CastSessionImpl> session) : session_(session) {}
    ~CastStreamListenerImpl() override;
    bool SendActionToPeers(int action, const std::string &param) override;
    void OnRenderReady(bool isReady) override;
    void OnEvent(EventId eventId, const std::string &data) override;

private:
    wptr<CastSessionImpl> session_;
};

class CastSessionImpl::BaseState : public State {
public:
    BaseState(SessionState stateId, sptr<CastSessionImpl> session, std::shared_ptr<State> parentState = nullptr)
        : State(parentState), session_(session), stateId_(stateId) {};

    void Enter(SessionState state);
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    SessionState GetStateId() const;

protected:
    wptr<CastSessionImpl> session_;
    SessionState stateId_;
    DISALLOW_EVIL_CONSTRUCTORS(BaseState);

private:
    void Enter() override {}
};

class CastSessionImpl::DefaultState : public BaseState {
public:
    DefaultState(SessionState stateId, sptr<CastSessionImpl> session, std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(DefaultState);
};

class CastSessionImpl::DisconnectedState : public BaseState {
public:
    DisconnectedState(SessionState stateId, sptr<CastSessionImpl> session, std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(DisconnectedState);
};

class CastSessionImpl::ConnectingState : public BaseState {
public:
    ConnectingState(SessionState stateId, sptr<CastSessionImpl> session, std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    void HandleSetupMessage(const Message &msg, sptr<CastSessionImpl> session);
    void HandleSetupSuccessMessage(const Message &msg, const MessageId &msgId, sptr<CastSessionImpl> session);
    void HandleDisconnectMessage(const Message &msg, sptr<CastSessionImpl> session);
    void HandleErrorMessage(const Message &msg, sptr<CastSessionImpl> session);
    void HandleRenderReadyMessage(const Message &msg, sptr<CastSessionImpl> session);
    void HandleConnectMessage(const Message &msg, const MessageId &msgId, sptr<CastSessionImpl> session);
    DISALLOW_EVIL_CONSTRUCTORS(ConnectingState);
};

class CastSessionImpl::ConnectedState : public BaseState {
public:
    ConnectedState(SessionState stateId, sptr<CastSessionImpl> session, std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(ConnectedState);
};

class CastSessionImpl::PausedState : public BaseState {
public:
    explicit PausedState(SessionState stateId, sptr<CastSessionImpl> session,
        std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(PausedState);
};

class CastSessionImpl::PlayingState : public BaseState {
public:
    explicit PlayingState(SessionState stateId, sptr<CastSessionImpl> session,
        std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(PlayingState);
};

class CastSessionImpl::StreamState : public BaseState {
public:
    explicit StreamState(SessionState stateId, sptr<CastSessionImpl> session,
        std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(StreamState);
};

class CastSessionImpl::DisconnectingState : public BaseState {
public:
    explicit DisconnectingState(SessionState stateId, sptr<CastSessionImpl> session,
        std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(DisconnectingState);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
