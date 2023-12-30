/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: rtsp internal listener
 * Author: dingkang
 * Create: 2022-01-24
 */
#ifndef LIBCASTENGINE_RTSP_LISTENER_INNER_H
#define LIBCASTENGINE_RTSP_LISTENER_INNER_H

#include "rtsp_parse.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
class RtspListenerInner {
public:
    RtspListenerInner() {}
    virtual ~RtspListenerInner() {}

    virtual void OnPeerReady(bool isSoftbus) = 0;
    virtual bool OnRequest(RtspParse &request) = 0;
    virtual bool OnResponse(RtspParse &response) = 0;
    virtual void OnPeerGone() = 0;
    virtual void OnTimeKeepAlive() = 0;
};
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // LIBCASTENGINE_RTSP_LISTENER_INNER_H
