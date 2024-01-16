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
 * Description: Remote Control related data from OH XComponent structure definitions.
 * Author: mayihao
 * Create: 2022-09-14
 */


#ifndef OH_REMOTE_CONTROL_EVENT_H
#define OH_REMOTE_CONTROL_EVENT_H

#include <stdint.h>
#include "cast_engine_common.h"

namespace OHOS {
namespace CastEngine {
enum class EXPORT XcomponentEventType {
    REMOTECONTROL_TOUCH,
    REMOTECONTROL_MOUSE,
    REMOTECONTROL_PEN,
    REMOTECONTROL_WHEEL,
    REMOTECONTROL_KEY,
    REMOTECONTROL_INPUT_METHOD,
    REMOTECONTROL_VIRTUAL_KEY,
    REMOTECONTROL_INVALID_EVENT,
};

enum EXPORT OHNativeXcomponentResult {
    OH_NATIVEXCOMPONENT_RESULT_SUCCESS = 0,
    OH_NATIVEXCOMPONENT_RESULT_FAILED = -1,
    OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER = -2,
};

enum EXPORT OHNativeXcomponentTouchEventType {
    OH_NATIVEXCOMPONENT_TOUCH_DOWN = 0,
    OH_NATIVEXCOMPONENT_TOUCH_UP,
    OH_NATIVEXCOMPONENT_TOUCH_MOVE,
    OH_NATIVEXCOMPONENT_TOUCH_CANCEL,
};

enum EXPORT OHNativeXcomponentMouseEventAction {
    OH_NATIVEXCOMPONENT_MOUSE_PRESS = 0,
    OH_NATIVEXCOMPONENT_MOUSE_RELEASE,
    OH_NATIVEXCOMPONENT_MOUSE_MOVE,
    OH_NATIVEXCOMPONENT_MOUSE_NONE,
};

enum EXPORT OHNativeXcomponentKeyEventType {
    OH_NATIVEXCOMPONENT_KEY_DOWN = 0,
    OH_NATIVEXCOMPONENT_KEY_UP,
};

enum EXPORT OHNativeXcomponentMouseEventButton {
    OH_NATIVEXCOMPONENT_LEFT_BUTTON = 0,
    OH_NATIVEXCOMPONENT_MIDDLE_BUTTON,
    OH_NATIVEXCOMPONENT_RIGHT_BUTTON,
};

enum EXPORT OHNativeXcomponentInputMethodEventType {
    OH_NATIVEXCOMPONENT_INPUT_CONTENT = 0,
    OH_NATIVEXCOMPONENT_INPUT_FOCUS,
};

enum EXPORT OHNativeXcomponentWheelEventDirection {
    OH_NATIVEXCOMPONENT_WHEEL_VERTICAL = 0,
    OH_NATIVEXCOMPONENT_WHEEL_HORIZONTAL
};

enum EXPORT OHNativeXcomponentVirtualKeyEventType {
    OH_NATIVEXCOMPONENT_VIRTUALKEY_BACK = 0,
    OH_NATIVEXCOMPONENT_VIRTUALKEY_HOME,
    OH_NATIVEXCOMPONENT_VIRTUALKEY_RECENT_APP,
    OH_NATIVEXCOMPONENT_VIRTUALKEY_QUICK_SETTING
};

const uint32_t EXPORT OH_MAX_TOUCH_POINTS_NUMBER = 10;
const uint32_t EXPORT OH_MAX_CONTENT_LEN = 472;

const uint32_t EXPORT EXPORT META_CAP_LOCKED = 0x100;
const uint32_t EXPORT META_ALT_LOCKED = 0x200;
const uint32_t EXPORT META_SYM_LOCKED = 0x400;
const uint32_t EXPORT META_SELECTING = 0x800;
const uint32_t EXPORT META_ALT_ON = 0x02;
const uint32_t EXPORT META_ALT_LEFT_ON = 0x10;
const uint32_t EXPORT META_ALT_RIGHT_ON = 0x20;
const uint32_t EXPORT META_SHIFT_ON = 0x1;
const uint32_t EXPORT META_SHIFT_LEFT_ON = 0x40;
const uint32_t EXPORT META_SHIFT_RIGHT_ON = 0x80;
const uint32_t EXPORT META_SYM_ON = 0x4;
const uint32_t EXPORT META_FUNCTION_ON = 0x8;
const uint32_t EXPORT META_CTRL_ON = 0x1000;
const uint32_t EXPORT META_CTRL_LEFT_ON = 0x2000;
const uint32_t EXPORT META_CTRL_RIGHT_ON = 0x4000;
const uint32_t EXPORT META_META_ON = 0x10000;
const uint32_t EXPORT META_META_LEFT_ON = 0x20000;
const uint32_t EXPORT META_META_RIGHT_ON = 0x40000;
const uint32_t EXPORT META_CAPS_LOCK_ON = 0x100000;
const uint32_t EXPORT META_NUM_LOCK_ON = 0x200000;
const uint32_t EXPORT META_SCROLL_LOCK_ON = 0x400000;

struct EXPORT OHNativeXcomponentTouchPoint {
    int32_t id = 0; // Unique identifier of a finger.
    float screenX = 0.0; // X coordinate of the touch point relative to the left edge of the screen.
    float screenY = 0.0; // Y coordinate of the touch point relative to the upper edge of the screen.
    float x = 0.0; // X coordinate of the touch point relative to the left edge of the element to touch.
    float y = 0.0; // Y coordinate of the touch point relative to the upper edge of the element to touch.
    OHNativeXcomponentTouchEventType type;
    double size = 0.0; // Contact area between the finger pad and the screen.
    float force = 0.0; // Pressure of the current touch event.
    int64_t timeStamp = 0; // Timestamp of the current touch event.
    bool isPressed = false; // Whether the current point is pressed.
};

struct EXPORT OHNativeXcomponentTouchEvent {
    int32_t id = 0; // Unique identifier of a finger.
    float screenX = 0.0; // X coordinate of the touch point relative to the left edge of the screen.
    float screenY = 0.0; // Y coordinate of the touch point relative to the upper edge of the screen.
    float x = 0.0; // X coordinate of the touch point relative to the left edge of the element to touch.
    float y = 0.0; // Y coordinate of the touch point relative to the upper edge of the element to touch.
    OHNativeXcomponentTouchEventType type;
    double size = 0.0; // Contact area between the finger pad and the screen.
    float force = 0.0; // Pressure of the current touch event.
    int64_t deviceId = 0; // ID of the device where the current touch event is generated.
    int64_t timeStamp = 0; // Timestamp of the current touch event.
    uint32_t numPoints = 0; // Number of current touch points.
    OHNativeXcomponentTouchPoint touchPoints[OH_MAX_TOUCH_POINTS_NUMBER]; // Array of the current touch points.
};

struct EXPORT OHNativeXcomponentMouseEvent {
    float x = 0.0; // X coordinate of the mouse point relative to the left edge of the element to mouse.
    float y = 0.0; // Y coordinate of the mouse point relative to the upper edge of the element to mouse.
    float screenX = 0.0; // X coordinate of the mouse point relative to the left edge of the screen.
    float screenY = 0.0; // Y coordinate of the mouse point relative to the upper edge of the screen.
    int64_t timestamp = 0; // Timestamp of the current mouse event.
    OHNativeXcomponentMouseEventAction action;
    OHNativeXcomponentMouseEventButton button;
};

struct EXPORT OHNativeXcomponentKeyEvent {
    uint8_t reserved;
    uint16_t keyCode1; // the first key code
    uint16_t keyCode2; // the second key code
    uint32_t metaState; // whether meta keys is pressed
    OHNativeXcomponentKeyEventType type;
};

struct EXPORT OHNativeXcomponentContentEvent {
    uint16_t msgLen;
    char inputText[OH_MAX_CONTENT_LEN];
};

struct EXPORT OHNativeXcomponentFocusEvent {
    uint8_t focusStat; // is focus on
    double cursorX1; // The X position of the upper-left corner of the cursor
    double cursorY1; // The Y position of the upper-left corner of the cursor
    double cursorX2; // The X position of the lowwer-right corner of the cursor
    double cursorY2; // The Y position of the lowwer-right corner of the cursor
};

struct EXPORT OHNativeXcomponentInputMethodEvent {
    OHNativeXcomponentInputMethodEventType type;
    union {
        OHNativeXcomponentContentEvent contentEvent;
        OHNativeXcomponentFocusEvent focusEvent;
    };
};

struct EXPORT OHNativeXcomponentWheelEvent {
    OHNativeXcomponentWheelEventDirection direction;
    uint8_t indication = 1; // 0b0:Scrolling to the right/Scrolling down, 0b1:Scrolling to the left/Scrolling up
    uint8_t scrollUnit = 2; // 0b00:the unit is a pixel, 0b01:the unit is a mouse notch, 0b10-0b11:Reserved
    uint16_t wheelDis; // wheel distance
    float x; // X-coordinate when wheel button down
    float y; // Y-coordinate when wheel button down
};

struct EXPORT OHNativeXcomponentVirtualKeyEvent {
    OHNativeXcomponentVirtualKeyEventType type;
    float x;
    float y;
};

struct EXPORT OHRemoteControlEvent {
    OHRemoteControlEvent() {};
    XcomponentEventType eventType = XcomponentEventType::REMOTECONTROL_INVALID_EVENT;
    union {
        OHNativeXcomponentTouchEvent touchEvent;
        OHNativeXcomponentMouseEvent mouseEvent;
        OHNativeXcomponentKeyEvent keyEvent;
        OHNativeXcomponentInputMethodEvent inputMethodEvent;
        OHNativeXcomponentWheelEvent wheelEvent;
        OHNativeXcomponentVirtualKeyEvent virtualKeyEvent;
    };
};
} // namespace CastEngine
} // namespace OHOS
#endif
