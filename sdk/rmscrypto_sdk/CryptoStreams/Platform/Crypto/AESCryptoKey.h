/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_CRYPTOKEY_
#define _CRYPTO_STREAMS_LIB_CRYPTOKEY_
#include <openssl/evp.h>
#include <string>
#include <vector>

#include "../../CryptoAPI/ICryptoKey.h"
#include "../../CryptoAPI/ICryptoEngine.h"
namespace rmscrypto {
namespace platform {
namespace crypto {
class AESCryptoKey : public api::ICryptoKey {
public:

  AESCryptoKey(const uint8_t   *pbKey,
               uint32_t         cbKey,
               api::CryptoAlgorithm& algorithm);
  ~AESCryptoKey();

  virtual void Encrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint8_t       *pbOut,
                       uint32_t     & cbOut,
                       const uint8_t *pbIv,
                       uint32_t       cbIv) override;
  virtual void Decrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint8_t       *pbOut,
                       uint32_t     & cbOut,
                       const uint8_t *pbIv,
                       uint32_t       cbIv) override;

private:

  void TransformBlock(bool           encrypt,
                      const uint8_t *pbIn,
                      uint32_t       cbIn,
                      uint8_t       *pbOut,
                      uint32_t     & cbOut,
                      const uint8_t *pbIv,
                      uint32_t       cbIv);


  std::vector<uint8_t> m_key;
  api::CryptoAlgorithm m_algorithm;
};
} // namespace crypto
} // namespace platform
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_CRYPTOKEY_
