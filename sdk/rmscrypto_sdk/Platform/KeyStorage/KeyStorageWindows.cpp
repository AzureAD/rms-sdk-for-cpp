/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "KeyStorageWindows.h"
#include "CryptoAPI/RMSCryptoExceptions.h"
#include "StorageAccessWindows.h"

using namespace std;
namespace rmscrypto {
namespace platform {
namespace keystorage {
void KeyStorageWindows::RemoveKey(const string& csKeyWrapper) {
    StorageAccessWindows::Instance().RemoveKey(csKeyWrapper);
}

void KeyStorageWindows::StoreKey(const string& csKeyWrapper,
                                 const string& csKey) {
    StorageAccessWindows::Instance().StoreKey(csKeyWrapper, csKey);
}

shared_ptr<string>KeyStorageWindows::LookupKey(const string& csKeyWrapper) {
    return StorageAccessWindows::Instance().LookupKey(csKeyWrapper);
}

std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
    return std::shared_ptr<KeyStorageWindows>(new KeyStorageWindows);
}

} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
