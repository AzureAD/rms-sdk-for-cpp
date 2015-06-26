/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <sstream>

#include "../Platform/KeyStorage/IKeyStorage.h"
#include "../Platform/KeyStorage/base64.h"
#include "../Platform/Crypto/CryptoEngine.h"
#include "../Crypto/CryptoConstants.h"
#include "../Crypto/Cbc4kCryptoProvider.h"
#include "../Crypto/Cbc512NoPaddingCryptoProvider.h"
#include "../Crypto/EcbCryptoProvider.h"

#include "CryptoAPI.h"
#include "BlockBasedProtectedStream.h"
#include "ICryptoStream.h"
#include "StdStreamAdapter.h"
#include "RMSCryptoExceptions.h"

using namespace std;
using namespace rmscrypto::crypto;
namespace rmscrypto {
namespace api {
SharedStream CreateCryptoStream(
  CipherMode             cipherMode,
  const vector<uint8_t>& key,
  SharedStream           backingStream)
{
  shared_ptr<ICryptoProvider> pCryptoProvider = nullptr;
  uint64_t nProtectedStreamBlockSize          = 4096;

  pCryptoProvider           = CreateCryptoProvider(cipherMode, key);
  nProtectedStreamBlockSize = pCryptoProvider->GetBlockSize() == 512 ? 512 : 4096;

  auto pProtectedStreamImpl = BlockBasedProtectedStream::Create(pCryptoProvider,
                                                                backingStream,
                                                                0,
                                                                -1,
                                                                nProtectedStreamBlockSize);

  return pProtectedStreamImpl;
}

SharedStream CreateCryptoStreamWithAutoKey(CipherMode    cipherMode,
                                           const string& csKeyName,
                                           SharedStream  backingStream)
{
  vector<uint8_t> key(16); // AES-128 crypto key
  auto ks = platform::keystorage::IKeyStorage::Create();

  // try to lookup key
  auto ret = ks->LookupKey(csKeyName);

  if ((ret.get() == nullptr) || ret->empty()) {
    // we should generate a new key
    if (RAND_bytes(key.data(), static_cast<int>(key.size())) != 0) {
      // convert to BASE64 encoding
      auto keyBase64 = platform::keystorage::base64_encode(
        key.data(), static_cast<unsigned int>(key.size()));

      // now store the new key
      ks->StoreKey(csKeyName, keyBase64);

      // reload the new key
      ret = ks->LookupKey(csKeyName);
    }
  }

  if ((ret.get() != nullptr) && !ret->empty()) {
    auto keyDec = platform::keystorage::base64_decode(*ret);
    key = vector<uint8_t>(keyDec.begin(), keyDec.end());
    return CreateCryptoStream(cipherMode, key, backingStream);
  }

  // fault
  return nullptr;
}

std::shared_ptr<std::vector<uint8_t> >EncryptWithAutoKey(
  std::shared_ptr<std::vector<uint8_t> >pbIn,
  CipherMode                            cipherMode,
  const std::string                   & csKeyName /*= "default"*/) {
  // create backing stream
  auto backingStdStream = make_shared<stringstream>(
    ios::in | ios::out | ios::binary);

  auto backingStream =
    CreateStreamFromStdStream(static_pointer_cast<iostream>(backingStdStream));

  auto cryptoStream = CreateCryptoStreamWithAutoKey(cipherMode,
                                                    csKeyName,
                                                    backingStream);

  // encrypt
  cryptoStream->Write(pbIn->data(), pbIn->size());
  cryptoStream->Flush();

  // return result
  auto resStr = backingStdStream->str();
  return make_shared<vector<uint8_t> >(resStr.begin(), resStr.end());
}

std::shared_ptr<std::vector<uint8_t> >DecryptWithAutoKey(
  std::shared_ptr<std::vector<uint8_t> >cbIn,
  CipherMode                            cipherMode,
  const std::string                   & csKeyName /*= "default"*/) {
  // create backing stream
  auto backingStdStream = make_shared<stringstream>(
    ios::in | ios::out | ios::binary);

  backingStdStream->write(reinterpret_cast<const char *>(cbIn->data()),
                          cbIn->size());

  backingStdStream->flush();

  auto backingStream =
    CreateStreamFromStdStream(static_pointer_cast<iostream>(backingStdStream));

  auto cryptoStream = CreateCryptoStreamWithAutoKey(cipherMode,
                                                    csKeyName,
                                                    backingStream);

  // encrypt
  auto resVec = cryptoStream->Read(cbIn->size());

  // return result
  return make_shared<vector<uint8_t> >(resVec.begin(), resVec.end());
}

SharedStream CreateStreamFromStdStream(
  std::shared_ptr<std::istream>stdIStream) {
  return static_pointer_cast<IStream>(make_shared<StdStreamAdapter>(stdIStream));
}

SharedStream CreateStreamFromStdStream(
  std::shared_ptr<std::ostream>stdOStream) {
  return static_pointer_cast<IStream>(make_shared<StdStreamAdapter>(stdOStream));
}

SharedStream CreateStreamFromStdStream(
  std::shared_ptr<std::iostream>stdIOStream)
{
  return static_pointer_cast<IStream>(make_shared<StdStreamAdapter>(stdIOStream));
}

std::shared_ptr<ICryptoProvider>CreateCryptoProvider(
  CipherMode                  cipherMode,
  const std::vector<uint8_t>& key)
{
  switch (cipherMode)
  {
  case CIPHER_MODE_CBC4K:
    return make_shared<Cbc4kCryptoProvider>(key);

  case CIPHER_MODE_ECB:
    return make_shared<EcbCryptoProvider>(key);

  case CIPHER_MODE_CBC512NOPADDING:
    return make_shared<Cbc512NoPaddingCryptoProvider>(key);

  default:
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid cipher mod");
  }
}

std::shared_ptr<ICryptoEngine>CreateCryptoEngine()
{
  return ICryptoEngine::Create();
}
} // namespace api
} // namespace rmscrypto
