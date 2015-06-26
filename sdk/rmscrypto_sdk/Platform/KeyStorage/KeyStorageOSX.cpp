/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "../../CryptoAPI/RMSCryptoExceptions.h"
#include "KeyStorageOSX.h"

using namespace std;
namespace rmscrypto {
namespace platform {
namespace keystorage {
void KeyStorageOSX::RemoveKey(const string& /*csKeyWrapper*/) {
  throw exceptions::RMSCryptoNotImplementedException(
          "Key storage for OSX is not implemented");
}

void KeyStorageOSX::StoreKey(const string& /*csKeyWrapper*/,
                             const string& /*csKey*/) {
  throw exceptions::RMSCryptoNotImplementedException(
          "Key storage for OSX is not implemented");
}

shared_ptr<string>KeyStorageOSX::LookupKey(const string& /*csKeyWrapper*/) {
  throw exceptions::RMSCryptoNotImplementedException(
          "Key storage for OSX is not implemented");
}

std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
  throw exceptions::RMSCryptoNotImplementedException(
          "KeyStorage is not implemented on this platform");
}
} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
