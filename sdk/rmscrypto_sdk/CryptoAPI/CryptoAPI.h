/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_CRYPTO_API_H_
#define _RMS_CRYPTO_API_H_

#include <memory>
#include <vector>
#include "CryptoAPIExport.h"
#include "IStream.h"
#include "ICryptoProvider.h"
#include "ICryptoEngine.h"

namespace rmscrypto {
namespace api {
// Stream factory
SharedStream DLL_PUBLIC_CRYPTO CreateCryptoStream(
  CipherMode                  cipherMode,
  const std::vector<uint8_t>& key,
  SharedStream                backingStream);

// A random new key for current user will be generated at first time using
// keyInitializationData
// To reuse the same key you MUST put the same keyInitializationData as the
// first time
SharedStream DLL_PUBLIC_CRYPTO CreateCryptoStreamWithAutoKey(
  CipherMode         cipherMode,
  const std::string& csKeyName,
  SharedStream       backingStream);

std::shared_ptr<std::vector<uint8_t> >DLL_PUBLIC_CRYPTO EncryptWithAutoKey(
  std::shared_ptr<std::vector<uint8_t> >pbIn,
  CipherMode                            cipherMode = CIPHER_MODE_CBC4K,
  const std::string                   & csKeyName = "default");

std::shared_ptr<std::vector<uint8_t> >DLL_PUBLIC_CRYPTO DecryptWithAutoKey(
  std::shared_ptr<std::vector<uint8_t> >pbIn,
  CipherMode                            cipherMode = CIPHER_MODE_CBC4K,
  const std::string                   & csKeyName = "default");

SharedStream DLL_PUBLIC_CRYPTO CreateStreamFromStdStream(
  std::shared_ptr<std::istream>stdIStream);
SharedStream DLL_PUBLIC_CRYPTO CreateStreamFromStdStream(
  std::shared_ptr<std::ostream>stdOStream);
SharedStream DLL_PUBLIC_CRYPTO CreateStreamFromStdStream(
  std::shared_ptr<std::iostream>stdIOStream);

// create crypto primitives directly
std::shared_ptr<ICryptoProvider>DLL_PUBLIC_CRYPTO CreateCryptoProvider(
  CipherMode                  cipherMode,
  const std::vector<uint8_t>& key);
std::shared_ptr<ICryptoEngine>DLL_PUBLIC_CRYPTO   CreateCryptoEngine();
} // namespace api
} // namespace rmscrypto
#endif // _RMS_CRYPTO_API_H_
