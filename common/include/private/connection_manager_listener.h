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