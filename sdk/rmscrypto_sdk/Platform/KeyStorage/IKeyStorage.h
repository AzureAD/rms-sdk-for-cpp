/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_IKEYSTORAGE_H
#define _CRYPTO_STREAMS_LIB_IKEYSTORAGE_H

#include <string>
#include <memory>

namespace rmscrypto {
namespace platform {
namespace keystorage {
class IKeyStorage {
public:

  virtual void                        RemoveKey(const std::string& csKeyWrapper) = 0;
  virtual void                        StoreKey(const std::string& csKeyWrapper,
                                               const std::string& csKey) = 0;
  virtual std::shared_ptr<std::string>LookupKey(const std::string& csKeyWrapper) = 0;

  static std::shared_ptr<IKeyStorage> Create();

  virtual ~IKeyStorage() {}
};
} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_IKEYSTORAGE_H
