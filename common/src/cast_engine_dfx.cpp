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
 * Description: cast engine dfx.
 * Author: wangxueshuang
 * Create: 2023-06-05
 */

#include "cast_engine_dfx.h"

#include <string>

#include "cast_engine_log.h"
#include "hisysevent.h"
#include "json.hpp"

using nlohmann::json;

namespace OHOS {
namespace CastEngine {
DEFINE_CAST_ENGINE_LABEL("Cast-Dfx");

json CastEngineDfx::jsonSteamInfo_ = {};
json CastEngineDfx::jsonLocalDeviceInfo_ = {};
json CastEngineDfx::jsonRemoteDeviceInfo_ = {};
json CastEngineDfx::jsonConnectInfo_ = {};
const std::string CastEngineDfx::PACKAGE_NAME = "CastEngineService";
const std::string CastEngineDfx::SEQUENTIAL_ID_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void CastEngineDfx::WriteErrorEvent(int32_t errorCode)
{
    CLOGD("In.");
    HiSysEventWrite(CAST_ENGINE_DFX_DOMAIN_NAME, "CAST_ENGINE_ERR", HiviewDFX::HiSysEvent::EventType::FAULT,
        "ERROR_CODE", errorCode);
}

void CastEngineDfx::SetStreamInfo(const std::string &streamInfoKey, const std::string &streamInfoValue)
{
    CLOGD("In.");
    jsonSteamInfo_[streamInfoKey] = streamInfoValue;
}

std::string CastEngineDfx::GetStreamInfo()
{
    CLOGD("In.");
    return jsonSteamInfo_.dump();
}

void CastEngineDfx::SetLocalDeviceInfo(const std::string &localDeviceInfoKey, const std::string &localDeviceInfoValue)
{
    CLOGD("In.");
    jsonLocalDeviceInfo_[localDeviceInfoKey] = localDeviceInfoValue;
}

std::string CastEngineDfx::GetLocalDeviceInfo()
{
    CLOGD("In.");
    return jsonLocalDeviceInfo_.dump();
}

void CastEngineDfx::SetRemoteDeviceInfo(const std::string &remoteDeviceInfoKey,
    const std::string &remoteDeviceInfoValue)
{
    CLOGD("In.");
    jsonRemoteDeviceInfo_[remoteDeviceInfoKey] = remoteDeviceInfoValue;
}

std::string CastEngineDfx::GetRemoteDeviceInfo()
{
    CLOGD("In.");
    return jsonRemoteDeviceInfo_.dump();
}

void CastEngineDfx::SetConnectInfo(const std::string &connectInfoKey, const std::string &connectInfoValue)
{
    CLOGD("In.");
    jsonConnectInfo_[connectInfoKey] = connectInfoValue;
}

std::string CastEngineDfx::GetConnectInfo()
{
    CLOGD("In.");
    return jsonConnectInfo_.dump();
}

std::string CastEngineDfx::GetSequentialId()
{
    CLOGD("In.");
    std::string sequentialId = "";
    srand(static_cast<unsigned>(time(NULL)));
    for (int i = 0; i < SN_LENGTH; i++) {
        int number = rand() % SEQUENTIAL_ID_CHARS_LENGTH;
        sequentialId.push_back(SEQUENTIAL_ID_CHARS[number]);
    }
    CLOGD("sequential ID: %s.", sequentialId.c_str());
    return sequentialId;
}

std::string CastEngineDfx::GetBizPackageName()
{
    CLOGD("In.");
    return PACKAGE_NAME;
}

int GetBIZSceneType(int protocols)
{
    static std::map<ProtocolType, BIZSceneType> typeRelation {
        {ProtocolType::CAST_PLUS_MIRROR, BIZSceneType::CAST_MIRROR},
        {ProtocolType::CAST_PLUS_STREAM, BIZSceneType::CAST_STREAM},
        {ProtocolType::DLNA, BIZSceneType::DLNA},
        {ProtocolType::MIRACAST, BIZSceneType::MIRACAST},
        {ProtocolType::COOPERATION, BIZSceneType::COOPERATION}
    };

    auto sceneType = BIZSceneType::CAST_MIRROR;
    if (auto itr = typeRelation.find(static_cast<ProtocolType>(protocols)); itr != typeRelation.cend()) {
        sceneType = itr->second;
    }

    return static_cast<int>(sceneType);
}

void HiSysEventWriteWrap(const std::string& funcName, const RadarParamInt& paramInt,
    const RadarParamString& paramStr)
{
    int32_t bizState = -1;
    if (auto it = paramInt.find("BIZ_STATE"); it != paramInt.cend()) {
        bizState = it->second;
    }

    if (bizState == -1) {
        HiSysEventWrite(
            CAST_ENGINE_DFX_DOMAIN_NAME, EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "ORG_PKG", ORG_PKG,
            "FUNC", funcName,
            "BIZ_SCENE", paramInt.at("BIZ_SCENE"),
            "BIZ_STAGE", paramInt.at("BIZ_STAGE"),
            "STAGE_RES", paramInt.at("STAGE_RES"),
            "ERROR_CODE", paramInt.at("ERROR_CODE"),
            "TO_CALL_PKG", paramStr.at("TO_CALL_PKG"),
            "LOCAL_SESS_NAME", paramStr.at("LOCAL_SESS_NAME"),
            "PEER_SESS_NAME", paramStr.at("PEER_SESS_NAME"),
            "PEER_UDID", paramStr.at("PEER_UDID"));
    } else {
        HiSysEventWrite(
            CAST_ENGINE_DFX_DOMAIN_NAME, EVENT_NAME, HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "ORG_PKG", ORG_PKG,
            "FUNC", funcName,
            "BIZ_SCENE", paramInt.at("BIZ_SCENE"),
            "BIZ_STATE", paramInt.at("BIZ_STATE"),
            "BIZ_STAGE", paramInt.at("BIZ_STAGE"),
            "STAGE_RES", paramInt.at("STAGE_RES"),
            "ERROR_CODE", paramInt.at("ERROR_CODE"),
            "TO_CALL_PKG", paramStr.at("TO_CALL_PKG"),
            "LOCAL_SESS_NAME", paramStr.at("LOCAL_SESS_NAME"),
            "PEER_SESS_NAME", paramStr.at("PEER_SESS_NAME"),
            "PEER_UDID", paramStr.at("PEER_UDID"));
    }
}
} // namespace CastEngine
} // namespace OHOS
