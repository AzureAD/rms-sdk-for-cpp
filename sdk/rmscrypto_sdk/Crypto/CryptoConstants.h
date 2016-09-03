/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_CRYPTODEFS_H_
#define _CRYPTO_STREAMS_LIB_CRYPTODEFS_H_

#include <string>

namespace rmscrypto {
namespace crypto {
const unsigned int AES128_KEY_BYTE_LENGTH = 16;
const unsigned int AES128_BLOCK_SIZE      = 16;
const unsigned int CBC4K_BLOCK_SIZE       = 4096;
const unsigned int PADDED_CBC4K_SIZE      = 4412;
const unsigned int CBC512_BLOCK_SIZE      = 512;
const std::string szAES                   = "AES";
const std::string szCBC4K                 = "CBC4k";
const std::string szECB                   = "ECB";
const std::string szSHA1                  = "SHA1";
const std::string szSHA256                = "SHA256";
} // namespace crypto
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_CRYPTODEFS_H_
