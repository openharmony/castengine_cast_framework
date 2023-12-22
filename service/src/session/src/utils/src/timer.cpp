/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "timer.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Timer");

Timer::~Timer()
{
    CLOGD("%s In, exit_ = %d.", __FUNCTION__, exit_.load());
    if (exit_.load()) {
        return;
    }
    Stop();
}

void Timer::Start(std::function<void()> task, int interval)
{
    CLOGD("%s In, exit_ = %d.", __FUNCTION__, exit_.load());
    std::thread([this, interval, task]() {
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
    }).detach();
}

void Timer::Stop()
{
    CLOGD("%s In, exit_ = %d.", __FUNCTION__, exit_.load());
    exit_ = true;
    std::unique_lock<std::mutex> locker(mutex_);
    cond_.notify_all();
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS