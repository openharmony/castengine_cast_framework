/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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