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
 * Description: handler module for send message and handle message in a looper
 * Author: lijianzhao
 * Create: 2022-01-19
 */

#ifndef HANDLER_H
#define HANDLER_H

#include <algorithm>
#include <chrono>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include "message.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class Handler {
public:
    Handler();
    virtual ~Handler();
    bool SendCastMessage(const Message &msg);
    bool SendCastMessage(int what);
    bool SendCastMessage(int what, int arg1);
    bool SendCastMessage(int what, int arg1, int arg2);
    bool SendCastMessageDelayed(int what, long uptimeMillis);
    void RemoveMessage(const Message &msg);
    void RemoveCallbackAndMessages();
    void StopSafty(bool stopSafty);
    bool IsQuiting();
    virtual void HandleMessage(const Message &msg) = 0;

    void ThreadJoin();

private:
    void HandleMessageInner(const Message &msg);
    std::vector<Message> msgQueue_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::thread looper_;
    bool stop_;
    bool stopWhenEmpty_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif