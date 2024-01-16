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
 * Description: encrypt and decrypt data module
 * Author: lijianzhao
 * Create: 2022-01-19
 */

#ifndef ENCRYPT_DECRYPT_H
#define ENCRYPT_DECRYPT_H

#include <cstdint>
#include <string>

#include "openssl/hmac.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "utils.h"
#include "singleton.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
struct EncryptInfo {
    PacketData aad;
    PacketData tag;
    ConstPacketData key;
    ConstPacketData iv;
};

class EncryptDecrypt final {
    DECLARE_SINGLETON(EncryptDecrypt);

public:
    static EncryptDecrypt &GetInstance();

    bool EncryptData(int algCode, const uint8_t *key, int keyLen, ConstPacketData inputData, PacketData &outputData);
    bool DecryptData(int algCode, const uint8_t *key, int keyLen, ConstPacketData inputData, PacketData &outputData);
    std::string GetEncryptInfo();
    int GetEncryptMatch(const std::string &encyptInfo);
    int GetVersion();

    static const int AES_KEY_LEN_128 = 16;
    static const unsigned int AES_IV_LEN = 16;
    static const int AES_KEY_LEN = 16;
    static const int AES_KEY_SIZE = 16;
    static const int PC_ENCRYPT_LEN = 64;

    static const int INVALID_CODE = -1;
    static const int DEFAULT_CODE = 0;
    static const int CTR_CODE = 1;
    static const int GCM_CODE = 2;

    static const std::string PC_ENCRYPT_ALG;

private:
    enum ErrorCode : int {
        SEC_COMMON_ERR_BASE = 0x66010000,
        SEC_ERR_CREATECIPHER_FAIL = SEC_COMMON_ERR_BASE + 10,
        SEC_ERR_ENCRYPTUPDATE_FAIL,
        SEC_ERR_ENCRYPTFINAL_FAIL,
        SEC_ERR_GCMGETTAG_FAIL,
        SEC_ERR_INVALID_AAD,
        SEC_ERR_INVALID_CID,
        SEC_ERR_INVALID_DATA_LEN,
        SEC_ERR_INVALID_EXTN,
        SEC_ERR_INVALID_IV,
        SEC_ERR_INVALID_IV_LEN,
        SEC_ERR_INVALID_KEY,
        SEC_ERR_INVALID_KEY_LEN,
        SEC_ERR_INVALID_MAC,
        SEC_ERR_INVALID_MODE,
        SEC_ERR_INVALID_OID,
        SEC_ERR_INVALID_PRINTABLE,
        SEC_ERR_INVALID_SALT_LEN,
        SEC_ERR_INVALID_SERIALNUMBER,
        SEC_ERR_INVALID_VERSION,
        SEC_ERR_NONCE_MISMATCH,
        SEC_ERR_MALLOC_FAIL,
        SEC_ERR_NULL_PTR,
        SEC_ERR_INITLIB,
        SEC_ERR_INITDEVICE,
        SEC_ERR_MEMCPY_FAILED,
        SEC_ERR_MEMSET_FAILED,
        SEC_ERR_INVALID_PLAIN,
        SEC_ERR_INVALID_CIPHERTEXT,
        SEC_ERR_SETAAD_FAIL,
    };

    static const int AES_GCM_MAX_IVLEN = 12;
    static const int AES_GCM_SIV_TAG_LEN = 16;
    static const int UNSIGNED_CHAR_MIN = 0;
    static const int UNSIGNED_CHAR_MAX = 255;

    static const int VERSION = 1;

    void GetAESIv(uint8_t iv[], int ivLen);
    int AES128Encry(ConstPacketData inputData, PacketData &outputData, ConstPacketData sessionKey,
        ConstPacketData sessionIV);
    int AES128Decrypt(ConstPacketData inputData, PacketData &outputData, ConstPacketData sessionKey,
        ConstPacketData sessionIV);
    int AES128GCMCheckEncryPara(ConstPacketData inputData, PacketData &outputData, EncryptInfo &encryInfo);
    int AES128GCMCheckDecryptPara(ConstPacketData inputData, PacketData &outputData, EncryptInfo &encryInfo);
    int EnctyptProcess(ConstPacketData inputData, PacketData &outputData, EncryptInfo &encryInfo, EVP_CIPHER_CTX *ctx);
    int DecryptProcess(ConstPacketData inputData, PacketData &outputData, EncryptInfo &encryInfo, EVP_CIPHER_CTX *ctx);
    int AES128GCMEncry(ConstPacketData inputData, PacketData &outputData, EncryptInfo &encryInfo);
    int AES128GCMDecrypt(ConstPacketData inputData, PacketData &outputData, EncryptInfo &encryInfo);
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // ENCRYPT_DECRYPT_H
