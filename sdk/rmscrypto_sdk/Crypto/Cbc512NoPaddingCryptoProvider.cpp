/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <cstring>
#include "Cbc512NoPaddingCryptoProvider.h"
#include "CryptoConstants.h"
#include "../CryptoAPI/CryptoAPI.h"
#include "../CryptoAPI/RMSCryptoExceptions.h"

using namespace std;
using namespace rmscrypto::api;

namespace rmscrypto {
namespace crypto {
Cbc512NoPaddingCryptoProvider::Cbc512NoPaddingCryptoProvider(
  const vector<uint8_t>& key)
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
}

void Cbc512NoPaddingCryptoProvider::Encrypt(const uint8_t *pbIn,
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

  if (!isFinal && (0 != cbIn % CBC512_BLOCK_SIZE)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  if (nullptr == pcbOut) {
    throw exceptions::RMSCryptoNullPointerException("Null pointer pcbOut exception");
  }

  if ((cbIn % AES128_BLOCK_SIZE) != 0) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  auto cbResult = 0;

  if (nullptr == pbOut)
  {
    // No need to do the encryption, just return the number of uint8_ts required
    *pcbOut = (unsigned long)cbIn;
    return;
  }


  while (cbIn > CBC512_BLOCK_SIZE)
  {
    if (cbOut < CBC512_BLOCK_SIZE) {
      throw exceptions::RMSCryptoInvalidArgumentException("Invalid buffer size");
    }

    // Encrypt the current block
    EncryptBlock(pbIn,
                 CBC512_BLOCK_SIZE,
                 dwStartingBlockNumber,
                 false,
                 pbOut,
                 cbOut);

    // Go to the next block
    pbIn += CBC512_BLOCK_SIZE;
    cbIn -= CBC512_BLOCK_SIZE;

    pbOut += CBC512_BLOCK_SIZE;
    cbOut -= CBC512_BLOCK_SIZE;

    ++dwStartingBlockNumber;
    cbResult += CBC512_BLOCK_SIZE;
  }

  if (!isFinal && (cbIn > CBC512_BLOCK_SIZE)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid aligment");
  }
  cbResult += EncryptBlock(pbIn, cbIn, dwStartingBlockNumber, true, pbOut, cbOut);
  *pcbOut   = cbResult;
}

void Cbc512NoPaddingCryptoProvider::Decrypt(const uint8_t *pbIn,
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

  if (!isFinal && (0 != cbIn % CBC512_BLOCK_SIZE)) {
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

  if ((cbIn % AES128_BLOCK_SIZE) != 0) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  if (nullptr == pbOut)
  {
    // No need to do the decryption, just return the number of uint8_ts required
    *pcbOut = cbIn;
    return;
  }

  if (cbOut < cbIn) {
    throw exceptions::RMSCryptoInsufficientBufferException("Insufficient buffer");
  }

  auto cbResult = 0;

  while (cbIn >= CBC512_BLOCK_SIZE)
  {
    if (cbOut < CBC512_BLOCK_SIZE) {
      throw exceptions::RMSCryptoInsufficientBufferException("Insufficient buffer");
    }

    DecryptBlock(pbIn,
                 CBC512_BLOCK_SIZE,
                 dwStartingBlockNumber,
                 false,
                 pbOut,
                 cbOut);

    pbIn += CBC512_BLOCK_SIZE;
    cbIn -= CBC512_BLOCK_SIZE;

    pbOut += CBC512_BLOCK_SIZE;
    cbOut -= CBC512_BLOCK_SIZE;

    ++dwStartingBlockNumber;

    cbResult += CBC512_BLOCK_SIZE;
  }

  if (!isFinal && (cbIn != 0)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid aligment");
  }

  if (isFinal && (cbIn > 0))
  {
    // Note that the cbIn can't be 0, the final block always should have at
    // least AES128_BLOCK_SIZE (16) uint8_ts
    if (cbIn < AES128_BLOCK_SIZE) {
      throw exceptions::RMSCryptoInsufficientBufferException("Insufficient buffer");
    }
    cbResult +=
      DecryptBlock(pbIn, cbIn, dwStartingBlockNumber, true, pbOut, cbOut);
  }

  *pcbOut = cbResult;
}

uint64_t Cbc512NoPaddingCryptoProvider::GetCipherTextSize(uint64_t clearTextSize)
{
  if ((clearTextSize <= 0) || ((clearTextSize % AES128_BLOCK_SIZE) != 0)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid argument");
  }
  return clearTextSize;
}

uint32_t Cbc512NoPaddingCryptoProvider::EncryptBlock(
  const uint8_t *pbIn,
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

  if (!isFinalBlock && (CBC512_BLOCK_SIZE != cbIn)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid aligment");
  }

  // Generate the IV
  vector<uint8_t> iv = GenerateIvForBlock(dwBlockNumber);
  m_pCbcKey->Encrypt(pbIn, cbIn, pbOut, cbOut, &iv[0],
                     static_cast<uint32_t>(iv.size()));

  return cbOut;
}

uint32_t Cbc512NoPaddingCryptoProvider::DecryptBlock(
  const uint8_t *pbIn,
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

  if (!isFinalBlock && (CBC512_BLOCK_SIZE != cbIn)) {
    throw exceptions::RMSCryptoInvalidArgumentException("Block is not aligned");
  }

  // Generate the IV
  vector<uint8_t> iv = GenerateIvForBlock(dwBlockNumber);

  m_pCbcKey->Decrypt(pbIn, cbIn, pbOut, cbOut, &iv[0],
                     static_cast<uint32_t>(iv.size()));

  return cbOut;
}

uint32_t Cbc512NoPaddingCryptoProvider::GetPaddedSize(uint32_t cbSizeUnsafe)
{
  auto cbSize = cbSizeUnsafe;

  if (cbSize <= 0) return 0;

  return (((cbSize - 1) / AES128_BLOCK_SIZE) + 1) * AES128_BLOCK_SIZE;
}

vector<uint8_t>Cbc512NoPaddingCryptoProvider::GenerateIvForBlock(
  uint32_t dwBlockNumber)
{
  vector<uint8_t> ivInput(AES128_KEY_BYTE_LENGTH, 0);

  // Set the first 8 uint8_ts to the number of uint8_ts
  uint64_t cbuint8_tNumber = (uint64_t)dwBlockNumber * CBC512_BLOCK_SIZE;
  memcpy(&ivInput[0], &cbuint8_tNumber, sizeof(cbuint8_tNumber));

  vector<uint8_t> iv(AES128_KEY_BYTE_LENGTH);

  uint32_t cbIv = AES128_KEY_BYTE_LENGTH;
  m_pEcbKey->Encrypt(&ivInput[0],
                     static_cast<uint32_t>(ivInput.size()), &iv[0], cbIv, nullptr,
                     0);

  return iv;
}
} // namespace crypto
} // namespace rmscrypto
