/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_CRYPTOENGINE_
#define _CRYPTO_STREAMS_LIB_CRYPTOENGINE_

#include "../../CryptoAPI/ICryptoEngine.h"
#include "AESCryptoKey.h"
#include "CryptoHash.h"
namespace rmscrypto {
namespace platform {
namespace crypto {
class CryptoEngine : public api::ICryptoEngine {
public:

  virtual std::shared_ptr<api::ICryptoKey>CreateKey(const uint8_t  *pbKey,
                                               uint32_t        cbKey,
                                               api::CryptoAlgorithm algorithm)
  override;
  virtual std::shared_ptr<api::ICryptoHash>CreateHash(api::CryptoHashAlgorithm algorithm)
  override;
};
} // namespace crypto
} // namespace platform
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_CRYPTOENGINE_
