/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_ECBCRYPTOPROVIDER_H_
#define _CRYPTO_STREAMS_LIB_ECBCRYPTOPROVIDER_H_

#include "../CryptoAPI/CryptoAPI.h"
#include "CryptoConstants.h"

namespace rmscrypto {
namespace crypto {
class EcbCryptoProvider : public api::ICryptoProvider {
public:

  EcbCryptoProvider(const std::vector<uint8_t>& key);

  virtual void Encrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint32_t       dwStartingBlockNumber,
                       bool           isFinal,
                       uint8_t       *pbOut,
                       uint32_t       cbOut,
                       uint32_t      *pcbOut) override;
  virtual void Decrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint32_t       dwStartingBlockNumber,
                       bool           isFinal,
                       uint8_t       *pbOut,
                       uint32_t       cbOut,
                       uint32_t      *pcbOut) override;

  virtual uint64_t GetCipherTextSize(uint64_t clearTextSize) override {
    return clearTextSize;
  }

  virtual uint32_t GetBlockSize() override {
    return AES128_BLOCK_SIZE;
  }

  virtual std::vector<uint8_t>GetKey() override {
    return m_key;
  }

private:

  std::shared_ptr<api::ICryptoKey> m_pKey;
  std::vector<uint8_t> m_key;
};
} // namespace crypto
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_ECBCRYPTOPROVIDER_H_
