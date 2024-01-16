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

#include "handler.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
Handler::Handler() : stop_(false), stopWhenEmpty_(false)
{
    looper_ = std::thread([this]() {
        for (;;) {
            Message msg;
            {
                std::unique_lock<std::mutex> lock(this->queueMutex_);
                if ((this->stopWhenEmpty_ && this->msgQueue_.empty()) || this->stop_) {
                    return;
                }

                if (this->msgQueue_.empty()) {
                    this->condition_.wait(lock);
                }

                if ((this->stopWhenEmpty_ && this->msgQueue_.empty()) || this->stop_) {
                    return;
                }

                // send message at when_
                if (!(this->msgQueue_.empty()) &&
                    (this->condition_.wait_until(lock, this->msgQueue_.back().when_) != std::cv_status::timeout)) {
                    continue;
                }

                if (!this->msgQueue_.empty()) {
                    msg = std::move(this->msgQueue_.back());
                    this->msgQueue_.pop_back();
                } else {
                    continue;
                }
            }
            this->HandleMessageInner(msg);
        }
    });
}

Handler::~Handler()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_ = true;
        condition_.notify_all();
    }

    if (looper_.joinable()) {
        looper_.join();
    }

    msgQueue_.clear();
}

void Handler::ThreadJoin()
{
    if (looper_.joinable()) {
        looper_.join();
    }
}

bool Handler::SendCastMessage(const Message &msg)
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    auto i = std::find(msgQueue_.begin(), msgQueue_.end(), msg);
    if (i != msgQueue_.end()) {
        msgQueue_.erase(i);
    }

    msgQueue_.push_back(msg);
    std::sort(msgQueue_.begin(), msgQueue_.end(), std::greater<Message>());
    condition_.notify_one();
    return true;
}

bool Handler::SendCastMessage(int what)
{
    return SendCastMessage(Message(what));
}

bool Handler::SendCastMessage(int what, int arg1)
{
    return SendCastMessage(Message(what, arg1));
}

bool Handler::SendCastMessage(int what, int arg1, int arg2)
{
    return SendCastMessage(Message(what, arg1, arg2));
}

bool Handler::SendCastMessageDelayed(int what, long uptimeMillis)
{
    if (uptimeMillis < 0) {
        return false;
    }

    Message msg(what);
    msg.SetWhen(uptimeMillis);
    return SendCastMessage(msg);
}

void Handler::RemoveMessage(const Message &msg)
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    for (auto it = msgQueue_.begin(); it != msgQueue_.end();) {
        if (it->what_ == msg.what_) {
            it = msgQueue_.erase(it);
        } else {
            ++it;
        }
    }
    condition_.notify_one();
}

void Handler::RemoveCallbackAndMessages()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    msgQueue_.clear();
}

void Handler::StopSafty(bool stopSafty)
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (stopSafty) {
        stopWhenEmpty_ = true;
    } else {
        stop_ = true;
    }

    condition_.notify_all();
}

bool Handler::IsQuiting()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (stop_ || stopWhenEmpty_) {
        return true;
    }
    return false;
}

void Handler::HandleMessageInner(const Message &msg)
{
    if (msg.task_ != nullptr) {
        msg.task_();
    } else {
        if (msg.what_ < 0) {
            return;
        }
        HandleMessage(msg);
    }
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
