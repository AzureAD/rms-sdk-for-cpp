/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "KeyStorageWindows.h"
#include "../../CryptoAPI/RMSCryptoExceptions.h"

using namespace std;
namespace rmscrypto {
namespace platform {
namespace keystorage {
void KeyStorageWindows::RemoveKey(const string& /*csKeyWrapper*/) {
  throw exceptions::RMSCryptoNotImplementedException(
          "Key storage for Windows is not implemented");
}

void KeyStorageWindows::StoreKey(const string& /*csKeyWrapper*/,
                                 const string& /*csKey*/) {
  throw exceptions::RMSCryptoNotImplementedException(
          "Key storage for Windows is not implemented");
}

shared_ptr<string>KeyStorageWindows::LookupKey(const string& /*csKeyWrapper*/) {
  throw exceptions::RMSCryptoNotImplementedException(
          "Key storage for Windows is not implemented");
}

std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
  throw exceptions::RMSCryptoNotImplementedException(
          "KeyStorage is not implemented on this platform");
}

} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
