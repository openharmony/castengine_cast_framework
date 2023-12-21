/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Add Token Fuzz.
 * Author: renshuang
 * Create: 2023-07-07
 */

#include "add_cast_engine_token_fuzzer.h"

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {

void AddCastEngineTokenFuzzer::AddCastEngineToken()
{
    constexpr int castFuzzPermissionNum = 2;
    const char *perms[castFuzzPermissionNum] = {
        "ohos.permission.ACCESS_CAST_ENGINE_MIRROR",
        "ohos.permission.ACCESS_CAST_ENGINE_STREAM",
    };

    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = castFuzzPermissionNum,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "cast_engine_fuzzer",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}
} // namespace OHOS