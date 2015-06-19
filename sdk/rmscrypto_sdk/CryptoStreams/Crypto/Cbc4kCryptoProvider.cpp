/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <cstring>
#include "Cbc4kCryptoProvider.h"
#include "CryptoConstants.h"
#include "../CryptoAPI/CryptoAPI.h"
#include "../CryptoAPI/RMSCryptoExceptions.h"

using namespace std;
using namespace rmscrypto::api;

namespace rmscrypto {
namespace crypto {
Cbc4kCryptoProvider::Cbc4kCryptoProvider(const vector<uint8_t>& key)
{
  if (key.size() < AES128_KEY_BYTE_LENGTH)
  {
      throw exceptions::RMSCryptoInvalidArgumentException("Invalid key size");
  }

  m_key = key;
  shared_ptr<ICryptoEngine> pCryptoEngine = api::ICryptoEngine::Create();

  m_pEcbKey = pCryptoEngine->CreateKey(key.data(),
                                       static_cast<uint32_t>(key.size()),
                                       CRYPTO_ALGORITHM_AES_ECB);
  m_pCbcKey = pCryptoEngine->CreateKey(key.data(),
                                       static_cast<uint32_t>(key.size()),
                                       CRYPTO_ALGORITHM_AES_CBC);
  m_pCbcPaddingKey = pCryptoEngine->CreateKey(key.data(),
                                              static_cast<uint32_t>(key.size()),
                                              CRYPTO_ALGORITHM_AES_CBC_PKCS7);
}

void Cbc4kCryptoProvider::Encrypt(const uint8_t *pbIn,
                                  uint32_t       cbInUnsafe,
                                  uint32_t       dwStartingBlockNumberUnsafe,
                                  bool           isFinal,
                                  uint8_t       *pbOut,
                                  uint32_t       cbOutUnsafe,
                                  uint32_t      *pcbOut)
{
  auto cbIn                  = cbInUnsafe, cbOut = cbOutUnsafe,
       dwStartingBlockNumber = dwStartingBlockNumberUnsafe;

  if (pbIn == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbIn exception");
  }

  if (!isFinal && (0 != cbIn % CBC4K_BLOCK_SIZE)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  if (nullptr == pcbOut) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pcbOut exception");
  }

  // if it's the final block add padding, otherwise the output size is the same
  uint32_t cbRequired = isFinal ? GetPaddedSize(cbIn) : cbIn;

  if (nullptr == pbOut)
  {
    // No need to do the encryption, just return the number of bytes required
    *pcbOut = cbRequired;
    return;
  }

  if (cbOut < cbRequired) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid buffer size");
  }

  auto cbResult = 0;

  while (cbIn >= CBC4K_BLOCK_SIZE)
  {
    if (cbOut < CBC4K_BLOCK_SIZE) {
      throw exceptions::RMSCryptoInvalidArgumentException("Invalid buffer size");
    }

    // Encrypt the current block
    EncryptBlock(pbIn,
                 CBC4K_BLOCK_SIZE,
                 dwStartingBlockNumber,
                 false,
                 pbOut,
                 cbOut);

    // Go to the next block

    pbIn += CBC4K_BLOCK_SIZE;
    cbIn -= CBC4K_BLOCK_SIZE;

    pbOut += CBC4K_BLOCK_SIZE;
    cbOut -= CBC4K_BLOCK_SIZE;

    ++dwStartingBlockNumber;

    cbResult += CBC4K_BLOCK_SIZE;
  }

  if (!isFinal && (cbIn != 0)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid aligment");
  }

  if (isFinal)
  {
    // Note that cbIn might be 0 when isFinal == true and 0 == cbIn %
    // CBC4K_BLOCK_SIZE.
    // In that case we just encrypt an empty buffer as final and get a padding
    // block of AES128_BLOCK_SIZE (16) bytes.
    cbResult +=
      EncryptBlock(pbIn, cbIn, dwStartingBlockNumber, true, pbOut, cbOut);
  }

