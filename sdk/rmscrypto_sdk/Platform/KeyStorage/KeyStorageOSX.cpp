/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "KeyStorageOSX.h"

using namespace std;
namespace rmscrypto {
namespace platform {
namespace keystorage {
void KeyStorageOSX::RemoveKey(const string& /*csKeyWrapper*/) {
}

void KeyStorageOSX::StoreKey(const string& /*csKeyWrapper*/, const string& /*csKey*/) {
}

shared_ptr<string> KeyStorageOSX::LookupKey(const string& /*csKeyWrapper*/) {
  shared_ptr<string> res = nullptr;
  return res;
}

} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
