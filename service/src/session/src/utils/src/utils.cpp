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

#include "utils.h"

#include <cctype>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <sys/prctl.h>
#include <sys/time.h>

#include "wifi_device.h"
#include "ohos_account_kits.h"
#include "os_account_manager.h"
#include "os_account_constants.h"
#include "cast_engine_log.h"
#include "ipc_skeleton.h"
#include "token_setproc.h"
#include "power_mgr_client.h"

using namespace OHOS::PowerMgr;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Utils");

const uint32_t BASE64_UNIT_ONE_PADDING = 1;
const uint32_t BASE64_UNIT_TWO_PADDING = 2;
const uint32_t BASE64_SRC_UNIT_SIZE = 3;
const uint32_t BASE64_DEST_UNIT_SIZE = 4;
constexpr static int32_t DEFAULT_OS_ACCOUNT_ID = 100;
std::atomic<bool> Utils::isEnablePowerForceTimingOut = false;

bool Utils::Base64Encode(const std::string &source, std::string &encoded)
{
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        CLOGE("Base64Encode error: BIO_new failed");
        return false;
    }
    BIO *b64 = BIO_new(BIO_f_base64());
    if (b64 == nullptr) {
        CLOGE("Base64Encode error: BIO_f_base64 failed");
        BIO_free(bio);
        return false;
    }

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    int result = BIO_write(b64, source.c_str(), source.length());
    if (result <= 0) {
        CLOGE("Base64Encode error: bio write fail.");
        BIO_free_all(b64);
        return false;
    }
    BIO_flush(b64);

    BUF_MEM *bptr = nullptr;
    bool ret = false;
    BIO_get_mem_ptr(b64, &bptr);
    if (bptr != nullptr) {
        encoded = std::string(bptr->data, bptr->length);
        ret = true;
    }
    BIO_free_all(b64);
    return ret;
}

bool Utils::Base64Decode(const std::string &encoded, std::string &decoded)
{
    if (encoded.length() % BASE64_DEST_UNIT_SIZE != 0 || encoded.length() == 0) {
        return false;
    }

    uint32_t decodedLen = encoded.length() * BASE64_SRC_UNIT_SIZE / BASE64_DEST_UNIT_SIZE;
    if (encoded.at(encoded.length() - BASE64_UNIT_ONE_PADDING) == '=') {
        decodedLen--;
        if (encoded.at(encoded.length() - BASE64_UNIT_TWO_PADDING) == '=') {
            decodedLen--;
        }
    }

    bool ret = false;
    char* buffer = static_cast<char *>(malloc(decodedLen));
    if (buffer == nullptr) {
        return ret;
    }

    BIO *bio = BIO_new_mem_buf(encoded.c_str(), encoded.length());
    if (bio == nullptr) {
        free(buffer);
        return ret;
    }

    BIO *b64 = BIO_new(BIO_f_base64());
    if (b64 == nullptr) {
        CLOGE("Base64Encode error: BIO_f_base64 failed");
        free(buffer);
        BIO_free(bio);
        return false;
    }

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    if (BIO_read(b64, buffer, encoded.length()) == static_cast<int32_t>(decodedLen)) {
        decoded = std::string(buffer, decodedLen);
        ret = true;
    }

    free(buffer);
    BIO_free_all(b64);
    return ret;
}

void Utils::SplitString(const std::string &src, std::vector<std::string> &dest, const std::string &seperator)
{
    std::string::size_type beginPos = 0;
    std::string::size_type endPos = src.find(seperator);
    while (endPos != std::string::npos) {
        dest.push_back(src.substr(beginPos, endPos - beginPos));
        beginPos = endPos + seperator.size();
        endPos = src.find(seperator, beginPos);
    }
    if (beginPos != src.length()) {
        dest.push_back(src.substr(beginPos));
    }
}

std::string &Utils::Trim(std::string &str)
{
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

std::string &Utils::ToLower(std::string &str)
{
    std::locale loc;
    for (std::string::size_type i = 0; i < str.length(); ++i) {
        str[i] = std::tolower(str[i], loc);
    }
    return str;
}

bool Utils::StartWith(const std::string &mainStr, const std::string &subStr)
{
    return mainStr.find(subStr) == 0;
}

int Utils::IntToByteArray(int num, int length, uint8_t *output)
{
    for (int i = 0; i < length; i++) {
        output[length - 1 - i] = static_cast<uint8_t>(
            (static_cast<unsigned int>(num) >> static_cast<unsigned int>(BYTE_TO_BIT_OFFSET * i))) &
            INT_TO_BYTE;
    }
    return length;
}

uint32_t Utils::ByteArrayToInt(const uint8_t *input, unsigned int length)
{
    uint32_t output = 0;
    if (length > BYTE_TO_BIT_OFFSET) {
        return output;
    }
    for (unsigned int i = 0; i < length; i++) {
        output |= (input[i] << (BYTE_TO_BIT_OFFSET * (length - 1 - i)));
    }
    return output;
}

int32_t Utils::StringToInt(const std::string &str, int base)
{
    if (str.size() == 0) {
        return 0;
    }
    char *nextPtr = nullptr;
    long result = strtol(str.c_str(), &nextPtr, base);
    if (errno == ERANGE || *nextPtr != '\0') {
        return 0;
    }
    return static_cast<int32_t>(result);
}

std::string Utils::ConvertIpv4Address(unsigned int addressIpv4)
{
    std::string address;
    if (addressIpv4 == 0) {
        return address;
    }
    std::ostringstream stream;
    stream<<((addressIpv4>>OHOS::Wifi::BITS_24) & 0xFF)<<"."<<((addressIpv4>>OHOS::Wifi::BITS_16) & 0xFF)<<"."
          <<((addressIpv4>>OHOS::Wifi::BITS_8) & 0xFF)<<"."<<(addressIpv4 & 0xFF);
    address = stream.str();
    return address;
}

std::string Utils::GetWifiIp()
{
    std::shared_ptr<OHOS::Wifi::WifiDevice> wifiDevice = OHOS::Wifi::WifiDevice::GetInstance(WIFI_DEVICE_ABILITY_ID);
    if (wifiDevice == nullptr) {
        return "";
    }
    OHOS::Wifi::IpInfo ipInfo;
    ErrCode ret = wifiDevice->GetIpInfo(ipInfo);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        return "";
    }
    std::string strIp = Utils::ConvertIpv4Address(ipInfo.ipAddress);
    return strIp;
}