  *pcbOut = cbResult;
}

void Cbc4kCryptoProvider::Decrypt(const uint8_t *pbIn,
                                  uint32_t       cbInUnsafe,
                                  uint32_t       dwStartingBlockNumberUnsafe,
                                  bool           isFinal,
                                  uint8_t       *pbOut,
                                  uint32_t       cbOutUnsafe,
                                  uint32_t      *pcbOut)
{
  auto cbIn                  = cbInUnsafe, cbOut = cbOutUnsafe,
       dwStartingBlockNumber = dwStartingBlockNumberUnsafe;

  if (pbIn == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbIn exception");
  }

  if (!isFinal && (0 != cbIn % CBC4K_BLOCK_SIZE)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  if (0 != cbIn % AES128_BLOCK_SIZE) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  if (nullptr == pcbOut) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pcbOut exception");
  }

  if (nullptr == pbOut)
  {
    // No need to do the decryption, just return the number of bytes required
    *pcbOut = cbIn;
    return;
  }

  if (cbOut < cbIn) {
    throw exceptions::RMSCryptoInsufficientBufferException("Insufficient buffer");
  }

  auto cbResult = 0;

  // If this is the final chunk of the data, don't decrypt the final block in
  // the loop (even if it's 4K). It needs a special
  // treatment and will be decrypted separately.
  while (isFinal
         ? cbIn > CBC4K_BLOCK_SIZE
         : cbIn >= CBC4K_BLOCK_SIZE)
  {
    if (cbOut < CBC4K_BLOCK_SIZE) {
      throw exceptions::RMSCryptoInsufficientBufferException("Insufficient buffer");
    }

    DecryptBlock(pbIn,
                 CBC4K_BLOCK_SIZE,
                 dwStartingBlockNumber,
                 false,
                 pbOut,
                 cbOut);

    pbIn += CBC4K_BLOCK_SIZE;
    cbIn -= CBC4K_BLOCK_SIZE;

    pbOut += CBC4K_BLOCK_SIZE;
    cbOut -= CBC4K_BLOCK_SIZE;

    ++dwStartingBlockNumber;

    cbResult += CBC4K_BLOCK_SIZE;
  }

  if (!isFinal && (cbIn != 0)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid aligment");
  }

  if (isFinal)
  {
    // Note that the cbIn can't be 0, the final block always should have at
    // least AES128_BLOCK_SIZE (16) bytes
    if (cbIn < AES128_BLOCK_SIZE) {
      throw exceptions::RMSCryptoInvalidArgumentException("Invalid aligment");
    }
    cbResult +=
      DecryptBlock(pbIn, cbIn, dwStartingBlockNumber, true, pbOut, cbOut);
  }

  *pcbOut = cbResult;
}

uint32_t Cbc4kCryptoProvider::EncryptBlock(const uint8_t *pbIn,
                                           uint32_t       cbIn,
                                           uint32_t       dwBlockNumber,
                                           bool           isFinalBlock,
                                           uint8_t       *pbOut,
                                           uint32_t       cbOut)
{
  if (pbIn == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbIn exception");
  }

  if (pbOut == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbOut exception");
  }

  if (!isFinalBlock && (CBC4K_BLOCK_SIZE != cbIn)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid aligment");
  }

  // Generate the IV
  vector<uint8_t> iv = GenerateIvForBlock(dwBlockNumber);

  if (!isFinalBlock)
  {
    m_pCbcKey->Encrypt(pbIn, cbIn, pbOut, cbOut, &iv[0],
                       static_cast<uint32_t>(iv.size()));
  }
  else
  {
    m_pCbcPaddingKey->Encrypt(pbIn, cbIn, pbOut, cbOut, &iv[0],
                              static_cast<uint32_t>(iv.size()));
  }

  return cbOut;
}

uint32_t Cbc4kCryptoProvider::DecryptBlock(const uint8_t *pbIn,
                                           uint32_t       cbIn,
                                           uint32_t       dwBlockNumber,
                                           bool           isFinalBlock,
                                           uint8_t       *pbOut,
                                           uint32_t       cbOut)
{
  if (pbIn == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbIn exception");
  }

  if (pbOut == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pbOut exception");
  }

  if (!isFinalBlock && (CBC4K_BLOCK_SIZE != cbIn)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  // Generate the IV
  vector<uint8_t> iv = GenerateIvForBlock(dwBlockNumber);

  if (!isFinalBlock)
  {
    m_pCbcKey->Decrypt(pbIn, cbIn, pbOut, cbOut, &iv[0],
                       static_cast<uint32_t>(iv.size()));
  }
  else
  {
    m_pCbcPaddingKey->Decrypt(pbIn, cbIn, pbOut, cbOut, &iv[0],
                              static_cast<uint32_t>(iv.size()));
  }

  return cbOut;
}

uint32_t Cbc4kCryptoProvider::GetPaddedSize(uint32_t cbSizeUnsafe)
{
  auto cbSize = cbSizeUnsafe;

  return ((cbSize / AES128_BLOCK_SIZE) + 1) * AES128_BLOCK_SIZE;
}

vector<uint8_t>Cbc4kCryptoProvider::GenerateIvForBlock(uint32_t dwBlockNumber)
{
  vector<uint8_t> ivInput(AES128_KEY_BYTE_LENGTH, 0);

  // Set the first 4 bytes to the block number
  memcpy(&ivInput[0], &dwBlockNumber, sizeof(dwBlockNumber));

  vector<uint8_t> iv(AES128_KEY_BYTE_LENGTH);

  uint32_t cbIv = AES128_KEY_BYTE_LENGTH;
  m_pEcbKey->Encrypt(&ivInput[0],
                     static_cast<uint32_t>(ivInput.size()), &iv[0], cbIv, nullptr,
                     0);

  return iv;
}
} // namespace crypto
} // namespace rmscrypto
