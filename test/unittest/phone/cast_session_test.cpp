/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Test Cast Session.
 * Author: jiangfan
 * Create: 2023-6-1
 */

#include "gtest/gtest.h"
#include "cast_device_data_manager.h"
#include "cast_engine_log.h"
#include "cast_engine_errors.h"
#include "cast_engine_common.h"
#include "cast_session_manager.h"
#include "mirror_player.h"
#include "stream_player.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;
using OHOS::CastEngine::CastEngineService::CastDeviceDataManager;

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-Session-Test");

class CastSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

class TestCastSessionManagerListener : public ICastSessionManagerListener {
public:
    void OnDeviceFound(const std::vector<CastRemoteDevice> &deviceList) override
    {
        static_cast<void>(deviceList);
    }
    void OnSessionCreated(const std::shared_ptr<ICastSession> &castSession) override
    {
        static_cast<void>(castSession);
    }
    void OnServiceDied() override {}
    void OnDeviceOffline(const std::string &deviceId) override
    {
        static_cast<void>(deviceId);
    }
};

class TestCastSessionListener : public ICastSessionListener {
public:
    void OnDeviceState(const DeviceStateInfo &stateInfo) override
    {
        static_cast<void>(stateInfo);
    }
    void OnEvent(const EventId &eventId, const std::string &jsonParam) override
    {
        static_cast<void>(eventId);
        static_cast<void>(jsonParam);
    }
};

class TestDataTransListener : public IDataTransListener {
public:
    void OnBytesReceived(const uint8_t *data, uint32_t len) override {}
    void OnFilesSent(std::string firstFile, int percent) override {}
    void OnFilesReceived(std::string firstFile, int percent, const RcvFdFileMap &fdMap) override {}
};

namespace {
std::shared_ptr<ICastSession> CreateSession()
{
    std::shared_ptr<ICastSession> session;
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    CastSessionManager::GetInstance().RegisterListener(listener);
    CastSessionManager::GetInstance().CreateCastSession(CastSessionProperty {}, session);
    return session;
}
}

