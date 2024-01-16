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
 * Description: Cast session implement class.
 * Author: zhangge
 * Create: 2022-07-19
 */

#ifndef CAST_SESSION_IMPL_H
#define CAST_SESSION_IMPL_H
#include "cast_session_impl_class.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using CastSessionRtsp::IRtspController;
using CastSessionRtsp::IRtspListener;
using CastSessionRtsp::ParamInfo;
using CastSessionEnums::MessageId;

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

class CastSessionImpl::ConnectManagerListenerImpl : public IConnectManagerSessionListener {
public:
    explicit ConnectManagerListenerImpl(sptr<CastSessionImpl> session) : session_(session) {}

    void NotifySessionEvent(const std::string &deviceId, int result) override;

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

class CastSessionImpl::AuthingState : public BaseState {
public:
    AuthingState(SessionState stateId, sptr<CastSessionImpl> session, std::shared_ptr<State> parentState = nullptr)
        : BaseState(stateId, session, parentState) {};

protected:
    void Enter() override;
    void Exit() override;
    bool HandleMessage(const Message &msg) override;
    DISALLOW_EVIL_CONSTRUCTORS(AuthingState);
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
    bool RecvActionEventFromPeers(const Message &msg);
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
