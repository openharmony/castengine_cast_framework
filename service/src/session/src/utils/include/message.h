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
 * Description: message definition
 * Author: lijianzhao
 * Create: 2022-01-19
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <chrono>
#include <functional>
#include <string>

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class Message {
public:
    int what_;
    int arg1_{ 0 };
    int arg2_{ 0 };
    EventCode eventCode_{ EventCode::DEFAULT_EVENT };

    using Function = std::function<void()>;
    Function task_ = nullptr;

    std::chrono::system_clock::time_point when_;

    // 用于保存指针类型数据
    intptr_t ptrArg_ = -1;

    std::string strArg_;

public:
    Message();
    Message(const Message &msg);
    Message &operator=(const Message &msg);
    explicit Message(int what);
    Message(int what, std::string strArg);
    Message(int what, std::string deviceId, EventCode eventCode) : what_(what),
        eventCode_(eventCode), strArg_(deviceId) {}
    Message(int what, int arg1);
    Message(int what, int arg1, int arg2);
    Message(int what, int arg1, int arg2, long uptimeMillis);
    Message(int what, int arg1, int arg2, long uptimeMillis, std::string strArg);
    Message(int what, int arg1, std::string strArg);
    virtual ~Message() = default;

    void SetWhen(long uptimeMillis);
    void SetFunction(Function func);
    void SetPtrArg(intptr_t arg);
    void SetStrArg(std::string strArg);

    bool operator>(const Message &msg) const
    {
        return (this->when_ > msg.when_);
    }

    bool operator<(const Message &msg) const
    {
        return (this->when_ < msg.when_);
    }

    bool operator==(const Message &msg) const
    {
        return (this->what_ == msg.what_) && (this->task_ != nullptr) && (msg.task_ != nullptr);
    }

    bool operator==(int what) const
    {
        return (this->what_ == what);
    }
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif