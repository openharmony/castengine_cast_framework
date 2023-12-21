/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: supply napi nums for interfaces.
 * Author: zhangjingnan
 * Create: 2023-4-11
 */

#ifndef NAPI_CASTENGINE_CONST_PROPERTIES_H
#define NAPI_CASTENGINE_CONST_PROPERTIES_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
napi_status InitEnums(napi_env env, napi_value exports);
} // namespace CastEngineClient
} // namespace CastEngine
} // namespace OHOS
#endif