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
 * Description: rtsp controller
 * Author: dingkang
 * Create: 2022-01-24
 */
#ifndef LIBCASTENGINE_RTSP_CONTROLLER_H
#define LIBCASTENGINE_RTSP_CONTROLLER_H

#include "channel.h"
#include "rtsp_listener.h"
#include "rtsp_listener_inner.h"
#include "rtsp_channel_manager.h"
#include "i_rtsp_controller.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
class RtspController : public IRtspController, public RtspListenerInner {
public:
    RtspController(std::shared_ptr<IRtspListener> listener, ProtocolType protocolType, EndType endType);
    ~RtspController() override;

    void OnPeerReady(bool isSoftbus) override;
    bool OnRequest(RtspParse &request) override;
    bool OnResponse(RtspParse &response) override;
    void OnPeerGone() override;
    void OnTimeKeepAlive() override;

    std::shared_ptr<IChannelListener> GetChannelListener() override;
    void AddChannel(std::shared_ptr<Channel> channel, const CastInnerRemoteDevice &device) override;
    void RemoveChannel(std::shared_ptr<Channel> channel) override;
    bool Start(const ParamInfo &sourceParam, const uint8_t *sessionKey, uint32_t sessionKeyLength) override;
    bool Action(ActionType actionType) override;
    bool SendEventChange(int moduleId, int event, const std::string &param) override;
    void SetupPort(int serverPort, int remotectlPort, int cpPort) override;
    void SendCastRenderReadyOption(int isReady) override;
    const std::set<int> &GetNegotiatedFeatureSet() override;
    void DetectKeepAliveFeature() const override;
    void ModuleCustomParamsNegotiationDone() override;
    void SetNegotiatedMediaCapability(const std::string &negotiationMediaParams) override;
    void SetNegotiatedPlayerControllerCapability(const std::string &negotiationParams) override;

private:
    enum class RtspEngineState {
        STATE_STOPPED,
        STATE_STARTED,
        STATE_STOPPING,
        STATE_ESTABLISHED
    };

    using ResponseFunc = bool (RtspController::*)(RtspParse &);
    using RequestFunc = bool (RtspController::*)(RtspParse &);
    bool ProcessAnnounceRequest(RtspParse &request);
    bool ProcessCommonResponse(RtspParse &response);
    bool ProcessGetParamM3Response(RtspParse &response);
    bool ProcessSetParamM4Response(RtspParse &response);
    bool ProcessSetParamM5Response(RtspParse &response);
    bool ProcessSetupM6Response(RtspParse &response);
    bool ProcessPlayM7Response(RtspParse &response);
    bool ProcessTearDownM8Response(RtspParse &response);
    bool ProcessPauseM9Response(RtspParse &response);
    bool ProcessKaResponse(RtspParse &response);
    bool SendAction(ActionType type);
    void ProcessSinkDeviceType(const std::string &content);
    bool StopEngine();
    std::string ParseCipherItem(const std::string &item) const;
    bool ProcessOptionRequest(RtspParse &request);
    bool ProcessSetupRequest(RtspParse &request);
    bool ProcessGetParameterRequestM3(RtspParse &request);
    void ProcessGetTrigger(RtspParse &request, const std::string &notifyTrigger) const;
    void ProcessEventChangeRequest(RtspParse &request) const;
    int ProcessGetPort(RtspParse &request) const;
    bool ProcessPlayRequest(RtspParse &request);
    bool ProcessPauseRequest(RtspParse &request);
    bool ProcessTearDownRequest(RtspParse &request);
    bool ProcessRenderReadyRequest(RtspParse &request);
    bool ProcessSetParamRequest(RtspParse &request);
    bool ProcessSetParamRequestM4(RtspParse &request);
    void ProcessUibc(const std::string &content);
    bool PreProcessUibc(const std::string &content, std::string &categoryList);
    void ProcessUibcVendor(const std::string &content, RemoteControlParamInfo &remoteControlParamInfo);
    void ProcessUibcDetermine(const std::string &givenStr, std::vector<std::string> &list);
    void ProcessSinkBitrate(const std::string &content, VideoProperty &videoProperty);
    void ProcessVideoInfo(const std::string &content);
    void ProcessAudioInfo(RtspParse &parseInfo);
    void ProcessAudioExpandInfo(const std::string &content, AudioProperty &audioProperty);
    void ProcessSinkVideoForResolution(const std::string &content, VideoProperty &videoProperty);
    void ProcessFeatureSet(const std::string &content);
    void ProcessSinkVtp(const std::string &content);
    void ProcessProjectionMode(const std::string &content);
    void ProcessModuleCustomParams(const std::string &mediaParams, const std::string &controllerParams);
    bool SendOptionM1M2();
    bool SendGetParamM3();
    bool SendSetParamM4();
    bool SendKeepAliveRequest();
    bool SendErrorResponse(RtspParse &request, const std::string &errorDetail) const;
    bool DealAnnounceRequest(RtspParse &response);
    bool UnOrderedMapContains(std::unordered_map<std::string, std::string> map, const std::string &key) const;
    void ProcessSourceDeviceType(const std::string &content);
    void ProcessTriggerMethod(RtspParse &request, const std::string &triggerMethod);
    void ResponseFuncMapInit();
    void RequestFuncMapInit();

    const ProtocolType protocolType_;
    std::shared_ptr<IRtspListener> listener_;
    EndType endType_;
    int currentSeq_{ 0 };
    int currentSetUpSeq_{ 0 };
    int currentKeepAliveCseq_{ 0 };
    WaitResponse waitRsp_{ WaitResponse::WAITING_RSP_NONE };
    std::unique_ptr<RtspChannelManager> rtspNetManager_;
    ParamInfo paramInfo_{};
    ParamInfo negotiatedParamInfo_{};
    RtspEngineState state_{ RtspEngineState::STATE_STOPPED };
    std::string deviceId_;
    std::map<WaitResponse, ResponseFunc> responseFuncMap_;
    std::map<std::string, RequestFunc> requestFuncMap_;
};
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // LIBCASTENGINE_RTSP_CONTROLLER_H
