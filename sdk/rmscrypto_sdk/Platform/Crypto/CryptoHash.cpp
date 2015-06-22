/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "CryptoHash.h"
#include "../../CryptoAPI/RMSCryptoExceptions.h"
using namespace std;

namespace rmscrypto {
namespace platform {
namespace crypto {
CryptoHash::CryptoHash(std::shared_ptr<QCryptographicHash> hash,
                       api::CryptoHashAlgorithm  algorithm)
  : m_hash(hash)
  , m_algorithm(algorithm)
{}

size_t CryptoHash::GetOutputSize()
{
  switch (m_algorithm)
  {
  case api::CRYPTO_HASH_ALGORITHM_SHA1:
    return 20;

  case api::CRYPTO_HASH_ALGORITHM_SHA256:
    return 32;

  default:
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid algorithm");
  }
}

void CryptoHash::Hash(const uint8_t *pbIn,
                      uint32_t       cbIn,
                      uint8_t       *pbOut,
                      uint32_t     & cbOut)
{
  m_hash->addData(reinterpret_cast<const char *>(pbIn), cbIn);

  // get the hash valuew
  auto valueBuffer = m_hash->result();
  m_hash->reset();

  // check that the output buffer is big enough
  if (static_cast<int>(cbOut) < valueBuffer.length()) {
    throw exceptions::RMSCryptoInvalidArgumentException("Bounds error");
  }

  // set the output size
  cbOut = valueBuffer.length();

  // copy
#ifdef Q_OS_WIN32
  memcpy_s(pbOut, cbOut, valueBuffer.data(), valueBuffer.size());
#else
  memcpy(pbOut, valueBuffer.data(), valueBuffer.size());
#endif
}
} // namespace crypto
} // namespace platform
} // namespace rmscrypto
