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

#ifndef CAST_ENGINE_DFX_H
#define CAST_ENGINE_DFX_H

#include <string>

#include "json.hpp"

namespace OHOS {
namespace CastEngine {
using nlohmann::json;

class CastEngineDfx {
public:
    static void WriteErrorEvent(int32_t errorCode);
    static void SetStreamInfo(const std::string &streamInfoKey, const std::string &streamInfoValue);
    static void SetLocalDeviceInfo(const std::string &localDeviceInfoKey, const std::string &localDeviceInfoValue);
    static void SetRemoteDeviceInfo(const std::string &remoteDeviceInfoKey, const std::string &remoteDeviceInfoValue);
    static void SetConnectInfo(const std::string &connectInfoKey, const std::string &connectInfoValue);
    static std::string GetStreamInfo();
    static std::string GetLocalDeviceInfo();
    static std::string GetRemoteDeviceInfo();
    static std::string GetConnectInfo();
    static std::string GetSequentialId();
    static std::string GetBizPackageName();

private:
    static json jsonSteamInfo_;
    static json jsonLocalDeviceInfo_;
    static json jsonRemoteDeviceInfo_;
    static json jsonConnectInfo_;
    static const std::string PACKAGE_NAME;
    static const std::string SEQUENTIAL_ID_CHARS;
    static const int SN_LENGTH = 32;
    static const int SEQUENTIAL_ID_CHARS_LENGTH = 62;
};

// discovery fail error code
static const int32_t START_DISCOVERY_FAIL = 100;

// connection fail error code
static const int32_t AUTHENTICATE_DEVICE_FAIL = 200;
static const int32_t SOURCE_CREATE_SESSION_SERVER_FAIL = 201;
static const int32_t SINK_CREATE_SESSION_SERVER_FAIL = 202;
static const int32_t OPEN_SESSION_FAIL = 203;
static const int32_t SEND_CONSULTION_DATA_FAIL = 204;

// stream fail error code
static const int32_t PLAYER_INIT_FAIL = 300;

static constexpr char CAST_ENGINE_DFX_DOMAIN_NAME[] = "CAST_ENGINE";
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_ENGINE_DFX_H
