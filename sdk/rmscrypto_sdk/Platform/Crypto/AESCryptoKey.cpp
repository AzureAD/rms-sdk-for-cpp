/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <cstring>
#include "AESCryptoKey.h"
#include "../../CryptoAPI/RMSCryptoExceptions.h"

using namespace std;

namespace rmscrypto {
namespace platform {
namespace crypto {
AESCryptoKey::AESCryptoKey(const uint8_t *pbKey, uint32_t cbKey,
                           api::CryptoAlgorithm& algorithm)
  : m_key(cbKey), m_algorithm(algorithm)
{
  if (cbKey == 0) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid key length");
  }
  memcpy(&m_key[0], pbKey, cbKey);
}

AESCryptoKey::~AESCryptoKey() {}

void AESCryptoKey::Encrypt(const uint8_t *pbIn,
                           uint32_t       cbIn,
                           uint8_t       *pbOut,
                           uint32_t     & cbOut,
                           const uint8_t *pbIv,
                           uint32_t       cbIv)
{
  TransformBlock(true, pbIn, cbIn, pbOut, cbOut, pbIv, cbIv);
}

void AESCryptoKey::Decrypt(const uint8_t *pbIn,
                           uint32_t       cbIn,
                           uint8_t       *pbOut,
                           uint32_t     & cbOut,
                           const uint8_t *pbIv,
                           uint32_t       cbIv)
{
  TransformBlock(false, pbIn, cbIn, pbOut, cbOut, pbIv, cbIv);
}

void AESCryptoKey::TransformBlock(bool           encrypt,
                                  const uint8_t *pbIn,
                                  uint32_t       cbIn,
                                  uint8_t       *pbOut,
                                  uint32_t     & cbOut,
                                  const uint8_t *pbIv,
                                  uint32_t       cbIv)
{
  if (pbIn == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbIn exception");
  }

  if (pbOut == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbOut exception");
  }

  if (((cbIv == 0) && (pbIv != nullptr)) || ((cbIv != 0) && (pbIv == nullptr))) {
    pbIv = nullptr;
    cbIv = 0;
  }

  int totalOut = static_cast<int>(cbOut);
  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);
  const EVP_CIPHER *cipher = nullptr;

  switch (m_algorithm) {
  case api::CRYPTO_ALGORITHM_AES_ECB:
    switch(m_key.size()) {
    case 16:
       cipher = EVP_aes_128_ecb();
       break;
    case 24:
       cipher = EVP_aes_192_ecb();
       break;
    case 32:
       cipher = EVP_aes_256_ecb();
       break;
    default:
        throw exceptions::RMSCryptoInvalidArgumentException("Invalid key length");
    }
    break;

  case api::CRYPTO_ALGORITHM_AES_CBC:
  case api::CRYPTO_ALGORITHM_AES_CBC_PKCS7:
      switch(m_key.size()) {
      case 16:
         cipher = EVP_aes_128_cbc();
         break;
      case 24:
         cipher = EVP_aes_192_cbc();
         break;
      case 32:
         cipher = EVP_aes_256_cbc();
         break;
      default:
          throw exceptions::RMSCryptoInvalidArgumentException("Invalid key length");
      }
      break;
    break;

  default:
    throw exceptions::RMSCryptoInvalidArgumentException("Unsupported algorithm");
  }

  // check lengths
  if ((pbIv != nullptr) &&
      (EVP_CIPHER_iv_length(cipher) != static_cast<int>(cbIv))) {
    throw exceptions::RMSCryptoInvalidArgumentException(
            "Invalid initial vector length");
  }

  if (EVP_CIPHER_key_length(cipher) != static_cast<int>(m_key.size())) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid key length");
  }

  EVP_CipherInit_ex(&ctx, cipher, NULL, m_key.data(), pbIv, encrypt ? 1 : 0);

  if (m_algorithm == api::CRYPTO_ALGORITHM_AES_CBC_PKCS7) {
    EVP_CIPHER_CTX_set_padding(&ctx, 1);
  } else {
    EVP_CIPHER_CTX_set_padding(&ctx, 0);
  }

  if (!EVP_CipherUpdate(&ctx, pbOut, &totalOut, pbIn, static_cast<int>(cbIn))) {
    throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoException::UnknownError,
            "Failed to transform data");
  }

  pbOut += totalOut;

  // add padding if necessary
  if (m_algorithm == api::CRYPTO_ALGORITHM_AES_CBC_PKCS7) {
    int remain = cbOut - totalOut;

    if (remain < EVP_CIPHER_block_size(cipher)) {
      throw exceptions::RMSCryptoInsufficientBufferException(
              "No enough buffer size");
    }

    if (!EVP_CipherFinal_ex(&ctx, pbOut, &remain)) {
      throw exceptions::RMSCryptoIOException(
              exceptions::RMSCryptoException::UnknownError,
              "Failed to transform final block");
    }
    totalOut += remain;
  }

  EVP_CIPHER_CTX_cleanup(&ctx);

  // remember total size
  cbOut = static_cast<uint32_t>(totalOut);
}
} // namespace crypto
} // namespace platform
} // namespace rmscrypto
