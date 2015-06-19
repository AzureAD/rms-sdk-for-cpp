/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "KeyStorageWindows.h"

using namespace std;
namespace rmscrypto {
namespace platform {
namespace keystorage {
void KeyStorageWindows::RemoveKey(const string& /*csKeyWrapper*/) {
}

void KeyStorageWindows::StoreKey(const string& /*csKeyWrapper*/, const string& /*csKey*/) {
}

shared_ptr<string>KeyStorageWindows::LookupKey(const string& /*csKeyWrapper*/) {
  shared_ptr<string> res = nullptr;
  return res;
}

} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
