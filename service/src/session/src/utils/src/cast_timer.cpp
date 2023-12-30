/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 */
#include "cast_timer.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Timer");

CastTimer::~CastTimer()
{
    CLOGD("%s In, exit_ = %d.", __FUNCTION__, exit_.load());
    if (exit_.load()) {
        return;
    }
    Stop();
}

void CastTimer::Start(std::function<void()> task, int interval)
{
    CLOGD("%s In, exit_ = %d.", __FUNCTION__, exit_.load());
    std::unique_lock<std::mutex> locker(threadMutex_);
    exit_ = false;
    workThread_ = std::thread([this, interval, task]() {
        while (!exit_.load()) {
            {
                // sleep every interval and do the task again and again until times up
                std::unique_lock<std::mutex> locker(mutex_);
                cond_.wait_for(locker, std::chrono::milliseconds(interval), [this] { return exit_.load(); });
            }

            if (exit_.load()) {
                return;
            }

            if (task) {
                task();
            }
        }
    });
}

void CastTimer::Stop()
{
    CLOGD("%s In, exit_ = %d.", __FUNCTION__, exit_.load());
    std::unique_lock<std::mutex> locker(threadMutex_);
    exit_ = true;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        cond_.notify_all();
    }
    if (workThread_.joinable()) {
        workThread_.join();
    }
}

bool CastTimer::IsStopped()
{
    return exit_;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS