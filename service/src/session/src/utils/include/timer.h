/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef TIMER_H
#define TIMER_H

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
class Timer final {
public:
    static constexpr int MILLISECONDS_IN_ONE_SECOND{ 1000 };
    Timer() = default;
    ~Timer();
    void Start(std::function<void()> task, int interval = MILLISECONDS_IN_ONE_SECOND);
    void Stop();

private:
    std::atomic<bool> exit_{ false };
    std::mutex mutex_;
    std::condition_variable cond_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // TIMER_H