void CastSessionTest::SetUpTestCase(void)
{
    constexpr int castPermissionNum = 2;
    const char *perms[castPermissionNum] = {
        "ohos.permission.ACCESS_CAST_ENGINE_MIRROR",
        "ohos.permission.ACCESS_CAST_ENGINE_STREAM",
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,  // Indicates the capsbility list of the sa.
        .permsNum = castPermissionNum,
        .aclsNum = 0,   // acls is the list of rights that can be escalated.
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "cast_session_test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    CLOGI("tokenId is %" PRIu64, tokenId);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void CastSessionTest::TearDownTestCase(void) {}
void CastSessionTest::SetUp(void)
{
    /* Sleep 200ms */
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void CastSessionTest::TearDown(void)
{
    CastSessionManager::GetInstance().Release();
}

/**
 * @tc.name: RegisterListenerTest_001
 * @tc.desc: Test the RegisterListener function. The empty listener is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, RegisterListenerTest_001, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    auto result = session->RegisterListener(nullptr);
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: RegisterListenerTest_002
 * @tc.desc: Test the RegisterListener function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, RegisterListenerTest_002, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_002");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    auto result = session->RegisterListener(std::make_shared<TestCastSessionListener>());
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    session->Release();
}

/**
 * @tc.name: AddDeviceTest_001
 * @tc.desc: Test the AddDevice function. The empty remoteDevice is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, AddDeviceTest_001, TestSize.Level1)
{
    CLOGD("AddDeviceTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    auto result = session->AddDevice(CastRemoteDevice{});
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: AddDeviceTest_002
 * @tc.desc: Test the AddDevice function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, AddDeviceTest_002, TestSize.Level1)
{
    CLOGD("AddDeviceTest_002");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    CastRemoteDevice remoteDevice{};
    remoteDevice.deviceId = "AddDeviceTest_002";
    auto result = session->AddDevice(remoteDevice);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    session->Release();
}

/**
 * @tc.name: AddDeviceTest_003
 * @tc.desc: Test the AddDevice function.Add a device that does not exist.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, AddDeviceTest_003, TestSize.Level1)
{
    CLOGD("AddDeviceTest_003");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    CastRemoteDevice remoteDevice{};
    remoteDevice.deviceId = "AddDeviceTest_003";
    auto result = session->AddDevice(remoteDevice);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    session->Release();
}

/**
 * @tc.name: RemoveDeviceTest_001
 * @tc.desc: Test the RemoveDevice function. The empty remoteDevice is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, RemoveDeviceTest_001, TestSize.Level1)
{
    CLOGD("RemoveDeviceTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    auto result = session->AddDevice(CastRemoteDevice {});
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: RemoveDeviceTest_002
 * @tc.desc: Test the RemoveDevice function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, RemoveDeviceTest_002, TestSize.Level1)
{
    CLOGD("RemoveDeviceTest_002");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    CastRemoteDevice remoteDevice{};
    remoteDevice.deviceId = "RemoveDeviceTest_002";
    session->AddDevice(remoteDevice);
    auto result = session->RemoveDevice(remoteDevice.deviceId);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    session->Release();
}

/**
 * @tc.name: StartAuthTest_001
 * @tc.desc: Test the StartAuth function. The empty authInfo is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, StartAuthTest_001, TestSize.Level1)
{
    CLOGD("StartAuthTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    auto result = session->StartAuth(AuthInfo {});
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: StartAuthTest_002
 * @tc.desc: Test the StartAuth function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, StartAuthTest_002, TestSize.Level1)
{
    CLOGD("StartAuthTest_002");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    AuthInfo authInfo;
    authInfo.deviceId = "StartAuthTest_002";
    CastSessionManager::GetInstance().Release();
    auto result = session->StartAuth(authInfo);
    EXPECT_EQ(result, ERR_NO_PERMISSION);
    session->Release();
}

/**
 * @tc.name: StartAuthTest_003
 * @tc.desc: Test the StartAuth function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, StartAuthTest_003, TestSize.Level1)
{
    CLOGD("StartAuthTest_003");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    AuthInfo authInfo;
    authInfo.deviceId = "StartAuthTest_003";
    auto result = session->StartAuth(authInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    session->Release();
}

/**
 * @tc.name: GetSessionIdTest_001
 * @tc.desc: Test the GetSessionId function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, GetSessionIdTest_001, TestSize.Level1)
{
    CLOGD("GetSessionIdTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    string sessionId;
    CastSessionManager::GetInstance().Release();
    auto result = session->GetSessionId(sessionId);
    EXPECT_EQ(result, ERR_NO_PERMISSION);
}

/**
 * @tc.name: GetSessionIdTest_002
 * @tc.desc: Test the GetSessionId function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, GetSessionIdTest_002, TestSize.Level1)
{
    CLOGD("GetSessionIdTest_002");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    string sessionId;
    auto result = session->GetSessionId(sessionId);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    session->Release();
}

/**
 * @tc.name: SetSessionPropertyTest_001
 * @tc.desc: Test the SetSessionProperty function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, SetSessionPropertyTest_001, TestSize.Level1)
{
    CLOGD("SetSessionPropertyTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    auto result = session->SetSessionProperty(CastSessionProperty {});
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    session->Release();
}

/**
 * @tc.name: CreateMirrorPlayerTest_001
 * @tc.desc: Test the CreateMirrorPlayer function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, CreateMirrorPlayerTest_001, TestSize.Level1)
{
    CLOGD("CreateMirrorPlayerTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    std::shared_ptr<IMirrorPlayer> mirrorPlayer;
    auto result = session->CreateMirrorPlayer(mirrorPlayer);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    session->Release();
}

/**
 * @tc.name: CreateStreamPlayerTest_001
 * @tc.desc: Test the CreateStreamPlayer function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, CreateStreamPlayerTest_001, TestSize.Level1)
{
    CLOGD("CreateStreamPlayerTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    std::shared_ptr<IStreamPlayer> streamPlayer;
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    auto result = session->CreateStreamPlayer(streamPlayer);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    session->Release();
}

/**
 * @tc.name: ReleaseTest_001
 * @tc.desc: Test the Release function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, ReleaseTest_001, TestSize.Level1)
{
    CLOGD("ReleaseTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    auto result = session->Release();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: SendBytesTest_001
 * @tc.desc: Test the SendBytes function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, SetCastModeTest_001, TestSize.Level1)
{
    CLOGD("SetCastModeTest_001");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    string jsonParam = "SetCastModeTest_001";
    CastSessionManager::GetInstance().Release();
    auto result = session->SetCastMode(CastMode {}, jsonParam);
    EXPECT_EQ(result, ERR_NO_PERMISSION);
}

/**
 * @tc.name: SendBytesTest_002
 * @tc.desc: Test the SendBytes function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionTest, SetCastModeTest_002, TestSize.Level1)
{
    CLOGD("SendBytesTest_002");
    std::shared_ptr<ICastSession> session = CreateSession();
    ASSERT_NE(session, nullptr);
    session->RegisterListener(std::make_shared<TestCastSessionListener>());
    string jsonParam = "SetCastModeTest_001";
    auto result = session->SetCastMode(CastMode {}, jsonParam);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}
}
}
}
