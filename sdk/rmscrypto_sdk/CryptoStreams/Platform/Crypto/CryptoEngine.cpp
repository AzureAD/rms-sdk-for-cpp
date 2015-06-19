/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <string>
#include <QCryptographicHash>
#include "CryptoEngine.h"
#include "../../CryptoAPI/RMSCryptoExceptions.h"
using namespace std;

namespace rmscrypto {
namespace api {
shared_ptr<ICryptoEngine>ICryptoEngine::Create() {
  return make_shared<platform::crypto::CryptoEngine>();
}
}
namespace platform {
namespace crypto {
static QCryptographicHash::Algorithm MapHashAlgorithm(
  api::CryptoHashAlgorithm algorithm)
{
  switch (algorithm)
  {
  case api::CRYPTO_HASH_ALGORITHM_SHA1:
    return QCryptographicHash::Sha1;

  case api::CRYPTO_HASH_ALGORITHM_SHA256:
    return QCryptographicHash::Sha256;

  default:
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid algorithm");
  }
}

shared_ptr<api::ICryptoKey>CryptoEngine::CreateKey(const uint8_t       *pbKey,
                                                   uint32_t             cbKey,
                                                   api::CryptoAlgorithm algorithm)
{
  if ((algorithm == api::CRYPTO_ALGORITHM_AES_ECB) ||
      (algorithm == api::CRYPTO_ALGORITHM_AES_CBC) ||
      (algorithm == api::CRYPTO_ALGORITHM_AES_CBC_PKCS7)) {
    return make_shared<AESCryptoKey>(pbKey, cbKey, algorithm);
  }

  // no algorithm
  throw exceptions::RMSCryptoInvalidArgumentException("Invalid algorithm");
}

shared_ptr<api::ICryptoHash>CryptoEngine::CreateHash(
  api::CryptoHashAlgorithm algorithm)
{
  auto hash = make_shared<QCryptographicHash>(MapHashAlgorithm(algorithm));

  return make_shared<CryptoHash>(hash, algorithm);
}
} // namespace crypto
} // namespace platform
} // namespace rmscrypto
