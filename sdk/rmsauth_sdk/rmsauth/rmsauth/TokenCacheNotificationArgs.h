/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef TOKENCACHENOTIFICATIONARGS_H
#define TOKENCACHENOTIFICATIONARGS_H

#include "types.h"

namespace rmsauth {

class TokenCache;

class TokenCacheNotificationArgs final
{
    TokenCache* tokenCache_;
    String clientId_;
    String resource_;
    String uniqueId_;
    //String displayableId_;

public:
    TokenCacheNotificationArgs(TokenCache* tokenCache)
        : tokenCache_{tokenCache}
    {}

    TokenCacheNotificationArgs(TokenCache* tokenCache, const String& clientId, const String& resource, const String& uniqueId/*, const String& displayableId*/)
        : tokenCache_(tokenCache)
        , clientId_(clientId)
        , resource_(resource)
        , uniqueId_(uniqueId)
//        , displayableId_(displayableId)
    {}

    TokenCache* tokenCache() const { return tokenCache_; }
    const String& clientId() const { return clientId_; }
    const String& resource() const { return resource_; }
    const String& uniqueId() const { return uniqueId_; }
//    const String& displayableId() const { return displayableId_; }
};

} //namespace rmsauth {

#endif // TOKENCACHENOTIFICATIONARGS_H
