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
 * Description: rtsp listener
 * Author: dingkang
 * Create: 2022-01-24
 */
#ifndef LIBCASTENGINE_RTSP_LISTENER_H
#define LIBCASTENGINE_RTSP_LISTENER_H

#include "rtsp_param_info.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
constexpr int ERROR_CODE_DEFAULT = 0;

class IRtspListener {
public:
    virtual ~IRtspListener() {}

    virtual void OnSetup(const ParamInfo &param, int tsPort, int rcPort, const std::string &deviceId) = 0;
    virtual bool OnPlayerReady(const ParamInfo &clientParam, const std::string &deviceId, int readyFlag) = 0;
    virtual bool OnPlay(const ParamInfo &param, int port, const std::string &deviceId) = 0;
    virtual bool OnPause() = 0;
    virtual void OnTearDown() = 0;
    virtual void OnError(int errCode) = 0;
    virtual void NotifyTrigger(int trigger) = 0;
    virtual void NotifyEventChange(int moduleId, int event, const std::string &param) = 0;
    virtual void NotifyModuleCustomParamsNegotiation(const std::string &mediaParams,
        const std::string &controllerParams) = 0;
    virtual int StartMediaVtp(const ParamInfo &param) = 0;
    virtual bool NotifyEvent(int event) = 0;
    virtual void ProcessStreamMode(const ParamInfo &param, const std::string &deviceId) = 0;
};
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // LIBCASTENGINE_RTSP_LISTENER_H