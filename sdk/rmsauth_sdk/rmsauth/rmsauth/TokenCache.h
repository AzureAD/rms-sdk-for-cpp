/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef TOKENCACHE_H
#define TOKENCACHE_H

#include "types.h"
#include "AuthenticationResult.h"
#include "ClientKey.h"
#include "CallState.h"
#include "TokenCacheNotificationArgs.h"
#include "TokenCacheKey.h"
#include "TokenCacheItem.h"
#include "Logger.h"
#include "rmsauthExport.h"

namespace rmsauth {

class RMSAUTH_EXPORT TokenCache
{
    static const String Tag(){static const String tag = "TokenCache"; return tag;}

    const int SchemaVersion_ = 2;
    const String LocalSettingsContainerName_ = "ActiveDirectoryAuthenticationLibrary";
    HashMap<TokenCacheKey, AuthenticationResultPtr> tokenCacheDictionary_;
    // We do not want to return near expiry tokens, this is why we use this hard coded setting to refresh tokens which are close to expiration.
    const int64_t expirationMarginInSeconds_ = 300;

public:
    TokenCache(const ByteArray& state);
    static TokenCache& defaultShared();
    bool hasStateChanged() const { return hasStateChanged_; }
    void hasStateChanged(const bool val) { hasStateChanged_ = val; }

    int count() const;
    ByteArray serialize();
    void deserialize(const ByteArray& state);
    virtual List<TokenCacheItemPtr> readItems();
    virtual void deleteItem(TokenCacheItemPtr item);
    virtual void clear();

    virtual void onAfterAccess(const TokenCacheNotificationArgs&)   {}
    virtual void onBeforeAccess(const TokenCacheNotificationArgs&)  {}
    virtual void onBeforeWrite(const TokenCacheNotificationArgs&)   {}

    AuthenticationResultPtr loadFromCache(const String& authority, const String& resource, const String& clientId, TokenSubjectType subjectType, const String& uniqueId, CallStatePtr callState);
    void storeToCache(AuthenticationResultPtr result, const String& authority, const String& resource, const String& clientId, TokenSubjectType subjectType, CallStatePtr callState);
    void updateCachedMrrtRefreshTokens(AuthenticationResultPtr result, const String& authority, const String& clientId, TokenSubjectType subjectType);
    TokenCacheItemPtr loadSingleItemFromCache(const String& authority, const String& resource, const String& clientId, TokenSubjectType subjectType, const String& uniqueId, CallStatePtr callState);
    List<TokenCacheItemPtr> queryCache(const String& authority, const String& clientId, TokenSubjectType subjectType, const String& uniqueId);
    virtual const String getCacheName() const {return TokenCache::Tag();}

protected:
    TokenCache();
    volatile bool hasStateChanged_ = false;
};

using TokenCachePtr = ptr<TokenCache>;

} // namespace rmsauth {

#endif // TOKENCACHE_H
