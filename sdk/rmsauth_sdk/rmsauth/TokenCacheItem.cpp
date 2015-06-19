/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <TokenCacheItem.h>

namespace rmsauth {

TokenCacheItem::TokenCacheItem(const TokenCacheKey& tokenCacheKey, AuthenticationResultPtr result)
    : tokenCacheKey_(tokenCacheKey)
    , resultPtr_(result)
{
}

} // namespace rmsauth {
