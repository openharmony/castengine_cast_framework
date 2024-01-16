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
 * Description: state machine function realization.
 * Author: lijianzhao
 * Create: 2022-01-25
 */

#include "state_machine.h"

#include <utility>
#include <vector>

#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Session-StateMachine");

void StateMachine::HandleMessage(const Message &msg)
{
    std::shared_ptr<State> cur = state_;
    while (cur != nullptr && !cur->HandleMessage(msg)) {
        cur = cur->GetParentState();
    }
    if (cur == nullptr) {
        CLOGW("message: %d unhandled in parent state", msg.what_);
    }
}

void StateMachine::TransferState(const std::shared_ptr<State> &state)
{
    if (state == state_) {
        return;
    }

    std::vector<std::shared_ptr<State>> currentStates;
    std::shared_ptr<State> cur = state_;
    while (cur != nullptr) {
        currentStates.push_back(cur);
        cur = cur->GetParentState();
    }

    std::vector<std::shared_ptr<State>> desiredStates;
    cur = state;
    while (cur != nullptr) {
        desiredStates.push_back(cur);
        cur = cur->GetParentState();
    }

    // Remove the common tail.
    while (!currentStates.empty() && !desiredStates.empty() && currentStates.back() == desiredStates.back()) {
        currentStates.pop_back();
        desiredStates.pop_back();
    }

    state_ = state;
    for (auto &currentState : currentStates) {
        currentState->Exit();
    }
    for (size_t i = desiredStates.size(); i > 0;) {
        i--;
        desiredStates[i]->Enter();
    }
    ProcessDeferredMessages();
}

void StateMachine::DeferMessage(const Message &msg)
{
    deferredQueue_.push(msg);
}

void StateMachine::ProcessDeferredMessages()
{
    while (!deferredQueue_.empty()) {
        const Message msg = deferredQueue_.front();
        deferredQueue_.pop();
        state_->HandleMessage(msg);
    }
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS