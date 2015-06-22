/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_ICRYPTOENGINE
#define _CRYPTO_STREAMS_LIB_ICRYPTOENGINE

#include <memory>
#include "ICryptoHash.h"
#include "ICryptoKey.h"

namespace rmscrypto {
namespace api {
enum CryptoAlgorithm
{
  CRYPTO_ALGORITHM_AES_ECB       = 0,
  CRYPTO_ALGORITHM_AES_CBC       = 1,
  CRYPTO_ALGORITHM_AES_CBC_PKCS7 = 2,
};

class ICryptoEngine {
public:

  virtual std::shared_ptr<ICryptoKey>CreateKey(const uint8_t  *pbKey,
                                               uint32_t        cbKey,
                                               CryptoAlgorithm algorithm) =
    0;
  virtual std::shared_ptr<ICryptoHash> CreateHash(CryptoHashAlgorithm algorithm) =
    0;
  static std::shared_ptr<ICryptoEngine>Create();
};
} // namespace api
} // namespace rmscrypto

#endif // _CRYPTO_STREAMS_LIB_ICRYPTOENGINE
