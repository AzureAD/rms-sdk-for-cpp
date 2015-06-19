/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_ICRYPTOKEY_
#define _CRYPTO_STREAMS_LIB_ICRYPTOKEY_
#include <memory>
#include <stdint.h>
namespace rmscrypto {
namespace api {
class ICryptoKey {
public:

  virtual void Encrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint8_t       *pbOut,
                       uint32_t     & cbOut,
                       const uint8_t *pbIv,
                       uint32_t       cbIv) = 0;
  virtual void Decrypt(const uint8_t *pbIn,
                       uint32_t       cbIn,
                       uint8_t       *pbOut,
                       uint32_t     & cbOut,
                       const uint8_t *pbIv,
                       uint32_t       cbIv) = 0;
};
} // namespace api
} // namespace rmscrypto

#endif // _CRYPTO_STREAMS_LIB_ICRYPTOKEY_
