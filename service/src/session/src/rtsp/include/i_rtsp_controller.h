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
 * Author: zhangge
 * Create: 2022-07-13
 */
#ifndef LIBCASTENGINE_I_RTSP_CONTROLLER_H
#define LIBCASTENGINE_I_RTSP_CONTROLLER_H

#include <memory>
#include <string>
#include <set>

#include "cast_service_common.h"
#include "channel.h"
#include "channel_listener.h"
#include "rtsp_param_info.h"
#include "rtsp_basetype.h"
#include "rtsp_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
class IRtspController {
public:
    IRtspController() = default;
    virtual ~IRtspController() = default;

    static std::shared_ptr<IRtspController> GetInstance(std::shared_ptr<IRtspListener> listener,
        ProtocolType protocolType, EndType endType);

    virtual void AddChannel(std::shared_ptr<Channel> channel, const CastInnerRemoteDevice &device) = 0;
    virtual void RemoveChannel(std::shared_ptr<Channel> channel) = 0;
    virtual std::shared_ptr<IChannelListener> GetChannelListener() = 0;

    virtual bool Start(const ParamInfo &sourceParam, const uint8_t *sessionKey, uint32_t sessionKeyLength) = 0;
    virtual bool Action(ActionType actionType) = 0;

    virtual bool SendEventChange(int moduleId, int event, const std::string &param) = 0;
    virtual void SetupPort(int serverPort, int remotectlPort, int cpPort) = 0;
    virtual void SendCastRenderReadyOption(int isReady) = 0;
    virtual void DetectKeepAliveFeature() const = 0;
    virtual void ModuleCustomParamsNegotiationDone() = 0;

    virtual void SetNegotiatedMediaCapability(const std::string &negotiationMediaParams) = 0;
    virtual void SetNegotiatedPlayerControllerCapability(const std::string &negotiationParams) = 0;
    virtual const std::set<int> &GetNegotiatedFeatureSet() = 0;
};
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // LIBCASTENGINE_I_RTSP_CONTROLLER_H
