/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply cast stream listener interface.
 * Author: huangchanggui
 * Create: 2023-02-01
 */

#ifndef I_CAST_STREAM_LISTERNER_H
#define I_CAST_STREAM_LISTERNER_H

#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class ICastStreamListener {
public:
    virtual ~ICastStreamListener() = default;
    virtual bool SendActionToPeers(int action, const std::string &param) = 0;
    virtual void OnRenderReady(bool isReady) = 0;
    virtual void OnEvent(EventId eventId, const std::string &data) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H