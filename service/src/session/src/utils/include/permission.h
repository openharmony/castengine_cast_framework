/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: add permission utils
 * Author: zhangge
 * Create: 2023-05-11
 */

#ifndef CAST_SERVICE_PERMISSION_H
#define CAST_SERVICE_PERMISSION_H

#include "cast_engine_common.h"
#include <vector>
#include <mutex>
#include "ipc_skeleton.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class Permission {
public:
    static bool CheckMirrorPermission();
    static bool CheckStreamPermission();
    static bool CheckPidPermission();
    static void SavePid(pid_t pid);
    static void RemovePid(pid_t pid);
    static void ClearPids();
    static void SaveMirrorAppInfo();
    static std::tuple<int32_t, uint32_t, int32_t> GetMirrorAppInfo();

private:
    static std::mutex pidLock_;
    static std::vector<pid_t> pids_;
    static int32_t appUid_;
    static uint32_t appTokenId_;
    static int32_t appPid_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif