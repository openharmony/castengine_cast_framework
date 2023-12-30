/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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