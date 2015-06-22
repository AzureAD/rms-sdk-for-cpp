/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_CBC4KCRYPTOPROVIDER_H_
#define _CRYPTO_STREAMS_LIB_CBC4KCRYPTOPROVIDER_H_

#include "../CryptoAPI/CryptoAPI.h"
#include "CryptoConstants.h"


namespace rmscrypto {
namespace crypto {
class Cbc4kCryptoProvider : public api::ICryptoProvider {
public:

  Cbc4kCryptoProvider(const std::vector<uint8_t>& key);

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
    return ((clearTextSize / AES128_BLOCK_SIZE) + 1) * AES128_BLOCK_SIZE;
  }

  virtual uint32_t GetBlockSize() override {
    return CBC4K_BLOCK_SIZE;
  }

  virtual std::vector<uint8_t> GetKey() override {
    return m_key;
  }

private:

  uint32_t EncryptBlock(const uint8_t *pbIn,
                        uint32_t       cbIn,
                        uint32_t       dwBlockNumber,
                        bool           isFinalBlock,
                        uint8_t       *pbOut,
                        uint32_t       cbOut);
  uint32_t DecryptBlock(const uint8_t *pbIn,
                        uint32_t       cbIn,
                        uint32_t       dwBlockNumber,
                        bool           isFinalBlock,
                        uint8_t       *pbOut,
                        uint32_t       cbOut);

  std::vector<uint8_t> GenerateIvForBlock(uint32_t dwBlockNumber);

  static uint32_t     GetPaddedSize(uint32_t cbSize);

private:

  std::shared_ptr<api::ICryptoKey> m_pEcbKey;
  std::shared_ptr<api::ICryptoKey> m_pCbcKey;
  std::shared_ptr<api::ICryptoKey> m_pCbcPaddingKey;
  std::vector<uint8_t> m_key;
};
} // namespace crypto
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_CBC4KCRYPTOPROVIDER_H_
