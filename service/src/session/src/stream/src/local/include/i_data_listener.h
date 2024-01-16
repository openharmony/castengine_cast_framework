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