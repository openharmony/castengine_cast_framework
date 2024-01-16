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
 * Description: supply cast stream listener interface.
 * Author: huangchanggui
 * Create: 2023-02-01
 */

#ifndef I_CAST_STREAM_LISTERNER_H
#define I_CAST_STREAM_LISTERNER_H

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class ICastStreamListener {
public:
    virtual ~ICastStreamListener() = default;
    virtual bool SendActionToPeers(int action, const std::string &param) = 0;
    virtual void OnRenderReady(bool isReady) = 0;
    virtual void OnEvent(EventId eventId, const std::string &data) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H