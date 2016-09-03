/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_CRYPTOPROVIDER_H_
#define _CRYPTO_STREAMS_LIB_CRYPTOPROVIDER_H_
#include <stdint.h>
#include <memory>
#include <vector>

#include "CryptoAPIExport.h"

namespace rmscrypto {
namespace api {
enum CipherMode
{
  CIPHER_MODE_CBC4K,
  CIPHER_MODE_ECB,
  CIPHER_MODE_CBC512NOPADDING
};

static const char* EnumStrings[] = { "MICROSOFT.CBC4K", "MICROSOFT.ECB", "MICROSOFT.CBC512.NOPADDING" };

class ICryptoProvider {
public:
  const static char* CipherModeString(CipherMode cm) { return EnumStrings[cm]; }
  static CipherMode StringToCipherMode(std::string s) { return s == EnumStrings[0] ? CIPHER_MODE_CBC4K : s == EnumStrings[1] ? CIPHER_MODE_ECB : s == EnumStrings[2] ? CIPHER_MODE_CBC512NOPADDING : CIPHER_MODE_CBC4K; }

  virtual void Encrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint32_t       dwStartingBlockNumber,
                       bool           isFinal,
                       uint8_t       *pbOut,
                       uint32_t       cbOut,
                       uint32_t      *pcbOut) = 0;
  virtual void Decrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint32_t       dwStartingBlockNumber,
                       bool           isFinal,
                       uint8_t       *pbOut,
                       uint32_t       cbOut,
                       uint32_t      *pcbOut) = 0;


  virtual uint64_t            GetCipherTextSize(uint64_t clearTextSize)
    = 0;

  virtual uint32_t            GetBlockSize() = 0;
  virtual std::vector<uint8_t>GetKey() = 0;
private:


};
} // namespace api
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_CRYPTOPROVIDER_H_
