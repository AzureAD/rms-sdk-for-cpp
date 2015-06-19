/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "EcbCryptoProvider.h"
#include "CryptoConstants.h"
#include "../CryptoAPI/CryptoAPI.h"
#include "../CryptoAPI/RMSCryptoExceptions.h"

using namespace std;
using namespace rmscrypto::api;

namespace rmscrypto {
namespace crypto {
EcbCryptoProvider::EcbCryptoProvider(const vector<uint8_t>& key)
{
  m_key = key;
  shared_ptr<ICryptoEngine> pCryptoEngine = api::ICryptoEngine::Create();
  m_pKey = pCryptoEngine->CreateKey(key.data(),
                                    static_cast<uint32_t>(key.size()),
                                    CRYPTO_ALGORITHM_AES_ECB);
}

void EcbCryptoProvider::Encrypt(const uint8_t *pbIn,
                                uint32_t       cbIn,
                                uint32_t,
                                bool,
                                uint8_t       *pbOut,
                                uint32_t       cbOut,
                                uint32_t      *pcbOut)
{
  if (pbIn == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbIn exception");
  }

  if (0 != cbIn % AES128_BLOCK_SIZE) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  if (nullptr == pcbOut) {
    throw exceptions::RMSCryptoNullPointerException(
            "Null pointer pcbOut exception");
  }

  *pcbOut = cbIn;

  if (nullptr == pbOut)
  {
    return;
  }

  if (cbOut < cbIn) {
    throw exceptions::RMSCryptoInsufficientBufferException("Insufficient buffer");
  }

  m_pKey->Encrypt(pbIn, cbIn, pbOut, cbOut, nullptr, 0);
}

void EcbCryptoProvider::Decrypt(const uint8_t *pbIn,
                                uint32_t       cbIn,
                                uint32_t,
                                bool,
                                uint8_t       *pbOut,
                                uint32_t       cbOut,
                                uint32_t      *pcbOut)
{
  if (pbIn == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbIn exception");
  }

  if (0 != cbIn % AES128_BLOCK_SIZE) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  if (nullptr == pcbOut) {
    throw exceptions::RMSCryptoNullPointerException(
            "Null pointer pcbOut exception");
  }

  *pcbOut = cbIn;

  if (nullptr == pbOut)
  {
    return;
  }

  if (cbOut < cbIn) {
    throw exceptions::RMSCryptoInsufficientBufferException("Insufficient buffer");
  }

  m_pKey->Decrypt(pbIn, cbIn, pbOut, cbOut, nullptr, 0);
}
} // namespace crypto
} // namespace rmscrypto
