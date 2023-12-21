/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Test Connection Manager.
 * Author: jiangfan
 * Create: 2023-5-27
 */

#include <thread>
#include "gtest/gtest.h"
#include "cast_session_manager.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "cast_engine_errors.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Cast-SessionManager-Test");

class CastSessionManagerTest : public testing::Test {
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

namespace {
    constexpr int PROTOCOLS = 1;
}

void CastSessionManagerTest::SetUpTestCase(void)
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
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "cast_session_manager_test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    CLOGI("tokenId is %" PRIu64, tokenId);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void CastSessionManagerTest::TearDownTestCase(void) {}
void CastSessionManagerTest::SetUp(void)
{
    /* Sleep 200ms */
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
void CastSessionManagerTest::TearDown(void) {}

/**
 * @tc.name: RegisterListenerTest_001
 * @tc.desc: Test the RegisterListener function. The empty listener is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, RegisterListenerTest_001, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_001");
    auto result = CastSessionManager::GetInstance().RegisterListener(nullptr);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: RegisterListenerTest_002
 * @tc.desc: Test the RegisterListener function.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, RegisterListenerTest_002, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_002");
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    auto result = CastSessionManager::GetInstance().RegisterListener(listener);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    CastSessionManager::GetInstance().Release();
}

/**
 * @tc.name: UnregisterListenerTest_001
 * @tc.desc: Test the UnregisterListener function in the uninitialized condition.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, UnregisterListenerTest_001, TestSize.Level1)
{
    CLOGD("UnregisterListenerTest_001");
    auto result = CastSessionManager::GetInstance().UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: UnregisterListenerTest_002
 * @tc.desc: Test the UnregisterListener function return true.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, UnregisterListenerTest_002, TestSize.Level1)
{
    CLOGD("UnregisterListenerTest_002");
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    CastSessionManager::GetInstance().RegisterListener(listener);
    auto result = CastSessionManager::GetInstance().UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: ReleaseTest_001
 * @tc.desc: Test the Release function in the uninitialized condition.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, ReleaseTest_001, TestSize.Level1)
{
    CLOGD("ReleaseTest_001");
    auto result = CastSessionManager::GetInstance().Release();
    EXPECT_EQ(result, ERR_NO_PERMISSION);
}

/**
 * @tc.name: ReleaseTest_002
 * @tc.desc: Test the Release function return true.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, ReleaseTest_002, TestSize.Level1)
{
    CLOGD("ReleaseTest_002");
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    CastSessionManager::GetInstance().RegisterListener(listener);
    auto result = CastSessionManager::GetInstance().Release();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: CreateCastSessionTest_001
 * @tc.desc: Test the CreateCastSession function in the uninitialized condition.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, CreateCastSessionTest_001, TestSize.Level1)
{
    CLOGD("CreateCastSessionTest_001");
    std::shared_ptr<ICastSession> castSession;
    CastSessionManager::GetInstance().CreateCastSession(CastSessionProperty {}, castSession);
    EXPECT_EQ(castSession, nullptr);
}

/**
 * @tc.name: CreateCastSessionTest_002
 * @tc.desc: Test the CreateCastSession function return non-empty.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, CreateCastSessionTest_002, TestSize.Level1)
{
    CLOGD("CreateCastSessionTest_002");
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    CastSessionManager::GetInstance().RegisterListener(listener);
    std::shared_ptr<ICastSession> castSession;
    CastSessionManager::GetInstance().CreateCastSession(CastSessionProperty {}, castSession);
    EXPECT_NE(castSession, nullptr);
    CastSessionManager::GetInstance().Release();
}

/**
 * @tc.name: StartDiscoveryTest_001
 * @tc.desc: Test the StartDiscovery function in the uninitialized condition.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, StartDiscoveryTest_001, TestSize.Level1)
{
    CLOGD("StartDiscoveryTest_001");
    auto result = CastSessionManager::GetInstance().StartDiscovery(PROTOCOLS);
    EXPECT_EQ(result, ERR_NO_PERMISSION);
}

/**
 * @tc.name: StartDiscoveryTest_002
 * @tc.desc: Test the StartDiscovery function return true.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, StartDiscoveryTest_002, TestSize.Level1)
{
    CLOGD("StartDiscoveryTest_0200");
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    CastSessionManager::GetInstance().RegisterListener(listener);
    auto result = CastSessionManager::GetInstance().StartDiscovery(PROTOCOLS);
    CastSessionManager::GetInstance().StopDiscovery();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    CastSessionManager::GetInstance().Release();
}

/**
 * @tc.name: SetDiscoverableTest_001
 * @tc.desc: Test the SetDiscoverable function return in the uninitialized condition.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, SetDiscoverableTest_001, TestSize.Level1)
{
    CLOGD("SetDiscoverableTest_001");
    auto result = CastSessionManager::GetInstance().SetDiscoverable(true);
    EXPECT_EQ(result, ERR_NO_PERMISSION);
}

/**
 * @tc.name: SetDiscoverableTest_002
 * @tc.desc: Test the SetDiscoverable function return true.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, SetDiscoverableTest_002, TestSize.Level1)
{
    CLOGD("SetDiscoverableTest_002");
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    CastSessionManager::GetInstance().RegisterListener(listener);
    auto result = CastSessionManager::GetInstance().SetDiscoverable(true);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    CastSessionManager::GetInstance().SetDiscoverable(false);
    CastSessionManager::GetInstance().Release();
}

/**
 * @tc.name: SetDiscoverableTest_003
 * @tc.desc: Test the SetDiscoverable function return true.
 * @tc.type: FUNC
 */
HWTEST_F(CastSessionManagerTest, SetDiscoverableTest_003, TestSize.Level1)
{
    CLOGD("SetDiscoverableTest_003");
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    CastSessionManager::GetInstance().RegisterListener(listener);
    auto result = CastSessionManager::GetInstance().SetDiscoverable(false);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    CastSessionManager::GetInstance().Release();
}
}
}
}
