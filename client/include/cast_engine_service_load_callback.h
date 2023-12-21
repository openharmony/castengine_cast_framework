/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
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