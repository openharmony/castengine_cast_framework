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
#include "cast_stream_manager.h"
#include "i_cast_stream_manager.h"
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
DEFINE_CAST_ENGINE_LABEL("Cast-Stream-Player-Manager-Test");

using nlohmann::json;

class CastStreamManagerTest : public testing::Test {
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
std::shared_ptr<CastStreamManager> CreateCastStreamManager()
{
    auto listener = std::make_shared<TestCastStreamListener>();
    EndType endType;
    return std::make_shared<CastStreamManager>(listener, endType);
}
}

void CastStreamManagerTest::SetUpTestCase(void)
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

void CastStreamManagerTest::TearDownTestCase(void) {}
void CastStreamManagerTest::SetUp(void) {}
void CastStreamManagerTest::TearDown(void) {}

/**
 * @tc.name: ProcessActionsEventTest_001
 * @tc.desc: Test the ProcessActionsEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, ProcessActionsEventTest_001, TestSize.Level1)
{
    CLOGD("ProcessActionsEventTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    std::string param = "ProcessActionsEventTest_001";
    castStreamManager->ProcessActionsEvent(CastStreamManager::MODULE_EVENT_ID_CONTROL_EVENT, param);
}

/**
 * @tc.name: NotifyPeerNextTest_001
 * @tc.desc: Test the NotifyPeerNext function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerNextTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerNextTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerNext();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerPreviousTest_001
 * @tc.desc: Test the NotifyPeerPrevious function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerPreviousTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerPreviousTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerPrevious();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerNextRequestTest_001
 * @tc.desc: Test the NotifyPeerNextRequest function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerNextRequestTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerNextRequestTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerNextRequest();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerPreviousRequestTest_001
 * @tc.desc: Test the NotifyPeerPreviousRequest function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerPreviousRequestTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerPreviousRequestTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerPreviousRequest();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerSeekDoneTest_001
 * @tc.desc: Test the NotifyPeerSeekDone function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerSeekDoneTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerSeekDoneTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    int position = 0; // start position
    auto result = castStreamManager->NotifyPeerSeekDone(position);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerPlayerStatusChangedTest_001
 * @tc.desc: Test the NotifyPeerPlayerStatusChanged function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerPlayerStatusChangedTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerPlayerStatusChangedTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerPlayerStatusChanged(PlayerStates::PLAYER_STATE_ERROR, true);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerPositionChangedTest_001
 * @tc.desc: Test the NotifyPeerPositionChanged function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerPositionChangedTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerPositionChangedTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    int position = 0;
    int bufferPosition = 0;
    int duration = 0;
    auto result = castStreamManager->NotifyPeerPositionChanged(position, bufferPosition, duration);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerMediaItemChangedTest_001
 * @tc.desc: Test the NotifyPeerMediaItemChanged function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerMediaItemChangedTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerMediaItemChangedTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    MediaInfo mediaInfo{"NotifyPeerMediaItemChangedTest_001", "NotifyPeerMediaItemChangedTest_001"};
    auto result = castStreamManager->NotifyPeerMediaItemChanged(mediaInfo);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerVolumeChangedTest_001
 * @tc.desc: Test the NotifyPeerVolumeChanged function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerVolumeChangedTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerVolumeChangedTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    int volume = 0;
    int maxVolume = 0;
    auto result = castStreamManager->NotifyPeerVolumeChanged(volume, maxVolume);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerRepeatModeChangedTest_001
 * @tc.desc: Test the NotifyPeerRepeatModeChanged function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerRepeatModeChangedTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerRepeatModeChangedTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerRepeatModeChanged(LoopMode::LOOP_MODE_SEQUENCE);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerPlaySpeedChangedTest_001
 * @tc.desc: Test the NotifyPeerPlaySpeedChanged function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerPlaySpeedChangedTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerPlaySpeedChangedTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerPlaySpeedChanged(PlaybackSpeed::SPEED_FORWARD_0_75_X);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerPlayerErrorTest_001
 * @tc.desc: Test the NotifyPeerPlayerError function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerPlayerErrorTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerPlayerErrorTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    int errCode = CAST_ENGINE_ERROR;
    std::string errMsg;
    auto result = castStreamManager->NotifyPeerPlayerError(errCode, errMsg);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerCreateChannelTest_001
 * @tc.desc: Test the NotifyPeerCreateChannel function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerCreateChannelTest_001, TestSize.Level1)
{
    CLOGD("NotifyPeerCreateChannelTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerCreateChannel();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NotifyPeerCreateChannelTest_002
 * @tc.desc: Test the NotifyPeerCreateChannel function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, NotifyPeerCreateChannelTest_002, TestSize.Level1)
{
    CLOGD("NotifyPeerCreateChannelTest_002");
    EndType endType;
    auto castStreamManager = std::make_shared<CastStreamManager>(nullptr, endType);
    ASSERT_NE(castStreamManager, nullptr);
    auto result = castStreamManager->NotifyPeerCreateChannel();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: OnEventTest_001
 * @tc.desc: Test the OnEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, OnEventTest_001, TestSize.Level1)
{
    CLOGD("OnEventTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    std::string data;
    castStreamManager->OnEvent(EventId::EVENT_BEGIN, data);
}

/**
 * @tc.name: OnEventTest_002
 * @tc.desc: Test the OnEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, OnEventTest_002, TestSize.Level1)
{
    CLOGD("OnEventTest_002");
    EndType endType;
    auto castStreamManager = std::make_shared<CastStreamManager>(nullptr, endType);
    ASSERT_NE(castStreamManager, nullptr);
    std::string data;
    castStreamManager->OnEvent(EventId::EVENT_BEGIN, data);
}

/**
 * @tc.name: OnRenderReadyTest_001
 * @tc.desc: Test the OnRenderReady function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamManagerTest, OnRenderReadyTest_001, TestSize.Level1)
{
    CLOGD("OnRenderReadyTest_001");
    auto castStreamManager = CreateCastStreamManager();
    ASSERT_NE(castStreamManager, nullptr);
    castStreamManager->OnRenderReady(true);
}
}
}
}