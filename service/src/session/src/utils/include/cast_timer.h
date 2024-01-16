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
 */
#ifndef CAST_TIMER_H
#define CAST_TIMER_H

#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastTimer final {
public:
    static constexpr int MILLISECONDS_IN_ONE_SECOND{ 1000 };
    CastTimer() = default;
    ~CastTimer();
    void Start(std::function<void()> task, int interval = MILLISECONDS_IN_ONE_SECOND);
    void Stop();
    bool IsStopped();

private:
    std::atomic<bool> exit_{ true };
    std::mutex mutex_;
    std::mutex threadMutex_;
    std::thread workThread_;
    std::condition_variable cond_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_TIMER_H