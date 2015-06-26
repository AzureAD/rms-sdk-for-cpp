/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _CRYPTO_STREAMS_LIB_BASE64_H
#define _CRYPTO_STREAMS_LIB_BASE64_H
#include <string>

namespace rmscrypto {
namespace platform {
namespace keystorage {
std::string base64_encode(unsigned char const *,
                          unsigned int len);
std::string base64_decode(const std::string& s);
} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
#endif // ifndef _CRYPTO_STREAMS_LIB_BASE64_H
