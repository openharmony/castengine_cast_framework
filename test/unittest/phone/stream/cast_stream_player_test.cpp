/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: Test cast_stream_player.
 * Author: jiangfan
 * Create: 2023-7-3
 */

#include <surface.h>
#include <consumer_surface.h>
#include <surface_utils.h>
#include "gtest/gtest.h"
#include "cast_engine_log.h"
#include "cast_engine_common.h"
#include "cast_engine_errors.h"
#include "cast_stream_common.h"
#include "cast_stream_player.h"
#include "cast_stream_player_utils.h"
#include "cast_stream_manager_server.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Stream-Player-Manager-Test");

class CastStreamPlayerTest : public testing::Test {
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
std::shared_ptr<CastStreamPlayerCallback> CreateCallback()
{
    auto listener = std::make_shared<TestCastStreamListener>();
    auto castStreamManager = std::make_shared<CastStreamManagerServer>(listener);
    return std::make_shared<CastStreamPlayerCallback>(castStreamManager);
}

std::shared_ptr<CastStreamPlayer> CreateStreamPlayer()
{
    auto listener = std::make_shared<TestCastStreamListener>();
    auto castStreamManager = std::make_shared<CastStreamManagerServer>(listener);
    auto callback = std::make_shared<CastStreamPlayerCallback>(castStreamManager);
    return std::make_shared<CastStreamPlayer>(callback, nullptr);
}
}

void CastStreamPlayerTest::SetUpTestCase(void)
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
        .processName = "cast_stream_player_test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    CLOGI("tokenId is %lu", tokenId);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void CastStreamPlayerTest::TearDownTestCase(void) {}
void CastStreamPlayerTest::SetUp(void) {}
void CastStreamPlayerTest::TearDown(void) {}

