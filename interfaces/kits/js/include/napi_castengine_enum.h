/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
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