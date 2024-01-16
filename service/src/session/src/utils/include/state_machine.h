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

#ifndef SERVICE_SRC_SESSION_SRC_UTILS_INCLUDE_STATE_MACHINE_H
#define SERVICE_SRC_SESSION_SRC_UTILS_INCLUDE_STATE_MACHINE_H

#include <queue>
#include <memory>
#include "handler.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class State {
public:
    explicit State(const std::shared_ptr<State> &parentState = nullptr) : parentState_(parentState) {}
    std::shared_ptr<State> GetParentState()
    {
        return parentState_;
    }

protected:
    virtual ~State() = default;
    virtual void Enter() {}
    virtual void Exit() {}
    virtual bool HandleMessage(const Message &msg) = 0;

private:
    friend class StateMachine;
    std::shared_ptr<State> parentState_;
    DISALLOW_EVIL_CONSTRUCTORS(State);
};

class StateMachine : public Handler {
public:
    StateMachine() = default;

protected:
    ~StateMachine() override = default;
    void HandleMessage(const Message &msg) override;
    void TransferState(const std::shared_ptr<State> &state);
    void DeferMessage(const Message &msg);

private:
    void ProcessDeferredMessages();
    std::shared_ptr<State> state_;
    std::queue<Message> deferredQueue_;
    DISALLOW_EVIL_CONSTRUCTORS(StateMachine);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