/**
 * @tc.name: GetSpeedModeTest_001
 * @tc.desc: Test the GetSpeedMode function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetSpeedModeTest_001, TestSize.Level1)
{
    CLOGD("GetSpeedModeTest_001");
    auto castCallback = CreateCallback();
    ASSERT_NE(castCallback, nullptr);
    castCallback->SetSpeedMode(Media::SPEED_FORWARD_0_75_X);
    auto result = castCallback->GetSpeedMode();
    EXPECT_EQ(result, Media::SPEED_FORWARD_0_75_X);
}

/**
 * @tc.name: SetSwitchingTest_001
 * @tc.desc: Test the SetSwitching function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetSwitchingTest_001, TestSize.Level1)
{
    CLOGD("SetSwitchingTest_001");
    auto castCallback = CreateCallback();
    ASSERT_NE(castCallback, nullptr);
    int switchingCount = 0;
    castCallback->SetSwitching();
    auto result = castCallback->GetSwitching();
    EXPECT_NE(result, switchingCount);
}

/**
 * @tc.name: PrepareAsyncTest_001
 * @tc.desc: Test the PrepareAsync function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, PrepareAsyncTest_001, TestSize.Level1)
{
    CLOGD("PrepareAsyncTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->PrepareAsync();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: PauseTest_001
 * @tc.desc: Test the Pause function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, PauseTest_001, TestSize.Level1)
{
    CLOGD("PauseTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->Pause();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: StopTest_001
 * @tc.desc: Test the Stop function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, StopTest_001, TestSize.Level1)
{
    CLOGD("StopTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->Stop();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: ResetTest_001
 * @tc.desc: Test the Reset function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, ResetTest_001, TestSize.Level1)
{
    CLOGD("ResetTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->Reset();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SetVolumeTest_001
 * @tc.desc: Test the SetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetVolumeTest_001, TestSize.Level1)
{
    CLOGD("SetVolumeTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    int volume = 0;  // Normal Value
    auto result = streamPlayer->SetVolume(volume);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: SetVolumeTest_002
 * @tc.desc: Test the SetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetVolumeTest_002, TestSize.Level1)
{
    CLOGD("SetVolumeTest_002");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    int volume = 500;  // boundary value
    auto result = streamPlayer->SetVolume(volume);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetVolumeTest_001
 * @tc.desc: Test the GetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetVolumeTest_001, TestSize.Level1)
{
    CLOGD("GetVolumeTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    int volume;
    int maxVolume;
    auto result = streamPlayer->GetVolume(volume, maxVolume);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: GetVolumeTest_002
 * @tc.desc: Test the GetVolume function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetVolumeTest_002, TestSize.Level1)
{
    CLOGD("GetVolumeTest_002");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    int maxVolume = CastStreamPlayerUtils::GetMaxVolume();
    EXPECT_NE(maxVolume, 0);
    int curVolume = CastStreamPlayerUtils::GetVolume();
    EXPECT_NE(curVolume, 0);
}


/**
 * @tc.name: SetLoopingTest_001
 * @tc.desc: Test the SetLooping function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetLoopingTest_001, TestSize.Level1)
{
    CLOGD("SetLoopingTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->SetLooping(true);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: SetLoopingTest_002
 * @tc.desc: Test the SetLooping function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetLoopingTest_002, TestSize.Level1)
{
    CLOGD("SetLoopingTest_002");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->SetLooping(false);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: GetCurrentTimeTest_001
 * @tc.desc: Test the GetCurrentTime function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetCurrentTimeTest_001, TestSize.Level1)
{
    CLOGD("GetCurrentTimeTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    int32_t currentTime;
    auto result = streamPlayer->GetCurrentTime(currentTime);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetVideoTrackInfoTest_001
 * @tc.desc: Test the GetVideoTrackInfo function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetVideoTrackInfoTest_001, TestSize.Level1)
{
    CLOGD("GetVideoTrackInfoTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    std::vector<Media::Format> videoTrack;
    auto result = streamPlayer->GetVideoTrackInfo(videoTrack);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetAudioTrackInfoTest_001
 * @tc.desc: Test the GetAudioTrackInfo function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetAudioTrackInfoTest_001, TestSize.Level1)
{
    CLOGD("GetAudioTrackInfoTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    std::vector<Media::Format> audioTrack;
    auto result = streamPlayer->GetAudioTrackInfo(audioTrack);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetVideoWidthTest_001
 * @tc.desc: Test the GetVideoWidth function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetVideoWidthTest_001, TestSize.Level1)
{
    CLOGD("GetVideoWidthTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->GetVideoWidth();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetVideoHeightTest_001
 * @tc.desc: Test the GetVideoHeight function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetVideoHeightTest_001, TestSize.Level1)
{
    CLOGD("GetVideoHeightTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->GetVideoHeight();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetDurationTest_001
 * @tc.desc: Test the GetDuration function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetDurationTest_001, TestSize.Level1)
{
    CLOGD("GetDurationTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->GetDuration();
    EXPECT_EQ(result, CAST_STREAM_INT_INVALID);
}

/**
 * @tc.name: SetPlaybackSpeedTest_001
 * @tc.desc: Test the SetPlaybackSpeed function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetPlaybackSpeedTest_001, TestSize.Level1)
{
    CLOGD("SetPlaybackSpeedTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->SetPlaybackSpeed(Media::SPEED_FORWARD_0_75_X);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetPlaybackSpeedTest_001
 * @tc.desc: Test the GetPlaybackSpeed function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, GetPlaybackSpeedTest_001, TestSize.Level1)
{
    CLOGD("GetPlaybackSpeedTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    Media::PlaybackRateMode mode;
    auto result = streamPlayer->GetPlaybackSpeed(mode);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: SelectBitRateTest_001
 * @tc.desc: Test the SelectBitRate function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SelectBitRateTest_001, TestSize.Level1)
{
    CLOGD("SelectBitRateTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    uint32_t bitRate = 0;
    auto result = streamPlayer->SelectBitRate(bitRate);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: IsPlayingTest_001
 * @tc.desc: Test the IsPlaying function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, IsPlayingTest_001, TestSize.Level1)
{
    CLOGD("IsPlayingTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->IsPlaying();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: IsLoopingTest_001
 * @tc.desc: Test the IsLooping function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, IsLoopingTest_001, TestSize.Level1)
{
    CLOGD("IsLoopingTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->IsLooping();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SetParameterTest_001
 * @tc.desc: Test the IsLooping function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetParameterTest_001, TestSize.Level1)
{
    CLOGD("SetParameterTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    Media::Format param;
    auto result = streamPlayer->SetParameter(param);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SetVideoSurfaceTest_001
 * @tc.desc: Test the IsLooping function.
 * @tc.type: FUNC
 */
HWTEST_F(CastStreamPlayerTest, SetVideoSurfaceTest_001, TestSize.Level1)
{
    CLOGD("SetVideoSurfaceTest_001");
    auto streamPlayer = CreateStreamPlayer();
    ASSERT_NE(streamPlayer, nullptr);
    auto result = streamPlayer->SetVideoSurface(nullptr);
    EXPECT_EQ(result, false);
}
}
}
}
