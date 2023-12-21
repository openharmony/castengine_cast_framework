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

#include <surface.h>
#include <consumer_surface.h>
#include <surface_utils.h>
#include "gtest/gtest.h"
#include "cast_engine_common.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "cast_stream_player_manager.h"
#include "cast_stream_manager_server.h"
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

class CastStreamPlayerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

class BufferConsumerListener : public ::OHOS::IBufferConsumerListener {
public:
    void OnBufferAvailable() override {}
};

namespace {
sptr<IStreamPlayerListenerImpl> listenerStub;

std::shared_ptr<CastStreamPlayerManager> CreateStreamPlayManager()
{
    std::shared_ptr<CastStreamManagerServer> callback;
    std::shared_ptr<CastLocalFileChannelClient> fileChannel;
    return std::make_shared<CastStreamPlayerManager>(callback, fileChannel);
}

sptr<IBufferProducer> CreateProducer()
{
    sptr<IConsumerSurface> consSurface;
    sptr<IBufferProducer> producer;
    consSurface = IConsumerSurface::Create();
    if (!consSurface) {
        return nullptr;
    }
    sptr<IBufferConsumerListener> surfaceListener = new BufferConsumerListener();
    if (!surfaceListener) {
        return nullptr;
    }
    consSurface->RegisterConsumerListener(surfaceListener);
    producer = consSurface->GetProducer();
    return producer;
}

void ReleaseCallback()
{
    CLOGD("Release Callback");
}
}

void CastStreamPlayerManagerTest::SetUpTestCase(void)
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
        .processName = "cast_stream_player_manager_test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    CLOGI("tokenId is %" PRIu64, tokenId);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
    listenerStub = new (std::nothrow) StreamPlayerListenerImplStub(nullptr);
    ASSERT_NE(listenerStub, nullptr);
}

void CastStreamPlayerManagerTest::TearDownTestCase(void) {}
void CastStreamPlayerManagerTest::SetUp(void) {}
void CastStreamPlayerManagerTest::TearDown(void) {}

