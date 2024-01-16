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
 * Description: Cast session listener implement interface.
 * Author: zhangge
 * Create: 2022-6-15
 */

#ifndef I_CAST_SESSION_LISTENER_IMPL_H
#define I_CAST_SESSION_LISTENER_IMPL_H

#include "cast_engine_common.h"
#include "i_cast_session.h"
#include "iremote_broker.h"

namespace OHOS {
namespace CastEngine {
class ICastSessionListenerImpl : public IRemoteBroker, public ICastSessionListener {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.CastEngine.ICastSessionListenerImpl");
    ICastSessionListenerImpl() = default;
    ICastSessionListenerImpl(const ICastSessionListenerImpl &) = delete;
    ICastSessionListenerImpl &operator=(const ICastSessionListenerImpl &) = delete;
    ICastSessionListenerImpl(ICastSessionListenerImpl &&) = delete;
    ICastSessionListenerImpl &operator=(ICastSessionListenerImpl &&) = delete;
    ~ICastSessionListenerImpl() override = default;

protected:
    enum {
        ON_DEVICE_STATE = 1,
        ON_EVENT,
        ON_REMOTE_CTRL_EVENT
    };
};
} // namespace CastEngine
} // namespace OHOS

#endif