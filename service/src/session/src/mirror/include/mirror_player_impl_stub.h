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
 * Description: supply cast mirrpr player implement stub class.
 * Author: zhangjingnan
 * Create: 2023-05-27
 */

#ifndef MIRROR_PLAYER_IMPL_STUB_H
#define MIRROR_PLAYER_IMPL_STUB_H

#include "cast_stub_helper.h"
#include "i_mirror_player_impl.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class MirrorPlayerImplStub : public IRemoteStub<IMirrorPlayerImpl> {
public:
    MirrorPlayerImplStub();

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DECLARE_STUB_TASK_MAP(MirrorPlayerImplStub);

    int32_t DoPlayTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoPauseTask(MessageParcel &data, MessageParcel &reply);
    int32_t DoSetSurface(MessageParcel &data, MessageParcel &reply);
    int32_t DoSetAppInfo(MessageParcel &data, MessageParcel &reply);
    int32_t DoDeliverInputEvent(MessageParcel &data, MessageParcel &reply);
    int32_t DoInjectEvent(MessageParcel &data, MessageParcel &reply);
    int32_t DoRelease(MessageParcel &data, MessageParcel &reply);
    int32_t DoGetDisplayId(MessageParcel &data, MessageParcel &reply);
    int32_t DoResizeVirtualScreen(MessageParcel &data, MessageParcel &reply);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif