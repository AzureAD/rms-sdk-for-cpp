/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/
#ifndef IRSACRYPTOKEY_H
#define IRSACRYPTOKEY_H
#include <vector>
#include <string>
#include <stdint.h>

namespace rmscrypto {
namespace api {
class IRSAKeyBlob
{
public:
    virtual std::vector<uint8_t> Sign(std::vector<uint8_t> digest, uint32_t& outsize) = 0;

    virtual bool VerifySignature(std::vector<uint8_t> signature, std::vector<uint8_t> digest, std::string& outMsg, uint32_t retsize) = 0;

    virtual std::vector<uint8_t> PublicEncrypt(std::vector<uint8_t> buffer) = 0;

    virtual std::vector<uint8_t> PrivateDecrypt(std::vector<uint8_t> buffer) = 0;
};
} //api
} //rmscrypto
#endif // IRSACRYPTOKEY_H
