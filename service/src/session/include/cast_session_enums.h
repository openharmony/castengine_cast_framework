/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * Description: Enumarations ralated to Cast session.
 * Author: yuezihao
 * Create: 2023-11-06
 */
 
#ifndef CAST_SESSION_ENUMS_H
#define CAST_SESSION_ENUMS_H
 
namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionEnums {

enum MessageId : int {
    MSG_CONNECT,
    MSG_SETUP,
    MSG_SETUP_SUCCESS,
    MSG_SETUP_FAILED,
    MSG_SETUP_DONE,
    MSG_PLAY,
    MSG_PAUSE,
    MSG_PLAY_REQ,
    MSG_PAUSE_REQ,
    MSG_DISCONNECT,
    MSG_CONNECT_TIMEOUT,
    MSG_PROCESS_TRIGGER_REQ,
    MSG_UPDATE_VIDEO_SIZE,
    MSG_STREAM_RECV_ACTION_EVENT_FROM_PEERS,
    MSG_STREAM_SEND_ACTION_EVENT_TO_PEERS,
    MSG_PEER_RENDER_READY,
    MSG_ERROR,
    MSG_SET_CAST_MODE,
    MSG_READY_TO_PLAYING,
    MSG_AUTH,
    MSG_MIRROR_SEND_ACTION_EVENT_TO_PEERS,
    MSG_ID_MAX,
};
} // namespace CastSessionEnums
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif