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
 * Description: supply cast stream player utils.
 * Author: zhangjingnan
 * Create: 2023-8-11
 */

#include "cast_stream_player_utils.h"

#include "audio_system_manager.h"
#include "cast_engine_log.h"
#include "cast_stream_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Stream-Player-Utils");

int CastStreamPlayerUtils::GetVolume()
{
    CLOGD("GetVolume in");
    auto audioSystemMgr = AudioStandard::AudioSystemManager::GetInstance();
    if (audioSystemMgr == nullptr) {
        CLOGE("Audio manager is null");
        return CAST_STREAM_INT_INVALID;
    }
    // In current audioStandard structure, all type will be converted into STREAM_MUSIC.
    auto streamType = AudioStandard::AudioVolumeType::STREAM_MUSIC;
    int originalVolume = audioSystemMgr->GetVolume(streamType);
    int maxVolume = audioSystemMgr->GetMaxVolume(streamType);
    if (maxVolume <= 0) {
        CLOGE("Max volume <= 0");
        return CAST_STREAM_INT_INVALID;
    }
    int initVolume = originalVolume * CAST_STREAM_FULL_VOLUME / maxVolume;
    CLOGI("Init initVolume is %{public}d", initVolume);
    return initVolume;
}

int CastStreamPlayerUtils::GetMaxVolume()
{
    CLOGD("GetMaxVolume in");
    auto audioSystemMgr = AudioStandard::AudioSystemManager::GetInstance();
    if (audioSystemMgr == nullptr) {
        CLOGE("Audio manager is null");
        return CAST_STREAM_INT_INVALID;
    }
    // In current audioStandard structure, all type will be converted into STREAM_MUSIC.
    auto streamType = AudioStandard::AudioVolumeType::STREAM_MUSIC;
    return audioSystemMgr->GetMaxVolume(streamType);
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS