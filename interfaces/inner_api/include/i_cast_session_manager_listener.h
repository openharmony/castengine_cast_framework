/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
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