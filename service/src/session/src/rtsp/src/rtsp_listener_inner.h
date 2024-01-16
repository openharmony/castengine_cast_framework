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
