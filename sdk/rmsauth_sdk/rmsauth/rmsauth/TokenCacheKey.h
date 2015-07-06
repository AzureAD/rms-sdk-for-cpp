/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef TOKENCACHEKEY_H
#define TOKENCACHEKEY_H

#include <functional>
#include "types.h"
#include "TokenSubjectType.h"
#include "utils.h"
#include "UserInfo.h"
#include "Logger.h"

namespace rmsauth {

class TokenCacheKey;
using TokenCacheKeyPtr = ptr<TokenCacheKey>;

class TokenCacheKey
{
    static const String& Tag() {static const String tag="TokenCacheKey"; return tag;}

    String authority_;
    String resource_;
    String clientId_;
    String uniqueId_;
    //String displayableId_;
    TokenSubjectType tokenSubjectType_;

public:
    TokenCacheKey() = default;
    TokenCacheKey(const String& authority, const String& resource, const String& clientId, const TokenSubjectType tokenSubjectType, const UserInfoPtr userInfo);
    TokenCacheKey(const String& authority, const String& resource, const String& clientId, const TokenSubjectType tokenSubjectType, const String& uniqueId);

    const String& authority()             const { return authority_; }
    const String& resource()              const { return resource_; }
    const String& clientId()              const { return clientId_; }
    const String& uniqueId()              const { return uniqueId_; }
    //const String& displayableId()         const { return displayableId_; }
    TokenSubjectType tokenSubjectType()   const { return tokenSubjectType_; }

    bool equals(const TokenCacheKey& other) const;
    bool operator==(const TokenCacheKey &other) const;
    std::size_t getHashCode() const;
};

} // namespace rmsauth {

namespace std {

template <>
struct hash<rmsauth::TokenCacheKey>
{
    size_t operator()(const rmsauth::TokenCacheKey& k) const
    {
        return (k.getHashCode());
    }
};

}

#endif // TOKENCACHEKEY_H
