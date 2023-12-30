/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast session manager listener apis.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef I_CAST_SESSION_MANAGER_LISTENER_H
#define I_CAST_SESSION_MANAGER_LISTENER_H

#include "cast_engine_common.h"
#include "i_cast_session.h"

namespace OHOS {
namespace CastEngine {
class EXPORT ICastSessionManagerListener {
public:
    ICastSessionManagerListener() = default;
    ICastSessionManagerListener(const ICastSessionManagerListener &) = delete;
    ICastSessionManagerListener &operator=(const ICastSessionManagerListener &) = delete;
    ICastSessionManagerListener(ICastSessionManagerListener &&) = delete;
    ICastSessionManagerListener &operator=(ICastSessionManagerListener &&) = delete;
    virtual ~ICastSessionManagerListener() = default;

    virtual void OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList) = 0;
    virtual void OnDeviceOffline(const std::string &deviceId) = 0;
    virtual void OnSessionCreated(const std::shared_ptr<ICastSession> &castSession) = 0;
    virtual void OnServiceDied() = 0;
};
} // namespace CastEngine
} // namespace OHOS
#endif