bool Utils::IsArrayAllZero(const uint8_t *input, int length)
{
    bool isAllZero = true;
    for (int i = 0; i < length; i++) {
        if (input[i] != 0) {
            isAllZero = false;
            break;
        }
    }
    return isAllZero;
}

uint64_t Utils::TimeMilliSecond()
{
    struct timeval curTime = {0, 0};
    gettimeofday(&curTime, nullptr);
    return static_cast<int64_t>(curTime.tv_sec) * USEC_1000 + curTime.tv_usec / USEC_1000;
}

std::string Utils::Mask(const std::string &str)
{
    if (str.empty() || str.length() <= MASK_MIN_LEN) {
        return str;
    } else if (str.length() < (MASK_PRINT_PREFIX_LEN + MASK_PRINT_POSTFIX_LEN)) {
        return str.substr(0, MASK_MIN_LEN) + "***" + str.substr(str.length() - 1);
    } else {
        return str.substr(0, MASK_PRINT_PREFIX_LEN) + "***" + str.substr(str.length() - MASK_PRINT_POSTFIX_LEN);
    }
}

DrmType Utils::DrmUuidToType(std::string drmUUID)
{
    if (drmUUID == UUID_CHINADRM) {
        return DrmType::CHINADRM;
    } else if (drmUUID == UUID_WIDEVINE) {
        return DrmType::WIDEVINE;
    } else if (drmUUID == UUID_PLAYREADY) {
        return DrmType::PLAYREADY;
    }
    return DrmType::DRM_BASE;
}

int32_t Utils::GetCurrentActiveAccountUserId()
{
    std::vector<int> activatedOsAccountIds;
    OHOS::ErrCode res = OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds);
    if (res != OHOS::ERR_OK || activatedOsAccountIds.size() <= 0) {
        CLOGE("QueryActiveOsAccountIds failed res:%{public}d", res);
        return DEFAULT_OS_ACCOUNT_ID;
    }

    int osAccountId = activatedOsAccountIds[0];
    if (osAccountId != DEFAULT_OS_ACCOUNT_ID) {
        CLOGI("currentOsAccount:%{public}d", osAccountId);
    }

    return osAccountId;
}

std::string Utils::GetOhosAccountId()
{
    AccountSA::OhosAccountInfo accountInfo;
    OHOS::ErrCode res = AccountSA::OhosAccountKits::GetInstance().GetOhosAccountInfo(accountInfo);
    if (res != OHOS::ERR_OK || accountInfo.uid_ == "") {
        CLOGE("GetOhosAccountInfo failed res:%{public}d", res);
        return "";
    }

    return accountInfo.uid_;
}

int Utils::SetFirstTokenID()
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    auto ret = SetFirstCallerTokenID(tokenId);
    CLOGI("SetFirstCallerTokenID ret %{public}d tokenId %{public}d", ret, tokenId);
    return ret;
}

void Utils::EnablePowerForceTimingOut()
{
    CLOGI("in");
    if (isEnablePowerForceTimingOut.load()) {
        CLOGI("already enable");
        return;
    }

    isEnablePowerForceTimingOut = true;
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    // 强制使能超时灭屏
    powerMgrClient.SetForceTimingOut(true);
    // 超时锁屏，但有常亮锁时不锁屏（如华为视频播放视频时不锁屏），且不发送熄屏广播
    powerMgrClient.LockScreenAfterTimingOut(true, true, false);
}

void Utils::ResetPowerForceTimingOut()
{
    CLOGI("in");

    if (!isEnablePowerForceTimingOut.load()) {
        CLOGI("already reset");
        return;
    }
    isEnablePowerForceTimingOut = false;
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    // 不强制使能超时灭屏
    powerMgrClient.SetForceTimingOut(false);
    // 超时锁屏，不受常亮锁影响（如华为视频播放视频时也锁屏），恢复发送熄屏广播
    powerMgrClient.LockScreenAfterTimingOut(true, false, true);
}

void Utils::LightAndLockScreen()
{
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    // 结束投屏时，如果手机是熄屏状态，要亮屏并锁屏
    bool isScreenOn = powerMgrClient.IsScreenOn();
    CLOGI("isScreenOn: %{public}d", isScreenOn);
    if (!isScreenOn) {
        PowerErrors wakeupRet = powerMgrClient.WakeupDevice(WakeupDeviceType::WAKEUP_DEVICE_APPLICATION,
                                                            std::string("cast+ exit playing"));
        PowerErrors suspendRet = powerMgrClient.SuspendDevice();
        CLOGI("wakeupRet: %{public}d, suspendRet: %{public}d", wakeupRet, suspendRet);
    }
}

void Utils::SetThreadName(const std::string &name)
{
    if (name.size() == 0) {
        return;
    }

    static std::atomic<unsigned int> suffix = 0;
    std::string threadName = name + "-" + std::to_string(suffix++);
    if (prctl(PR_SET_NAME, threadName.c_str()) < 0) {
        CLOGE("prctl errno %d", errno);
    }
}

} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
