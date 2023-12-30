/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply cast session manager service proxy class
 * Author: zhuzhibin
 * Create: 2022-10-25
 */

#ifndef CAST_ENGINE_SERVICE_LOAD_CALLBACK_H
#define CAST_ENGINE_SERVICE_LOAD_CALLBACK_H
#include "system_ability_load_callback_stub.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
class CastEngineServiceLoadCallback : public SystemAbilityLoadCallbackStub {
public:
    void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
    void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;
};
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_ENGINE_SERVICE_LOAD_CALLBACK_H