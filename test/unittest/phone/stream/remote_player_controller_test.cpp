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
 * Create: 2023-6-29
 */

#include "gtest/gtest.h"
#include "cast_engine_common.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "remote_player_controller.h"
#include "cast_stream_manager_client.h"
#include "i_cast_stream_listener.h"
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
DEFINE_CAST_ENGINE_LABEL("Remote-Player-Controller-Test");

class RemotePlayerControllerTest : public testing::Test {
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
sptr<IStreamPlayerListenerImpl> g_listenerStub;

void ReleaseCallback()
{
    CLOGD("Release Callback");
}
}

void RemotePlayerControllerTest::SetUpTestCase(void)
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
        .processName = "remote_player_controller_test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    CLOGI("tokenId is %" PRIu64, tokenId);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
    g_listenerStub = new (std::nothrow) StreamPlayerListenerImplStub(nullptr);
    ASSERT_NE(g_listenerStub, nullptr);
}

void RemotePlayerControllerTest::TearDownTestCase(void) {}
void RemotePlayerControllerTest::SetUp(void) {}
void RemotePlayerControllerTest::TearDown(void) {}

/**
 * @tc.name: RegisterListenerTest_001
 * @tc.desc: Test the RegisterListener function. The empty listener is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, RegisterListenerTest_001, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->RegisterListener(nullptr);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: RegisterListenerTest_002
 * @tc.desc: Test the RegisterListener function. non-empty construction.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, RegisterListenerTest_002, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(callback, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->RegisterListener(g_listenerStub);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: RegisterListenerTest_003
 * @tc.desc: Test the RegisterListener function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, RegisterListenerTest_003, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_003");
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->RegisterListener(g_listenerStub);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: UnregisterListener_001
 * @tc.desc: Test the UnregisterListener function. empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, UnregisterListener_001, TestSize.Level1)
{
    CLOGD("UnregisterListener_001");
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: UnregisterListener_002
 * @tc.desc: Test the RegisterListener function. non-empty construction.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, UnregisterListener_002, TestSize.Level1)
{
    CLOGD("UnregisterListener_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(callback, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: SetSurface_001
 * @tc.desc: Test the SetSurface function.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetSurface_001, TestSize.Level1)
{
    CLOGD("SetSurface_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->SetSurface(nullptr);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: LoadTest_001
 * @tc.desc: Test the Load function. empty callback and fileChannel.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, LoadTest_001, TestSize.Level1)
{
    CLOGD("LoadTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    MediaInfo mediaInfo{"LoadTest_001", "LoadTest_001"};
    auto result = remoteController->Load(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: LoadTest_002
 * @tc.desc: Test the Load function. non-empty callback and fileChannel.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, LoadTest_002, TestSize.Level1)
{
    CLOGD("LoadTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(callback, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    MediaInfo mediaInfo{"LoadTest_002", "LoadTest_002"};
    auto result = remoteController->Load(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: PlayTest_001
 * @tc.desc: Test the Play function. empty fileChannel.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PlayTest_001, TestSize.Level1)
{
    CLOGD("PlayTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    MediaInfo mediaInfo{"PlayTest_001", "PlayTest_001"};
    auto result = remoteController->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: PlayTest_002
 * @tc.desc: Test the Play function. empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PlayTest_002, TestSize.Level1)
{
    CLOGD("PlayTest_002");
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    MediaInfo mediaInfo{"PlayTest_002", "PlayTest_002"};
    auto result = remoteController->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: PlayTest_003
 * @tc.desc: Test the Play function. not empty callback and fileChannel.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PlayTest_003, TestSize.Level1)
{
    CLOGD("PlayTest_003");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(callback, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    MediaInfo mediaInfo{"PlayTest_003", "PlayTest_003"};
    auto result = remoteController->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: PlayTest_004
 * @tc.desc: Test the Play function. empty fileChannel.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PlayTest_004, TestSize.Level1)
{
    CLOGD("PlayTest_004");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    MediaInfo mediaInfo{"PlayTest_003", "PlayTest_003"};
    auto result = remoteController->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: PlayTest_005
 * @tc.desc: Test the Play function.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PlayTest_005, TestSize.Level1)
{
    CLOGD("PlayTest_005");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int index = 0; // non-boundary value
    auto result = remoteController->Play(index);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: PlayTest_006
 * @tc.desc: Test the Play function. empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PlayTest_006, TestSize.Level1)
{
    CLOGD("PlayTest_006");
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Play();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: PlayTest_007
 * @tc.desc: Test the Play function. not empty callback and fileChannel.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PlayTest_007, TestSize.Level1)
{
    CLOGD("PlayTest_007");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto fileChannel = std::make_shared<CastLocalFileChannelServer>();
    auto remoteController = std::make_shared<RemotePlayerController>(callback, fileChannel);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Play();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: PauseTest_001
 * @tc.desc: Test the Pause function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PauseTest_001, TestSize.Level1)
{
    CLOGD("PauseTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Pause();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: PauseTest_002
 * @tc.desc: Test the Pause function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PauseTest_002, TestSize.Level1)
{
    CLOGD("PauseTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Pause();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: StopTest_001
 * @tc.desc: Test the Stop function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, StopTest_001, TestSize.Level1)
{
    CLOGD("StopTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Stop();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: StopTest_002
 * @tc.desc: Test the Stop function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, StopTest_002, TestSize.Level1)
{
    CLOGD("StopTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Stop();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: NextTest_001
 * @tc.desc: Test the Next function.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, NextTest_001, TestSize.Level1)
{
    CLOGD("NextTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Next();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: PreviousTest_001
 * @tc.desc: Test the Previous function.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, PreviousTest_001, TestSize.Level1)
{
    CLOGD("PreviousTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Previous();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: SeekTest_001
 * @tc.desc: Test the Seek function.empty callback.Abnormal data.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SeekTest_001, TestSize.Level1)
{
    CLOGD("SeekTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int position = -1; // Abnormal data
    auto result = remoteController->Seek(position);
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: SeekTest_002
 * @tc.desc: Test the Seek function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SeekTest_002, TestSize.Level1)
{
    CLOGD("SeekTest_002");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int position = 0; // normal data
    auto result = remoteController->Seek(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: SeekTest_003
 * @tc.desc: Test the Seek function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SeekTest_003, TestSize.Level1)
{
    CLOGD("SeekTest_003");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int position = 0; // normal data
    auto result = remoteController->Seek(position);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: FastForwardTest_001
 * @tc.desc: Test the FastForward function.empty callback.Abnormal data.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, FastForwardTest_001, TestSize.Level1)
{
    CLOGD("FastForwardTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int delta = -1; // Abnormal data
    auto result = remoteController->FastForward(delta);
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: FastForwardTest_002
 * @tc.desc: Test the FastForward function.empty callback.Normal data.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, FastForwardTest_002, TestSize.Level1)
{
    CLOGD("FastForwardTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int delta = 1000; // normal data
    auto result = remoteController->FastForward(delta);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: FastRewindTest_001
 * @tc.desc: Test the FastRewind function.empty callback.Abnormal data.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, FastRewindTest_001, TestSize.Level1)
{
    CLOGD("FastRewindTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int delta = -1; // Abnormal data
    auto result = remoteController->FastRewind(delta);
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: FastRewindTest_002
 * @tc.desc: Test the FastRewind function.empty callback.Normal data.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, FastRewindTest_002, TestSize.Level1)
{
    CLOGD("FastRewindTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int delta = 1000; // normal data
    auto result = remoteController->FastRewind(delta);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: SetVolumeTest_001
 * @tc.desc: Test the SetVolume function.Abnormal data.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetVolumeTest_001, TestSize.Level1)
{
    CLOGD("SetVolumeTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int volume = -1; // Abnormal data
    auto result = remoteController->SetVolume(volume);
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: SetVolumeTest_002
 * @tc.desc: Test the SetVolume function.Out of boundary value.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetVolumeTest_002, TestSize.Level1)
{
    CLOGD("SetVolumeTest_002");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int volume = 101; // Out of boundary value
    auto result = remoteController->SetVolume(volume);
    EXPECT_EQ(result, ERR_INVALID_PARAM);
}

/**
 * @tc.name: SetVolumeTest_003
 * @tc.desc: Test the SetVolume function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetVolumeTest_003, TestSize.Level1)
{
    CLOGD("SetVolumeTest_003");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int volume = 0; // normal data
    auto result = remoteController->SetVolume(volume);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: SetVolumeTest_004
 * @tc.desc: Test the SetVolume function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetVolumeTest_004, TestSize.Level1)
{
    CLOGD("SetVolumeTest_004");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int volume = 0; // normal data
    auto result = remoteController->SetVolume(volume);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: SetLoopModeTest_001
 * @tc.desc: Test the SetLoopMode function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetLoopModeTest_001, TestSize.Level1)
{
    CLOGD("SetLoopModeTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->SetLoopMode(LoopMode::LOOP_MODE_SEQUENCE);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: SetLoopModeTest_002
 * @tc.desc: Test the SetLoopMode function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetLoopModeTest_002, TestSize.Level1)
{
    CLOGD("SetLoopModeTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->SetLoopMode(LoopMode::LOOP_MODE_SEQUENCE);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: SetSpeedTest_001
 * @tc.desc: Test the SetSpeed function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetSpeedTest_001, TestSize.Level1)
{
    CLOGD("SetSpeedTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->SetSpeed(PlaybackSpeed::SPEED_FORWARD_0_75_X);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: SetSpeedTest_002
 * @tc.desc: Test the SetSpeed function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, SetSpeedTest_002, TestSize.Level1)
{
    CLOGD("SetSpeedTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->SetSpeed(PlaybackSpeed::SPEED_FORWARD_0_75_X);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: GetPlayerStatusTest_001
 * @tc.desc: Test the GetPlayerStatus function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetPlayerStatusTest_001, TestSize.Level1)
{
    CLOGD("GetPlayerStatusTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    PlayerStates playerStates;
    auto result = remoteController->GetPlayerStatus(playerStates);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: GetPlayerStatusTest_002
 * @tc.desc: Test the GetPlayerStatus function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetPlayerStatusTest_002, TestSize.Level1)
{
    CLOGD("GetPlayerStatusTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    PlayerStates playerStates;
    auto result = remoteController->GetPlayerStatus(playerStates);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: GetPositionTest_001
 * @tc.desc: Test the GetPosition function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetPositionTest_001, TestSize.Level1)
{
    CLOGD("GetPositionTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int position;
    auto result = remoteController->GetPosition(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: GetPositionTest_002
 * @tc.desc: Test the GetPosition function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetPositionTest_002, TestSize.Level1)
{
    CLOGD("GetPositionTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int position;
    auto result = remoteController->GetPosition(position);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: GetDurationTest_001
 * @tc.desc: Test the GetDuration function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetDurationTest_001, TestSize.Level1)
{
    CLOGD("GetDurationTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int position;
    auto result = remoteController->GetDuration(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: GetDurationTest_002
 * @tc.desc: Test the GetDuration function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetDurationTest_002, TestSize.Level1)
{
    CLOGD("GetDurationTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int duration;
    auto result = remoteController->GetDuration(duration);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: GetVolumeTest_001
 * @tc.desc: Test the GetVolume function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetVolumeTest_001, TestSize.Level1)
{
    CLOGD("GetVolumeTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int volume;
    int maxVolume;
    auto result = remoteController->GetVolume(volume, maxVolume);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: GetVolumeTest_002
 * @tc.desc: Test the GetVolume function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetVolumeTest_002, TestSize.Level1)
{
    CLOGD("GetVolumeTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    int volume;
    int maxVolume;
    auto result = remoteController->GetVolume(volume, maxVolume);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: GetLoopModeTest_001
 * @tc.desc: Test the GetLoopMode function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetLoopModeTest_001, TestSize.Level1)
{
    CLOGD("GetLoopModeTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    LoopMode loopMode;
    auto result = remoteController->GetLoopMode(loopMode);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: GetLoopModeTest_002
 * @tc.desc: Test the GetLoopMode function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetLoopModeTest_002, TestSize.Level1)
{
    CLOGD("GetLoopModeTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    LoopMode loopMode;
    auto result = remoteController->GetLoopMode(loopMode);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: GetPlaySpeedTest_001
 * @tc.desc: Test the GetPlaySpeed function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetPlaySpeedTest_001, TestSize.Level1)
{
    CLOGD("GetPlaySpeedTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    PlaybackSpeed playbackSpeed;
    auto result = remoteController->GetPlaySpeed(playbackSpeed);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: GetPlaySpeedTest_002
 * @tc.desc: Test the GetPlaySpeed function.non empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetPlaySpeedTest_002, TestSize.Level1)
{
    CLOGD("GetPlaySpeedTest_002");
    auto callback = std::make_shared<CastStreamManagerClient>(std::make_shared<TestCastStreamListener>());
    auto remoteController = std::make_shared<RemotePlayerController>(callback, nullptr);
    ASSERT_NE(remoteController, nullptr);
    PlaybackSpeed playbackSpeed;
    auto result = remoteController->GetPlaySpeed(playbackSpeed);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: GetMediaInfoHolderTest_001
 * @tc.desc: Test the GetMediaInfoHolder function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, GetMediaInfoHolderTest_001, TestSize.Level1)
{
    CLOGD("GetMediaInfoHolderTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    MediaInfoHolder mediaInfoHolder;
    auto result = remoteController->GetMediaInfoHolder(mediaInfoHolder);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: ReleaseTest_001
 * @tc.desc: Test the Release function.empty callback.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, ReleaseTest_001, TestSize.Level1)
{
    CLOGD("ReleaseTest_001");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    auto result = remoteController->Release();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: ReleaseTest_002
 * @tc.desc: Test the Release function.Setting Callback in Advance.
 * @tc.type: FUNC
 */
HWTEST_F(RemotePlayerControllerTest, ReleaseTest_002, TestSize.Level1)
{
    CLOGD("ReleaseTest_002");
    auto remoteController = std::make_shared<RemotePlayerController>(nullptr, nullptr);
    ASSERT_NE(remoteController, nullptr);
    remoteController->SetSessionCallbackForRelease(ReleaseCallback);
    auto result = remoteController->Release();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}
}
}
}