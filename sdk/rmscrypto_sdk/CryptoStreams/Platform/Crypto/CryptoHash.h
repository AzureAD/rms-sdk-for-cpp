/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_CRYPTOHASH_
#define _CRYPTO_STREAMS_LIB_CRYPTOHASH_
#include <memory>
#include <QCryptographicHash>
#include "../../CryptoAPI/ICryptoHash.h"
namespace rmscrypto {
namespace platform {
namespace crypto {
class CryptoHash : public api::ICryptoHash {
public:

  CryptoHash(std::shared_ptr<QCryptographicHash>hash,
             api::CryptoHashAlgorithm                algorithm);

  virtual size_t GetOutputSize() override;
  virtual void   Hash(const uint8_t *pbIn,
                      uint32_t       cbIn,
                      uint8_t       *pbOut,
                      uint32_t     & cbOut) override;

private:

  std::shared_ptr<QCryptographicHash> m_hash;
  api::CryptoHashAlgorithm m_algorithm;
};
} // namespace crypto
} // namespace platform
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_CRYPTOHASH_
