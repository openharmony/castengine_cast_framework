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
 * Create: 2023-6-8
 */

#include <surface.h>
#include <consumer_surface.h>
#include <surface_utils.h>
#include "gtest/gtest.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "cast_engine_errors.h"
#include "cast_session_manager.h"
#include "stream_player.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Stream-Player-Test");

class StreamPlayerTest : public testing::Test {
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

class BufferConsumerListener : public ::OHOS::IBufferConsumerListener {
public:
    void OnBufferAvailable() override {}
};

class TestStreamPlayerListener : public IStreamPlayerListener {
public:
    void OnStateChanged(const PlayerStates playbackState, bool isPlayWhenReady) override {}
    void OnPositionChanged(int position, int bufferPosition, int duration) override {}
    void OnMediaItemChanged(const MediaInfo &mediaInfo) override {}
    void OnVolumeChanged(int volume, int maxVolume) override {}
    void OnLoopModeChanged(const LoopMode loopMode) override {}
    void OnPlaySpeedChanged(const PlaybackSpeed speed) override {}
    void OnPlayerError(int errorCode, const std::string &errorMsg) override {}
    void OnVideoSizeChanged(int width, int height) override {}
    void OnNextRequest() override {}
    void OnPreviousRequest() override {}
    void OnSeekDone(int position) override {}
    void OnEndOfStream(int isLooping) override {}
    void OnPlayRequest(const MediaInfo &mediaInfo) override {}
};

namespace {
std::shared_ptr<ICastSession> g_session;

std::shared_ptr<IStreamPlayer> CreateStream()
{
    std::shared_ptr<IStreamPlayer> stream;
    auto result = g_session->RegisterListener(std::make_shared<TestCastSessionListener>());
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    g_session->CreateStreamPlayer(stream);
    EXPECT_NE(stream, nullptr);
    return stream;
}

std::string CreateSurfaceId()
{
    sptr<IConsumerSurface> consSurface;
    sptr<IBufferProducer> producer;
    sptr<Surface> prodSurface;
    consSurface = IConsumerSurface::Create();
    if (!consSurface) {
        return "";
    }
    sptr<IBufferConsumerListener> surfaceListener = new BufferConsumerListener();
    if (!surfaceListener) {
        return "";
    }
    auto result = consSurface->RegisterConsumerListener(surfaceListener);
    EXPECT_EQ(result, OHOS::GSError::GSERROR_OK);
    producer = consSurface->GetProducer();
    if (!producer) {
        return "";
    }
    prodSurface = Surface::CreateSurfaceAsProducer(producer);
    if (!prodSurface) {
        return "";
    }
    SurfaceUtils::GetInstance()->Add(prodSurface->GetUniqueId(), prodSurface);
    auto uniqueId = prodSurface->GetUniqueId();
    std::string surfaceId = std::to_string(uniqueId);
    return surfaceId;
}
}

void StreamPlayerTest::SetUpTestCase(void)
{
    constexpr int castPermissionNum = 2;
    const char *perms[castPermissionNum] = {
        "ohos.permission.ACCESS_CAST_ENGINE_MIRROR",
        "ohos.permission.ACCESS_CAST_ENGINE_STREAM",
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0, // Indicates the capsbility list of the sa.
        .permsNum = castPermissionNum,
        .aclsNum = 0, // acls is the list of rights that can be escalated.
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "stream_player_test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    CLOGI("tokenId is %" PRIu64, tokenId);
    SetSelfTokenID(tokenId);
    auto ret = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(ret, Security::AccessToken::RET_SUCCESS);
    auto listener = std::make_shared<TestCastSessionManagerListener>();
    auto result = CastSessionManager::GetInstance().RegisterListener(listener);
    ASSERT_EQ(result, CAST_ENGINE_SUCCESS);
    CastSessionManager::GetInstance().CreateCastSession(CastSessionProperty{}, g_session);
    ASSERT_NE(g_session, nullptr);
}

void StreamPlayerTest::TearDownTestCase(void)
{
    CastSessionManager::GetInstance().UnregisterListener();
    CastSessionManager::GetInstance().Release();
}

void StreamPlayerTest::SetUp(void) {}
void StreamPlayerTest::TearDown(void) {}

/**
 * @tc.name: RegisterListenerTest_001
 * @tc.desc: Test the RegisterListener function. The empty listener is input, false is returned.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, RegisterListenerTest_001, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    auto result = stream->RegisterListener(nullptr);
    EXPECT_EQ(result, ERR_INVALID_PARAM);
    stream->Release();
}

/**
 * @tc.name: RegisterListenerTest_002
 * @tc.desc: Test the RegisterListener function.Transfer non-null listener deregistration.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, RegisterListenerTest_002, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    auto result = stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: RegisterListenerTest_003
 * @tc.desc: Test the RegisterListener function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, RegisterListenerTest_003, TestSize.Level1)
{
    CLOGD("RegisterListenerTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: UnregisterListenerTest_001
 * @tc.desc: Test the UnregisterListener function.Unregistered interception.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, UnregisterListenerTest_001, TestSize.Level1)
{
    CLOGD("UnregisterListenerTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    auto result = stream->UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: UnregisterListenerTest_002
 * @tc.desc: Test the UnregisterListener function.Listening initialized.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, UnregisterListenerTest_002, TestSize.Level1)
{
    CLOGD("UnregisterListenerTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: UnregisterListenerTest_003
 * @tc.desc: Test the UnregisterListener function.Proxy is null
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, UnregisterListenerTest_003, TestSize.Level1)
{
    CLOGD("UnregisterListenerTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->UnregisterListener();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SetSurfaceTest_001
 * @tc.desc: Test the SetSurface function.The input surface ID is empty.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetSurfaceTest_001, TestSize.Level1)
{
    CLOGD("SetSurfaceTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->SetSurface("");
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SetSurfaceTest_002
 * @tc.desc: Test the SetSurface function.Registering a Listener..Construction surface ID.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetSurfaceTest_002, TestSize.Level1)
{
    CLOGD("SetSurfaceTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->SetSurface(CreateSurfaceId());
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: SetSurfaceTest_003
 * @tc.desc: Test the SetSurface function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetSurfaceTest_003, TestSize.Level1)
{
    CLOGD("SetSurfaceTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->SetSurface(CreateSurfaceId());
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: LoadTest_001
 * @tc.desc: Test the Load function.The input mediaUrl is empty.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, LoadTest_001, TestSize.Level1)
{
    CLOGD("LoadTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    MediaInfo mediaInfo{"LoadTest_001", "LoadTest_001"};
    auto result = stream->Load(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: LoadTest_002
 * @tc.desc: Test the Load function.The input mediaUrl is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, LoadTest_002, TestSize.Level1)
{
    CLOGD("LoadTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    MediaInfo mediaInfo{"LoadTest_002", "LoadTest_002", "LoadTest_002"};
    auto result = stream->Load(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: LoadTest_003
 * @tc.desc: Test the Load function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, LoadTest_003, TestSize.Level1)
{
    CLOGD("LoadTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    MediaInfo mediaInfo{"LoadTest_003", "LoadTest_003", "LoadTest_003"};
    auto result = stream->Load(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PlayTest_001
 * @tc.desc: Test the Play function.The input mediaUrl is empty.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_001, TestSize.Level1)
{
    CLOGD("PlayTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    MediaInfo mediaInfo{"PlayTest_001", "PlayTest_001"};
    auto result = stream->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PlayTest_002
 * @tc.desc: Test the Play function.The input mediaUrl is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_002, TestSize.Level1)
{
    CLOGD("PlayTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    MediaInfo mediaInfo{"PlayTest_002", "PlayTest_002", "http://PlayTest_002"};
    auto result = stream->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: PlayTest_003
 * @tc.desc: Test the Play function.The input non-boundary value.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_003, TestSize.Level1)
{
    CLOGD("PlayTest_003");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    int index = 0; // non-boundary value
    auto result = stream->Play(index);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: PlayTest_004
 * @tc.desc: Test the Play function.Not in playable state.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_004, TestSize.Level1)
{
    CLOGD("PlayTest_004");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->Play();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PlayTest_005
 * @tc.desc: Test the Play function.Prerequisite: Playable.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_005, TestSize.Level1)
{
    CLOGD("PlayTest_005");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"PlayTest_005", "PlayTest_005", "http://PlayTest_005"};
    stream->Play(mediaInfo);
    stream->Pause();
    auto result = stream->Play();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: PlayTest_007
 * @tc.desc: Test the Play function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_007, TestSize.Level1)
{
    CLOGD("PlayTest_007");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    MediaInfo mediaInfo{"PlayTest_007", "PlayTest_007", "http://PlayTest_007"};
    auto result = stream->Play(mediaInfo);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PlayTest_008
 * @tc.desc: Test the Play function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_008, TestSize.Level1)
{
    CLOGD("PlayTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    int index = 0; // non-boundary value
    auto result = stream->Play(index);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PlayTest_009
 * @tc.desc: Test the Play function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PlayTest_009, TestSize.Level1)
{
    CLOGD("PlayTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->Play();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PauseTest_001
 * @tc.desc: Test the Pause function.Cannot pause when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PauseTest_001, TestSize.Level1)
{
    CLOGD("PauseTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->Pause();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PauseTest_002
 * @tc.desc: Test the Pause function.When playing, can pause it.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PauseTest_002, TestSize.Level1)
{
    CLOGD("PauseTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"PauseTest_002", "PauseTest_002", "http://PauseTest_002"};
    stream->Play(mediaInfo);
    auto result = stream->Pause();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: PauseTest_003
 * @tc.desc: Test the Pause function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PauseTest_003, TestSize.Level1)
{
    CLOGD("PauseTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->Pause();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: StopTest_001
 * @tc.desc: Test the Stop function.Cannot stop when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, StopTest_001, TestSize.Level1)
{
    CLOGD("StopTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->Stop();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: StopTest_002
 * @tc.desc: Test the Stop function.When playing, can stop it.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, StopTest_002, TestSize.Level1)
{
    CLOGD("StopTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"StopTest_002", "StopTest_002", "http://StopTest_002"};
    stream->Play(mediaInfo);
    auto result = stream->Stop();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: StopTest_003
 * @tc.desc: Test the Stop function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, StopTest_003, TestSize.Level1)
{
    CLOGD("StopTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->Stop();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: NextTest_001
 * @tc.desc: Test the Next function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, NextTest_001, TestSize.Level1)
{
    CLOGD("NextTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->Next();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: NextTest_002
 * @tc.desc: Test the Stop function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, NextTest_002, TestSize.Level1)
{
    CLOGD("NextTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->Next();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: PreviousTest_001
 * @tc.desc: Test the Previous function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PreviousTest_001, TestSize.Level1)
{
    CLOGD("PreviousTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    auto result = stream->Previous();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: PreviousTest_002
 * @tc.desc: Test the Previous function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, PreviousTest_002, TestSize.Level1)
{
    CLOGD("PreviousTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->Previous();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SeekTest_001
 * @tc.desc: Test the Seek function.Cannot seek position when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SeekTest_001, TestSize.Level1)
{
    CLOGD("SeekTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    int position = 0; // start position
    auto result = stream->Seek(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SeekTest_002
 * @tc.desc: Test the Seek function.When playing, can seek position.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SeekTest_002, TestSize.Level1)
{
    CLOGD("SeekTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"SeekTest_002", "SeekTest_002", "http://SeekTest_002"};
    stream->Play(mediaInfo);
    int position = 0; // start position
    auto result = stream->Seek(position);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: SeekTest_003
 * @tc.desc: Test the Seek function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SeekTest_003, TestSize.Level1)
{
    CLOGD("SeekTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    int position = 0; // start position
    auto result = stream->Seek(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: FastForwardTest_001
 * @tc.desc: Test the FastForward function.Cannot FastForward when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, FastForwardTest_001, TestSize.Level1)
{
    CLOGD("FastForwardTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"FastForwardTest_001", "FastForwardTest_001", "http://FastForwardTest_001"};
    stream->Play(mediaInfo);
    int delta = 1000;
    auto result = stream->FastForward(delta);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: FastForwardTest_002
 * @tc.desc: Test the FastForward function.Cannot FastForward when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, FastForwardTest_002, TestSize.Level1)
{
    CLOGD("FastForwardTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    int delta = 1000;
    auto result = stream->FastForward(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: FastForwardTest_003
 * @tc.desc: Test the FastForward function.Cannot FastForward when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, FastForwardTest_003, TestSize.Level1)
{
    CLOGD("FastForwardTest_003");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"FastForwardTest_003", "FastForwardTest_003", "http://FastForwardTest_003"};
    stream->Play(mediaInfo);
    int delta = -1000;
    auto result = stream->FastForward(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: FastRewindTest_001
 * @tc.desc: Test the FastRewind function.Cannot FastRewind when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, FastRewindTest_001, TestSize.Level1)
{
    CLOGD("FastRewindTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"FastRewindTest_001", "FastRewindTest_001", "http://FastRewindTest_001"};
    stream->Play(mediaInfo);
    int delta = 1000;
    auto result = stream->FastRewind(delta);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: FastRewindTest_002
 * @tc.desc: Test the FastRewind function.Cannot FastRewind when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, FastRewindTest_002, TestSize.Level1)
{
    CLOGD("FastRewindTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    int delta = 1000;
    auto result = stream->FastRewind(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: FastRewindTest_003
 * @tc.desc: Test the FastRewind function.Cannot FastRewind when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, FastRewindTest_003, TestSize.Level1)
{
    CLOGD("FastRewindTest_003");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"FastRewindTest_003", "FastRewindTest_003", "http://FastRewindTest_003"};
    stream->Play(mediaInfo);
    int delta = -1000;
    auto result = stream->FastRewind(delta);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SetVolumeTest_001
 * @tc.desc: Test the SetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetVolumeTest_001, TestSize.Level1)
{
    CLOGD("SetVolumeTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->RegisterListener(std::make_shared<TestStreamPlayerListener>());
    int32_t volume = 0;
    auto result = stream->SetVolume(volume);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: SetVolumeTest_002
 * @tc.desc: Test the SetVolume function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetVolumeTest_002, TestSize.Level1)
{
    CLOGD("SetVolumeTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    int32_t volume = 0;
    auto result = stream->SetVolume(volume);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SetLoopModeTest_001
 * @tc.desc: Test the SetLoopMode function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetLoopModeTest_001, TestSize.Level1)
{
    CLOGD("SetLoopModeTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    auto result = stream->SetLoopMode(LoopMode::LOOP_MODE_SEQUENCE);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: SetLoopModeTest_002
 * @tc.desc: Test the SetLoopMode function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetLoopModeTest_002, TestSize.Level1)
{
    CLOGD("SetLoopModeTest_001");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->SetLoopMode(LoopMode::LOOP_MODE_SEQUENCE);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SetSpeedTest_001
 * @tc.desc: Test the SetSpeed function.Cannot set speed when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetSpeedTest_001, TestSize.Level1)
{
    CLOGD("SetSpeedTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    auto result = stream->SetSpeed(PlaybackSpeed::SPEED_FORWARD_1_00_X);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: SetSpeedTest_002
 * @tc.desc: Test the SetSpeed function.When playing, can set speed position.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetSpeedTest_002, TestSize.Level1)
{
    CLOGD("SetSpeedTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"SetSpeedTest_002", "SetSpeedTest_002", "http://SetSpeedTest_002"};
    stream->Play(mediaInfo);
    auto result = stream->SetSpeed(PlaybackSpeed::SPEED_FORWARD_1_00_X);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: SetSpeedTest_003
 * @tc.desc: Test the SetSpeed function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, SetSpeedTest_003, TestSize.Level1)
{
    CLOGD("SetSpeedTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->SetSpeed(PlaybackSpeed::SPEED_FORWARD_1_00_X);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetPlayerStatusTest_001
 * @tc.desc: Test the GetPlayerStatus function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetPlayerStatusTest_001, TestSize.Level1)
{
    CLOGD("GetPlayerStatusTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    PlayerStates playerStates;
    auto result = stream->GetPlayerStatus(playerStates);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: GetPlayerStatusTest_002
 * @tc.desc: Test the GetPlayerStatus function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetPlayerStatusTest_002, TestSize.Level1)
{
    CLOGD("GetPlayerStatusTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    PlayerStates playerStates;
    auto result = stream->GetPlayerStatus(playerStates);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetPositionTest_001
 * @tc.desc: Test the GetPosition function.Cannot get position when not playing.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetPositionTest_001, TestSize.Level1)
{
    CLOGD("GetPositionTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    int position;
    auto result = stream->GetPosition(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetPositionTest_002
 * @tc.desc: Test the GetPosition function.When playing, can get position.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetPositionTest_002, TestSize.Level1)
{
    CLOGD("GetPositionTest_002");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    stream->SetSurface(CreateSurfaceId());
    MediaInfo mediaInfo{"GetPositionTest_002", "GetPositionTest_002", "http://GetPositionTest_002"};
    stream->Play(mediaInfo);
    int position;
    auto result = stream->GetPosition(position);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: GetPositionTest_003
 * @tc.desc: Test the GetPosition function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetPositionTest_003, TestSize.Level1)
{
    CLOGD("GetPositionTest_003");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    int position;
    auto result = stream->GetPosition(position);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetDurationTest_001
 * @tc.desc: Test the GetDuration function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetDurationTest_001, TestSize.Level1)
{
    CLOGD("GetDurationTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    int duration;
    auto result = stream->GetDuration(duration);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: GetDurationTest_002
 * @tc.desc: Test the GetDuration function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetDurationTest_002, TestSize.Level1)
{
    CLOGD("GetDurationTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    int duration;
    auto result = stream->GetDuration(duration);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetVolumeTest_001
 * @tc.desc: Test the GetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetVolumeTest_001, TestSize.Level1)
{
    CLOGD("GetVolumeTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    int volume;
    int maxVolume;
    auto result = stream->GetVolume(volume, maxVolume);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: GetVolumeTest_002
 * @tc.desc: Test the GetVolume function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetVolumeTest_002, TestSize.Level1)
{
    CLOGD("GetVolumeTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    int volume;
    int maxVolume;
    auto result = stream->GetVolume(volume, maxVolume);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetLoopModeTest_001
 * @tc.desc: Test the GetLoopMode function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetLoopModeTest_001, TestSize.Level1)
{
    CLOGD("GetLoopModeTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    LoopMode sequence = LoopMode::LOOP_MODE_SEQUENCE;
    auto result = stream->GetLoopMode(sequence);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: GetLoopModeTest_002
 * @tc.desc: Test the GetLoopMode function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetLoopModeTest_002, TestSize.Level1)
{
    CLOGD("GetLoopModeTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    LoopMode sequence = LoopMode::LOOP_MODE_SEQUENCE;
    auto result = stream->GetLoopMode(sequence);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetPlaySpeedTest_001
 * @tc.desc: Test the GetPlaySpeed function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetPlaySpeedTest_001, TestSize.Level1)
{
    CLOGD("GetPlaySpeedTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    PlaybackSpeed playBackSpeed = PlaybackSpeed::SPEED_FORWARD_0_75_X;
    auto result = stream->GetPlaySpeed(playBackSpeed);
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: GetPlaySpeedTest_002
 * @tc.desc: Test the GetPlaySpeed function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetPlaySpeedTest_002, TestSize.Level1)
{
    CLOGD("GetPlaySpeedTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    PlaybackSpeed playBackSpeed = PlaybackSpeed::SPEED_FORWARD_0_75_X;
    auto result = stream->GetPlaySpeed(playBackSpeed);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetMediaInfoHolderTest_001
 * @tc.desc: Test the GetMediaInfoHolder function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetMediaInfoHolderTest_001, TestSize.Level1)
{
    CLOGD("GetMediaInfoHolderTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    MediaInfoHolder mediaInfoHolder;
    auto result = stream->GetMediaInfoHolder(mediaInfoHolder);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: GetMediaInfoHolderTest_002
 * @tc.desc: Test the GetMediaInfoHolder function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, GetMediaInfoHolderTest_002, TestSize.Level1)
{
    CLOGD("GetMediaInfoHolderTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    PlaybackSpeed playBackSpeed = PlaybackSpeed::SPEED_FORWARD_0_75_X;
    auto result = stream->GetPlaySpeed(playBackSpeed);
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}

/**
 * @tc.name: ReleaseTest_001
 * @tc.desc: Test the Release function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, ReleaseTest_001, TestSize.Level1)
{
    CLOGD("ReleaseTest_001");
    std::shared_ptr<IStreamPlayer> stream = CreateStream();
    ASSERT_NE(stream, nullptr);
    auto result = stream->Release();
    EXPECT_EQ(result, CAST_ENGINE_SUCCESS);
    stream->Release();
}

/**
 * @tc.name: ReleaseTest_002
 * @tc.desc: Test the Release function.Proxy is null.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerTest, ReleaseTest_002, TestSize.Level1)
{
    CLOGD("ReleaseTest_002");
    std::shared_ptr<IStreamPlayer> stream = std::make_shared<StreamPlayer>(nullptr);
    ASSERT_NE(stream, nullptr);
    auto result = stream->Release();
    EXPECT_EQ(result, CAST_ENGINE_ERROR);
    stream->Release();
}
}
}
}