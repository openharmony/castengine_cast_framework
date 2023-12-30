/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: supply data listener interface definition
 * Author: huangchanggui
 * Create: 2023-04-20
 */

#ifndef I_DATA_LISTENER_H
#define I_DATA_LISTENER_H

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class IDataListener {
public:
    virtual ~IDataListener() = default;
    virtual bool OnBytesReceived(const std::string &fileId, const uint8_t *bytes, int64_t offset, int64_t length) = 0;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif