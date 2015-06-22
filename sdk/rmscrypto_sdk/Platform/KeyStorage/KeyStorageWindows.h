/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_KEYSTORAGE_H
#define _CRYPTO_STREAMS_LIB_KEYSTORAGE_H

#include <string>
#include <memory>
#include "IKeyStorage.h"

namespace rmscrypto {
namespace platform {
namespace keystorage {

class KeyStorageWindows : public IKeyStorage
{
public:
    virtual void                        RemoveKey(const std::string& csKeyWrapper) override;
    virtual void                        StoreKey(const std::string& csKeyWrapper,
                                                 const std::string& csKey) override;
    virtual std::shared_ptr<std::string>LookupKey(const std::string& csKeyWrapper) override;

    KeyStorageWindows() {}
};

} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_KEYSTORAGE_H
