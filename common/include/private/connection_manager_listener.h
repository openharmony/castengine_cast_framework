/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: connection manager session listener
 * Author: jiangfan
 * Create: 2023-11-08
 */
#ifndef LIBCASTENGINE_CONNECTMANAGER_LISTENER_H
#define LIBCASTENGINE_CONNECTMANAGER_LISTENER_H

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {

class IConnectManagerSessionListener {
public:
    virtual ~IConnectManagerSessionListener() {}

    virtual void NotifySessionEvent(const std::string &deviceId, int result) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // LIBCASTENGINE_CONNECTMANAGER_LISTENER_H