/**
 * @tc.name: RegisterListenerTest_001
 * @tc.desc: Test the RegisterListener function. The empty listener is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, RegisterListenerTest_001, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->RegisterListener(nullptr);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: RegisterListenerTest_002
 * @tc.desc: Test the RegisterListener function.Transfer non-null listener deregistration.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, RegisterListenerTest_002, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->RegisterListener(listenerStub);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->UnregisterListener();
}

/**
 * @tc.name: UnregisterListenerTest_001
 * @tc.desc: Test the UnregisterListener function.Unregistered interception.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, UnregisterListenerTest_001, TestSize.Level1)
{
    CLOGD("UnregisterListenerTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: SetSurfaceTest_001
 * @tc.desc: Test the SetSurface function.The input surface produce is empty.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SetSurfaceTest_001, TestSize.Level1)
{
    CLOGD("SetSurfaceTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    streamPlayManager->RegisterListener(listenerStub);
    auto result = streamPlayManager->SetSurface(nullptr);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: SetSurfaceTest_002
 * @tc.desc: Test the SetSurface function.Construction surface produce.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SetSurfaceTest_002, TestSize.Level1)
{
    CLOGD("SetSurfaceTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    streamPlayManager->RegisterListener(listenerStub);
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    auto result = streamPlayManager->SetSurface(producer);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: LoadTest_001
 * @tc.desc: Test the Load function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, LoadTest_001, TestSize.Level1)
{
    CLOGD("LoadTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"LoadTest_001", "LoadTest_001"};
    auto result = streamPlayManager->Load(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: PlayTest_001
 * @tc.desc: Test the Play function.The input non-boundary value.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PlayTest_001, TestSize.Level1)
{
    CLOGD("PlayTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int index = 1;  // non-boundary value
    auto result = streamPlayManager->Play(index);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}

/**
 * @tc.name: PlayTest_002
 * @tc.desc: Test the Play function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PlayTest_002, TestSize.Level1)
{
    CLOGD("PlayTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"PlayTest_002", "PlayTest_002"};
    auto result = streamPlayManager->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: PlayTest_003
 * @tc.desc: Test the Play function.The input mediaUrl is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PlayTest_003, TestSize.Level1)
{
    CLOGD("PlayTest_003");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"PlayTest_003", "PlayTest_003"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    auto result = streamPlayManager->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

HWTEST_F(CastStreamPlayerManagerTest, PlayTest_004, TestSize.Level1)
{
    CLOGD("PlayTest_004");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"PlayTest_004", "PlayTest_004", "http://PlayTest_004"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    auto result = streamPlayManager->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: PlayTest_005
 * @tc.desc: Test the Play function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PlayTest_005, TestSize.Level1)
{
    CLOGD("PlayTest_005");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->Play();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: PlayTest_006
 * @tc.desc: Test the Play function.Prerequisite: Playable.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PlayTest_006, TestSize.Level1)
{
    CLOGD("PlayTest_006");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"PlayTest_006", "PlayTest_006", "http://PlayTest_006"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    streamPlayManager->Pause();
    auto result = streamPlayManager->Play();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: PauseTest_001
 * @tc.desc: Test the Pause function.Cannot pause when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PauseTest_001, TestSize.Level1)
{
    CLOGD("PauseTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->Pause();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: PauseTest_002
 * @tc.desc: Test the Pause function.When playing, can pause it.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PauseTest_002, TestSize.Level1)
{
    CLOGD("PauseTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"PauseTest_002", "PauseTest_002", "http://PauseTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    auto result = streamPlayManager->Pause();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: StopTest_001
 * @tc.desc: Test the Stop function.Cannot stop when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, StopTest_001, TestSize.Level1)
{
    CLOGD("StopTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->Stop();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: StopTest_002
 * @tc.desc: Test the Stop function.When playing, can stop it.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, StopTest_002, TestSize.Level1)
{
    CLOGD("StopTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"StopTest_002", "StopTest_002", "http://StopTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    auto result = streamPlayManager->Stop();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: NextTest_001
 * @tc.desc: Test the Next function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, NextTest_001, TestSize.Level1)
{
    CLOGD("NextTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->Next();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: PreviousTest_001
 * @tc.desc: Test the Previous function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, PreviousTest_001, TestSize.Level1)
{
    CLOGD("PreviousTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->Previous();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: SeekTest_001
 * @tc.desc: Test the Seek function.Cannot seek position when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SeekTest_001, TestSize.Level1)
{
    CLOGD("SeekTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int position = 0;
    auto result = streamPlayManager->Seek(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: SeekTest_002
 * @tc.desc: Test the Seek function.When playing, can seek position.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SeekTest_002, TestSize.Level1)
{
    CLOGD("SeekTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"SeekTest_002", "SeekTest_002", "http://SeekTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    int position = 0;
    auto result = streamPlayManager->Seek(position);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: FastForwardTest_001
 * @tc.desc: Test the FastForward function.Cannot fastForward when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, FastForwardTest_001, TestSize.Level1)
{
    CLOGD("FastForwardTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"FastForwardTest_001", "FastForwardTest_001", "http://FastForwardTest_001"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    int delta = 1000;
    auto result = streamPlayManager->FastForward(delta);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: FastForwardTest_002
 * @tc.desc: Test the FastForward function.Cannot fastForward when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, FastForwardTest_002, TestSize.Level1)
{
    CLOGD("FastForwardTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"FastForwardTest_002", "FastForwardTest_002", "http://FastForwardTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    int delta = -1000;
    auto result = streamPlayManager->FastForward(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: FastForwardTest_003
 * @tc.desc: Test the FastForward function.Cannot fastForward when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, FastForwardTest_003, TestSize.Level1)
{
    CLOGD("FastForwardTest_003");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int delta = 1000;
    auto result = streamPlayManager->FastForward(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: FastRewindTest_001
 * @tc.desc: Test the FastRewind function.Cannot fastRewind when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, FastRewindTest_001, TestSize.Level1)
{
    CLOGD("FastRewindTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"FastRewindTest_001", "FastRewindTest_001", "http://FastRewindTest_001"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    int delta = 1000;
    auto result = streamPlayManager->FastRewind(delta);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: FastRewindTest_002
 * @tc.desc: Test the FastRewind function.Cannot FastRewind when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, FastRewindTest_002, TestSize.Level1)
{
    CLOGD("FastRewindTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"FastRewindTest_002", "FastRewindTest_002", "http://FastRewindTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    int delta = -1000;
    auto result = streamPlayManager->FastRewind(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: FastRewindTest_003
 * @tc.desc: Test the FastRewind function.Cannot FastRewind when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, FastRewindTest_003, TestSize.Level1)
{
    CLOGD("FastRewindTest_003");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int delta = 1000;
    auto result = streamPlayManager->FastRewind(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: SetVolumeTest_001
 * @tc.desc: Test the SetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SetVolumeTest_001, TestSize.Level1)
{
    CLOGD("SetVolumeTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int volume = 0; // Normal Value
    auto result = streamPlayManager->SetVolume(volume);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: SetLoopModeTest_001
 * @tc.desc: Test the SetLoopMode function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SetVolumeTest_002, TestSize.Level1)
{
    CLOGD("SetVolumeTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int volume = 200; // Abnormal data
    auto result = streamPlayManager->SetVolume(volume);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: SetLoopModeTest_001
 * @tc.desc: Test the SetLoopMode function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SetLoopModeTest_001, TestSize.Level1)
{
    CLOGD("SetLoopModeTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->SetLoopMode(LoopMode::LOOP_MODE_SEQUENCE);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: SetSpeedTest_001
 * @tc.desc: Test the SetSpeed function.Cannot set speed when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SetSpeedTest_001, TestSize.Level1)
{
    CLOGD("SetSpeedTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->SetSpeed(PlaybackSpeed::SPEED_FORWARD_0_75_X);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: SetSpeedTest_002
 * @tc.desc: Test the SetSpeed function.When playing, can set speed position.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, SetSpeedTest_002, TestSize.Level1)
{
    CLOGD("SetSpeedTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"SetSpeedTest_002", "SetSpeedTest_002", "http://SetSpeedTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    auto result = streamPlayManager->SetSpeed(PlaybackSpeed::SPEED_FORWARD_0_75_X);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetPlayerStatusTest_001
 * @tc.desc: Test the GetPlayerStatus function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetPlayerStatusTest_001, TestSize.Level1)
{
    CLOGD("GetPlayerStatusTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    PlayerStates playerStates;
    auto result = streamPlayManager->GetPlayerStatus(playerStates);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetPositionTest_001
 * @tc.desc: Test the GetPosition function.Cannot get position when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetPositionTest_001, TestSize.Level1)
{
    CLOGD("GetPositionTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int position;
    auto result = streamPlayManager->GetPosition(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetPositionTest_002
 * @tc.desc: Test the GetPosition function.When playing, can get position.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetPositionTest_002, TestSize.Level1)
{
    CLOGD("GetPositionTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"GetPositionTest_002", "GetPositionTest_002", "http://GetPositionTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    int position;
    auto result = streamPlayManager->GetPosition(position);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetDurationTest_001
 * @tc.desc: Test the GetDuration function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetDurationTest_001, TestSize.Level1)
{
    CLOGD("GetDurationTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int duration;
    auto result = streamPlayManager->GetDuration(duration);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetVolumeTest_001
 * @tc.desc: Test the GetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetVolumeTest_001, TestSize.Level1)
{
    CLOGD("GetVolumeTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    int volume;
    int maxVolume;
    auto result = streamPlayManager->GetVolume(volume, maxVolume);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetLoopModeTest_001
 * @tc.desc: Test the GetLoopMode function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetLoopModeTest_001, TestSize.Level1)
{
    CLOGD("GetLoopModeTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    LoopMode sequence = LoopMode::LOOP_MODE_SEQUENCE;
    auto result = streamPlayManager->GetLoopMode(sequence);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetMediaInfoHolderTest_001
 * @tc.desc: Test the GetMediaInfoHolder function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetPlaySpeedTest_001, TestSize.Level1)
{
    CLOGD("GetPlaySpeedTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    PlaybackSpeed playbackSpeed;
    auto result = streamPlayManager->GetPlaySpeed(playbackSpeed);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    streamPlayManager->Release();
}

/**
 * @tc.name: GetMediaInfoHolderTest_001
 * @tc.desc: Test the GetMediaInfoHolder function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, GetMediaInfoHolderTest_001, TestSize.Level1)
{
    CLOGD("GetMediaInfoHolderTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfoHolder mediaInfoHolder;
    auto result = streamPlayManager->GetMediaInfoHolder(mediaInfoHolder);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    streamPlayManager->Release();
}

/**
 * @tc.name: ReleaseTest_001
 * @tc.desc: Test the Release function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, ReleaseTest_001, TestSize.Level1)
{
    CLOGD("ReleaseTest_001");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    auto result = streamPlayManager->Release();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
}

/**
 * @tc.name: ReleaseTest_002
 * @tc.desc: Test the Release function.Setting Callback in Advance.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerManagerTest, ReleaseTest_002, TestSize.Level1)
{
    CLOGD("ReleaseTest_002");
    std::shared_ptr<CastStreamPlayerManager> streamPlayManager = CreateStreamPlayManager();
    ASSERT_NE(streamPlayManager, nullptr);
    MediaInfo mediaInfo{"ReleaseTest_002", "ReleaseTest_002", "http://ReleaseTest_002"};
    auto producer = CreateProducer();
    ASSERT_NE(producer, nullptr);
    streamPlayManager->SetSurface(producer);
    streamPlayManager->Play(mediaInfo);
    streamPlayManager->SetSessionCallbackForRelease(ReleaseCallback);
    auto result = streamPlayManager->Release();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
}
}
}
}