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
 * Description: common method for cast session
 * Author: lijianzhao
 * Create: 2022-01-19
 */

#ifndef UTILS_H
#define UTILS_H

#include <locale>
#include <string>
#include <vector>
#include <atomic>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
#if !defined(DISALLOW_COPY_AND_ASSIGN)
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &);            \
    void operator = (const TypeName &)
#endif

#if !defined(DISALLOW_EVIL_CONSTRUCTORS)
#define DISALLOW_EVIL_CONSTRUCTORS(TypeName) DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

#if !defined(DISALLOW_IMPLICIT_CONSTRUCTORS)
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName();                                  \
    DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

#define USEC_1000 1000

struct PacketData {
    uint8_t *data;
    int length;
};

struct ConstPacketData {
    const uint8_t *data;
    int length;
};

enum class DrmType {
    DRM_BASE = 1,
    CLEARKEY = DRM_BASE << 1,
    WIDEVINE = DRM_BASE << 2,
    PLAYREADY = DRM_BASE << 3,
    CHINADRM = DRM_BASE << 4,
};

class Utils {
public:
    Utils() {}
    ~Utils() {}
    static void SplitString(const std::string &src, std::vector<std::string> &dest, const std::string &seperator);
    static std::string &Trim(std::string &str);
    static std::string &ToLower(std::string &str);
    static bool StartWith(const std::string &mainStr, const std::string &subStr);
    static int IntToByteArray(int num, int length, uint8_t *output);
    static uint32_t ByteArrayToInt(const uint8_t *input, unsigned int length);
    static int32_t StringToInt(const std::string &str, int base = DECIMALISM);
    static std::string GetWifiIp();
    static bool IsArrayAllZero(const uint8_t *input, int length);
    static uint64_t TimeMilliSecond();
    static std::string Mask(const std::string &str);

    static bool Base64Encode(const std::string &source, std::string &encoded);
    static bool Base64Decode(const std::string &encoded, std::string &decoded);
    static DrmType DrmUuidToType(std::string drmUUID);
    static int32_t GetCurrentActiveAccountUserId();
    static std::string GetOhosAccountId();
    static int SetFirstTokenID();
    static void EnablePowerForceTimingOut();
    static void ResetPowerForceTimingOut();
    static void LightAndLockScreen();

    static void SetThreadName(const std::string &name);

private:
    static const int BYTE_TO_BIT_OFFSET = 8;
    static const uint8_t INT_TO_BYTE = 0xff;
    static const std::string BASE64_CHARS;
    static inline bool IsBase64(unsigned char c);
    static const int DECIMALISM = 10;
    static const int HEXADECIMALISM = 16;
    static std::string ConvertIpv4Address(unsigned int addressIpv4);
    constexpr static int MASK_MIN_LEN = 2;
    constexpr static int MASK_PRINT_PREFIX_LEN = 4;
    constexpr static int MASK_PRINT_POSTFIX_LEN = 2;
    static std::atomic<bool> isEnablePowerForceTimingOut;
};

inline const std::string UUID_CHINADRM = "3d5e6d35-9b9a-41e8-b843-dd3c6e72c42c";
inline const std::string UUID_WIDEVINE = "edef8ba9-79d6-4ace-a3c8-27dcd51d21ed";
inline const std::string UUID_PLAYREADY = "9a04f079-9840-4286-ab92-e65be0885f95";
inline constexpr char SANDBOX_PATH[] = "/data/service/el1/public/cast_engine_service";
inline constexpr char PARAM_MEDIA_DUMP[] = "debug.cast.media.dump";
inline constexpr char PARAM_MEDIA_DUMP_FOR_BETA[] = "debug.cast.media.dump.beta";
inline constexpr char PARAM_VIDEO_CACHE[] = "debug.cast.video.cache";
inline constexpr char PARAM_PERFORMANCE[] = "debug.cast.performance";
inline constexpr char UNIQUE_SCREEN[] = "debug.cast.unique.screen";
inline constexpr char NOTIFY_DEVICE_FOUND[] = "debug.cast.device.found";
inline constexpr char USED_AGGR_SEND[] = "debug.cast.aggr.send";
inline constexpr char PARAM_VTP_SUPPORT[] = "debug.cast.vtp.support";
inline constexpr char PARAM_YUV_SUPPORT[] = "debug.cast.yuv.support";
inline constexpr char FLASH_LIGHT[] = "debug.cast.flash.light";
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
