/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <TokenCache.h>

namespace rmsauth {

TokenCache::TokenCache()
{
}

TokenCache::TokenCache(const ByteArray& state)
{
    deserialize(state);
}


TokenCache& TokenCache::defaultShared()
{
    static TokenCache tokenCache;
    return tokenCache;
}

int TokenCache::count() const
{
    return (int)tokenCacheDictionary_.size();
}


List<TokenCacheItemPtr> TokenCache::readItems()
{
    TokenCacheNotificationArgs args{this};
    onBeforeAccess(args);

    List<TokenCacheItemPtr> items;
    for (auto kvp : tokenCacheDictionary_)
    {
        items.push_back(std::make_shared<TokenCacheItem>(kvp.first, kvp.second));
    }

    onAfterAccess(args);

    return move(items);
}

void TokenCache::deleteItem(TokenCacheItemPtr item)
{
    Logger::info(Tag(), "deleteItem");

    TokenCacheNotificationArgs args(this, item->resource(), item->clientId(), item->uniqueId()/*, item->displayableId()*/);

    onBeforeAccess(args);
    onBeforeWrite(args);

    tokenCacheDictionary_.erase(item->tokenCacheKey());

    hasStateChanged_ = true;
    onAfterAccess(args);
}

void TokenCache::clear()
{
    Logger::info(Tag(), "clear");

    TokenCacheNotificationArgs args{ this };
    onBeforeAccess(args);
    onBeforeWrite(args);
    tokenCacheDictionary_.clear();
    hasStateChanged_ = true;
    onAfterAccess(args);
}

AuthenticationResultPtr TokenCache::loadFromCache(const String& authority, const String& resource, const String& clientId, TokenSubjectType subjectType, const String& uniqueId, /*const String& displayableId, */CallStatePtr callState)
{
    Logger::info(Tag(), "loadFromCache");

    AuthenticationResultPtr result = nullptr;

    auto item = loadSingleItemFromCache(authority, resource, clientId, subjectType, uniqueId, /*displayableId, */callState);

    if (item != nullptr)
    {
        TokenCacheKey& cacheKey = item->tokenCacheKey();
        result = item->authenticationResult();

        DateTime nowUtc;
        nowUtc.addSecs(expirationMarginInSeconds_);

        Logger::info(Tag(), "Local time UTC: '%', token expiresOn: % (%)", DateTime().toString("HH:mm:ss MM.dd.yy"), DateTime(result->expiresOn()).toString("HH:mm:ss MM.dd.yy"), result->expiresOn() );

        bool tokenNearExpiry = (result->expiresOn() <= nowUtc.toTime());

        if (tokenNearExpiry || !StringUtils::equalsIC(cacheKey.resource(), resource) )
        {
            result->accessToken("");
            if (tokenNearExpiry)
            {
                Logger::info(Tag(), "An expired or near expiry token was found in the cache");
            }
        }

        if (result->accessToken().empty() && result->refreshToken().empty())
        {
            tokenCacheDictionary_.erase(cacheKey);
            Logger::info(Tag(), "An old item was removed from the cache");
            hasStateChanged_ = true;
            result = nullptr;
        }

        if (result != nullptr)
        {
            Logger::info(Tag(), "A matching token was found in the cache");
        }
    }
    else
    {
         Logger::info(Tag(), "No matching token was found in the cache");
    }

    return result;
}

void TokenCache::storeToCache(AuthenticationResultPtr result, const String& authority, const String& resource, const String& clientId, TokenSubjectType subjectType, CallStatePtr/* callState*/)
{
    Logger::info(Tag(), "storeToCache");

    String uniqueId = (result->userInfo() != nullptr) ? result->userInfo()->uniqueId() : "";
    String displayableId = (result->userInfo() != nullptr) ? result->userInfo()->displayableId() : "";

    TokenCacheNotificationArgs args(this, clientId, resource,  uniqueId/*, displayableId*/);
    onBeforeWrite(args);

    TokenCacheKey tokenCacheKey(authority, resource, clientId, subjectType, result->userInfo());
    auto it = tokenCacheDictionary_.find(tokenCacheKey);
    if(it != tokenCacheDictionary_.end())
    {
        it->second = result;
        Logger::info(Tag(), "An item was updated in the cache");
    }
    else
    {
        Logger::info(Tag(), "An item was added to the cache");
        tokenCacheDictionary_.insert(KeyValuePair<TokenCacheKey, AuthenticationResultPtr>(std::move(tokenCacheKey), result));
    }

    updateCachedMrrtRefreshTokens(result, authority, clientId, subjectType);

    hasStateChanged_ = true;
}

void TokenCache::updateCachedMrrtRefreshTokens(AuthenticationResultPtr result, const String& authority, const String& clientId, TokenSubjectType subjectType)
{
    if (result->userInfo() != nullptr && result->isMultipleResourceRefreshToken())
    {
        auto list = queryCache(authority, clientId, subjectType, result->userInfo()->uniqueId()/*, result->userInfo()->displayableId()*/);
        for(TokenCacheItemPtr itemPtr : list)
        {
            if(itemPtr->isMultipleResourceRefreshToken())
            {
                itemPtr->refreshToken(result->refreshToken());
            }
        }
    }
}

TokenCacheItemPtr TokenCache::loadSingleItemFromCache(const String& authority, const String& resource, const String& clientId, TokenSubjectType subjectType, const String& uniqueId, /*const String& displayableId, */CallStatePtr/* callState*/)
{
    auto list = queryCache(authority, clientId, subjectType, uniqueId/*, displayableId*/);
    auto qnty = std::count_if(
        std::begin(list), std::end(list),
        [&resource](TokenCacheItemPtr itemPtr)
        {
            return StringUtils::equalsIC(itemPtr->resource(), resource);
        });

    if (qnty > 1)
    {
        RmsauthException(Constants::rmsauthError().MultipleTokensMatched);
    }

    if(qnty == 1)
    {
        Logger::info(Tag(), "An item matching the requested resource was found in the cache");
        return list.front();
    }

    if(qnty == 0)
    {
        auto mrrt = std::find_if(
            std::begin(list), std::end(list),
            [&](TokenCacheItemPtr itemPtr)
            {
                return itemPtr->isMultipleResourceRefreshToken();
            });
        if(mrrt != std::end(list))
        {
            return *mrrt;
        }
    }

    return nullptr;
}

List<TokenCacheItemPtr> TokenCache::queryCache(const String& authority, const String& clientId, TokenSubjectType subjectType, const String& uniqueId/*, const String& displayableId*/)
{
    List<TokenCacheItemPtr> list;
    for (auto kvp : tokenCacheDictionary_)
    {
        TokenCacheKey key = kvp.first;
        if (StringUtils::equalsIC(key.authority(), authority)
                && (clientId.empty() || StringUtils::equalsIC(key.clientId(), clientId))
                && (uniqueId.empty() || StringUtils::equalsIC(key.uniqueId(), uniqueId))
//                && (displayableId.empty() || StringUtils::equalsIC(key.displayableId(), displayableId))
                && (key.tokenSubjectType() == subjectType))
        {
            list.push_back(std::make_shared<TokenCacheItem>(kvp.first, kvp.second));
        }
    }

    return std::move(list);
}

} // namespace rmsauth {
