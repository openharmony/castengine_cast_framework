/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
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
} // namespace CastEngine
} // namespace OHOS
