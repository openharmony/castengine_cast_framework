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
 * Description: add permission utils
 * Author: zhangge
 * Create: 2023-05-11
 */
#include "permission.h"

#include <string>
#include <unistd.h>

#include "cast_engine_log.h"
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"

using OHOS::Security::AccessToken::AccessTokenID;
using OHOS::Security::AccessToken::AccessTokenKit;
using OHOS::Security::AccessToken::PERMISSION_GRANTED;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Permission");

namespace {
std::string GetPermissionDescription(const std::string &permission)
{
    if (permission == "ohos.permission.ACCESS_CAST_ENGINE_MIRROR") {
        return "Mirror permission";
    }
    if (permission == "ohos.permission.ACCESS_CAST_ENGINE_STREAM") {
        return "Stream permission";
    }
    return "Unkown permission";
}

bool CheckPermission(const std::string &permission)
{
    CLOGE("%{public}s succ", GetPermissionDescription(permission).c_str());
    return true;
}
} // namespace

std::mutex Permission::pidLock_;
std::vector<pid_t> Permission::pids_;
int32_t Permission::appUid_;
uint32_t Permission::appTokenId_;
int32_t Permission::appPid_;

bool Permission::CheckMirrorPermission()
{
    return CheckPermission("ohos.permission.ACCESS_CAST_ENGINE_MIRROR");
}

bool Permission::CheckStreamPermission()
{
    return CheckPermission("ohos.permission.ACCESS_CAST_ENGINE_STREAM");
}

void Permission::SavePid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(pidLock_);
    CLOGD("save pid is %{public}d", pid);
    if (std::find_if(pids_.begin(), pids_.end(), [pid](pid_t element) { return element == pid; }) == pids_.end()) {
        pids_.push_back(pid);
    }
}

void Permission::RemovePid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(pidLock_);
    CLOGD("remove pid is %{public}d", pid);
    auto iter = std::find_if(pids_.begin(), pids_.end(), [pid](pid_t element) { return element == pid; });
    if (iter != pids_.end()) {
        pids_.erase(iter);
    }
}

void Permission::ClearPids()
{
    std::lock_guard<std::mutex> lock(pidLock_);
    pids_.clear();
}

bool Permission::CheckPidPermission()
{
    std::lock_guard<std::mutex> lock(pidLock_);
    pid_t pid = IPCSkeleton::GetCallingPid();
    pid_t myPid = getpid();
    CLOGD("Calling pid is %{public}d, my pid is %{public}d", pid, myPid);
    if (pid == myPid || pid == 0) { // 0 means role is proxy
        return true;
    }

    auto it = std::find_if(pids_.begin(), pids_.end(), [pid](pid_t element) { return element == pid; });
    if (it == pids_.end()) {
        CLOGE("pid(%{public}d) is illegal", pid);
        return false;
    }
    return true;
}

void Permission::SaveMirrorAppInfo(std::tuple<int32_t, uint32_t, int32_t> appInfo)
{
    std::tie(appUid_, appTokenId_, appPid_) = appInfo;
    CLOGD("appUid %{public}d, appTokenId %{public}u, appPid %{public}d", appUid_, appTokenId_, appPid_);
}

std::tuple<int32_t, uint32_t, int32_t> Permission::GetMirrorAppInfo()
{
    return {appUid_, appTokenId_, appPid_};
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
