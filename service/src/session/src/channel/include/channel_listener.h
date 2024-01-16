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
 * Description: channel listener
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CHANNEL_LISTENER_H
#define CHANNEL_LISTENER_H

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using OpenFdFunc = int (*)(const char *filename, int32_t flag, int32_t mode);
using CloseFdFunc = int (*)(int32_t fd);

constexpr int FILE_SCHEMA_MAX_LENGTH = 32;
struct ChannelFileSchema {
    const char name[FILE_SCHEMA_MAX_LENGTH];
    OpenFdFunc openFd;
    CloseFdFunc closeFd;
};

class IChannelListener {
public:
    IChannelListener() = default;
    virtual ~IChannelListener() = default;

    virtual void OnDataReceived(const uint8_t *buffer, unsigned int length, long timeCost) {}
    virtual void OnSendFileProcess(int percent) {}
    virtual void OnFilesSent(std::string firstFile, int percent) {}
    virtual void OnFilesReceived(std::string files, int percent) {}
    virtual void OnFileTransError() {}
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CHANNEL_LISTENER_H
