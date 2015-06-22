/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_ICRYPTOHASH
#define _CRYPTO_STREAMS_LIB_ICRYPTOHASH
#include <stdint.h>
namespace rmscrypto {
namespace api {
enum CryptoHashAlgorithm
{
  CRYPTO_HASH_ALGORITHM_SHA1   = 0,
  CRYPTO_HASH_ALGORITHM_SHA256 = 1,
};

class ICryptoHash {
public:

  virtual size_t GetOutputSize() = 0;
  virtual void   Hash(const uint8_t *pbIn,
                      uint32_t       cbIn,
                      uint8_t       *pbOut,
                      uint32_t     & cbOut) = 0;
};
} // namespace api
} // namespace rmscrypto

#endif // _CRYPTO_STREAMS_LIB_ICRYPTOHASH
