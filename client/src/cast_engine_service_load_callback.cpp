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
 * Description: supply cast session manager service proxy class
 * Author: zhuzhibin
 * Create: 2022-10-25
 */

#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "system_ability_definition.h"
#include "cast_engine_service_load_callback.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Client-LoadServiceCallback");

void CastEngineServiceLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
    const sptr<IRemoteObject> &remoteObject)
{
    CLOGI("In systemAbilityId: %d", systemAbilityId);
    if (systemAbilityId != CAST_ENGINE_SA_ID) {
        CLOGE("Start aystemabilityId is not sinkSAId!");
        return;
    }
    if (remoteObject == nullptr) {
        CLOGE("RemoteObject is nullptr.");
        return;
    }
}

void CastEngineServiceLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    CLOGI("In systemAbilityId: %d.", systemAbilityId);
    if (systemAbilityId != CAST_ENGINE_SA_ID) {
        CLOGE("Start aystemabilityId is not sinkSAId!");
        return;
    }
}
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS