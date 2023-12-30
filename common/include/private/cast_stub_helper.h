/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply a helper to define some methods for stub object.
 * Author: zhangge
 * Create: 2022-06-15
 */

#ifndef CAST_STUB_HELPER_H
#define CAST_STUB_HELPER_H

#include <map>
#include "cast_engine_log.h"
#include "iremote_stub.h"

namespace OHOS {
namespace CastEngine {
#define DECLARE_STUB_TASK_MAP(className)                                              \
private:                                                                              \
    using className##Func = int32_t (className::*)(MessageParcel &, MessageParcel &); \
    std::map<uint32_t, className##Func> taskMap_

#define FILL_SINGLE_STUB_TASK(id, func) taskMap_[id] = (func)

#define RETURN_IF_WRONG_INTERFACE_TOKEN(data)             \
    if (GetDescriptor() != (data).ReadInterfaceToken()) { \
        CLOGE("Invalid interface token");                 \
        return ERR_FLATTEN_OBJECT;                        \
    }

#define RETRUEN_IF_WRONG_TASK(code, data, reply, option)                      \
    do {                                                                      \
        RETURN_IF_WRONG_INTERFACE_TOKEN(data);                                \
        if (taskMap_.count(code) == 0) {                                      \
            CLOGE("Invalid code:%d", code);                                   \
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option); \
        }                                                                     \
    } while (0)

#define EXECUTE_SINGLE_STUB_TASK(code, data, reply) (this->*taskMap_[code])(data, reply)
} // namespace CastEngine
} // namespace OHOS

#endif
