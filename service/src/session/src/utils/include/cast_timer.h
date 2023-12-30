/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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