/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
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