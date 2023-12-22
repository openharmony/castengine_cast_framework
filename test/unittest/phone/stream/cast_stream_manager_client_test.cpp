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
 * Create: 2023-6-9
 */

#include "gtest/gtest.h"
#include "cast_engine_common.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "cast_stream_manager_client.h"
#include "stream_player_listener_impl_stub.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;
using OHOS::CastEngine::CastEngineClient::StreamPlayerListenerImplStub;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Stream-Manager-client-Test");

using nlohmann::json;

class CastStreamManagerClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

class TestCastStreamListener : public ICastStreamListener {
public:
    bool SendActionToPeers(int action, const std::string &param) override
    {
        return true;
    }
    void OnRenderReady(bool isReady) override {}
    void OnEvent(EventId eventId, const std::string &data) override {}
};

namespace {
std::shared_ptr<CastStreamManagerClient> CreateCastStreamManagerClient()
{
    auto listener = std::make_shared<TestCastStreamListener>();
    return std::make_shared<CastStreamManagerClient>(listener);
}
}

void CastStreamManagerClientTest::SetUpTestCase(void)
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
        .processName = "cast_stream_manager_test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    CLOGI("tokenId is %" PRIu64, tokenId);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void CastStreamManagerClientTest::TearDownTestCase(void) {}
void CastStreamManagerClientTest::SetUp(void) {}
void CastStreamManagerClientTest::TearDown(void) {}

/**
 * @tc.name: ProcessActionsEventTest_001
 * @tc.desc: Test the ProcessActionsEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerClientTest, ProcessActionsEventTest_001, TestSize.Level1)
{
    CLOGD("ProcessActionsEventTest_001");
    auto castStreamManager = CreateCastStreamManagerClient();
    ASSERT_NE(castStreamManager, nullptr);
    std::string param = "ProcessActionsEventTest_001";
    castStreamManager->ProcessActionsEvent(ICastStreamManager::MODULE_EVENT_ID_CONTROL_EVENT, param);
}

/**
 * @tc.name: NotifyPeerNextTest_001
 * @tc.desc: Test the NotifyPeerNext function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerClientTest, NotifyPeerNextTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerNextTest_001");
    auto castStreamManager = CreateCastStreamManagerClient();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerNext();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerPreviousTest_001
 * @tc.desc: Test the NotifyPeerPrevious function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerClientTest, NotifyPeerPreviousTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerPreviousTest_001");
    auto castStreamManager = CreateCastStreamManagerClient();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerPrevious();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerFastForwardTest_001
 * @tc.desc: Test the NotifyPeerFastForward function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerClientTest, NotifyPeerFastForwardTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerFastForwardTest_001");
    auto castStreamManager = CreateCastStreamManagerClient();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerFastForward(1000);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerFastRewindTest_001
 * @tc.desc: Test the NotifyPeerFastRewind function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerClientTest, NotifyPeerFastRewindTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerFastRewindTest_001");
    auto castStreamManager = CreateCastStreamManagerClient();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerFastRewind(1000);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: OnEventTest_001
 * @tc.desc: Test the OnEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerClientTest, OnEventTest_001, TestSize.Level1)
{
    CLOGD("OnEventTest_001");
    auto castStreamManager = CreateCastStreamManagerClient();
    ASSERT_NE(castStreamManager, nullptr);
    std::string data;
    castStreamManager->OnEvent(EventId::EVENT_BEGIN, data);
}

/**
 * @tc.name: OnEventTest_002
 * @tc.desc: Test the OnEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerClientTest, OnEventTest_002, TestSize.Level1)
{
    CLOGD("OnEventTest_002");
    auto castStreamManager = std::make_shared<CastStreamManagerClient>(nullptr);
    ASSERT_NE(castStreamManager, nullptr);
    std::string data;
    castStreamManager->OnEvent(EventId::EVENT_BEGIN, data);
}
}
}
}
