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
 * Create: 2023-7-3
 */

#include "gtest/gtest.h"
#include "cast_engine_common.h"
#include "cast_engine_common_helper.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"
#include "stream_player_listener_impl_stub.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "ipc_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace CastEngine {
namespace CastEngineClient {
DEFINE_CAST_ENGINE_LABEL("Stream-Player-Listener-Impl_Stub-Test");

class StreamPlayerListenerImplStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
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
std::shared_ptr<StreamPlayerListenerImplStub> CreatelistenerStub()
{
    auto userListener_ = std::make_shared<TestStreamPlayerListener>();
    if (!userListener_) {
        return nullptr;
    }
    return std::make_shared<StreamPlayerListenerImplStub>(userListener_);
}

enum {
    ON_PLAYER_STATUS_CHANGED = 1,
    ON_POSITION_CHANGED,
    ON_MEDIA_ITEM_CHANGED,
    ON_VOLUME_CHANGED,
    ON_REPEAT_MODE_CHANGED,
    ON_PLAY_SPEED_CHANGED,
    ON_PLAYER_ERROR,
    ON_VIDEO_SIZE_CHANGED,
    ON_NEXT_REQUEST,
    ON_PREVIOUS_REQUEST,
    ON_SEEK_DONE,
    ON_END_OF_STREAM,
    ON_PLAY_REQUEST,
    INVALID_CODE
};
}

void StreamPlayerListenerImplStubTest::SetUpTestCase(void) {}
void StreamPlayerListenerImplStubTest::TearDownTestCase(void) {}
void StreamPlayerListenerImplStubTest::SetUp(void) {}
void StreamPlayerListenerImplStubTest::TearDown(void) {}

/**
 * @tc.name: OnRemoteRequestTest_001
 * @tc.desc: Test the OnRemoteRequest function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, OnRemoteRequestTest_001, TestSize.Level1)
{
    CLOGD("OnRemoteRequestTest_001");
    auto listenerImplStub = std::make_shared<StreamPlayerListenerImplStub>(nullptr);
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_PLAYER_STATUS_CHANGED, data, reply, option);
    EXPECT_EQ(result, ERR_NULL_OBJECT);
}

/**
 * @tc.name: OnRemoteRequestTest_002
 * @tc.desc: Test the OnRemoteRequest function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, OnRemoteRequestTest_002, TestSize.Level1)
{
    CLOGD("OnRemoteRequestTest_002");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_PLAYER_STATUS_CHANGED, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequestTest_003
 * @tc.desc: Test the OnRemoteRequest function.error code.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, OnRemoteRequestTest_003, TestSize.Level1)
{
    CLOGD("OnRemoteRequestTest_003");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(INVALID_CODE, data, reply, option);
    EXPECT_EQ(result, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: DoOnLoopModeChangedTaskTest_001
 * @tc.desc: Test the DoOnLoopModeChangedTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnLoopModeChangedTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnLoopModeChangedTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_REPEAT_MODE_CHANGED, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnPlaySpeedChangedTaskTest_001
 * @tc.desc: Test the DoOnPlaySpeedChangedTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnPlaySpeedChangedTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnPlaySpeedChangedTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_PLAY_SPEED_CHANGED, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnPlayerErrorTaskTest_001
 * @tc.desc: Test the DoOnPlayerErrorTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnPlayerErrorTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnPlayerErrorTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_PLAYER_ERROR, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnVideoSizeChangedTaskTest_001
 * @tc.desc: Test the DoOnVideoSizeChangedTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnVideoSizeChangedTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnVideoSizeChangedTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_VIDEO_SIZE_CHANGED, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnNextRequestTaskTest_001
 * @tc.desc: Test the DoOnNextRequestTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnNextRequestTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnNextRequestTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_NEXT_REQUEST, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnPreviousRequestTaskTest_001
 * @tc.desc: Test the DoOnPreviousRequestTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnPreviousRequestTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnPreviousRequestTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_PREVIOUS_REQUEST, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnSeekDoneTaskTest_001
 * @tc.desc: Test the DoOnSeekDoneTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnSeekDoneTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnSeekDoneTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_SEEK_DONE, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnEndOfStreamTaskTest_001
 * @tc.desc: Test the DoOnEndOfStreamTask function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnEndOfStreamTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnEndOfStreamTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_END_OF_STREAM, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnPlayRequestTaskTest_001
 * @tc.desc: Test the DoOnPlayRequestTaskTest function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnPlayRequestTaskTest_001, TestSize.Level1)
{
    CLOGD("DoOnPlayRequestTaskTest_001");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    auto result = listenerImplStub->OnRemoteRequest(ON_PLAY_REQUEST, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: DoOnPlayRequestTaskTest_002
 * @tc.desc: Test the DoOnPlayRequestTaskTest function.
 * @tc.type: FUNC
 */
HWTEST_F(StreamPlayerListenerImplStubTest, DoOnPlayRequestTaskTest_002, TestSize.Level1)
{
    CLOGD("DoOnPlayRequestTaskTest_002");
    auto listenerImplStub = CreatelistenerStub();
    ASSERT_NE(listenerImplStub, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(StreamPlayerListenerImplStub::GetDescriptor());
    MediaInfo mediaInfo = MediaInfo{};
    WriteMediaInfo(data, mediaInfo);
    auto result = listenerImplStub->OnRemoteRequest(ON_PLAY_REQUEST, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}
}
}
}