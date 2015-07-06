/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef TOKENCACHEITEM_H
#define TOKENCACHEITEM_H

#include "types.h"
#include "AuthenticationResult.h"
#include "TokenCacheKey.h"

namespace rmsauth {

class TokenCacheItem
{
    TokenCacheKey tokenCacheKey_;
    AuthenticationResultPtr resultPtr_;

public:
    TokenCacheItem(const TokenCacheKey& key, AuthenticationResultPtr result);

    const TokenCacheKey& tokenCacheKey()                    const {return tokenCacheKey_;}
    TokenCacheKey& tokenCacheKey()                          { return tokenCacheKey_; }
    const AuthenticationResultPtr authenticationResult()    const {return resultPtr_; }
    AuthenticationResultPtr authenticationResult()          { return resultPtr_;}

    const String& authority()               const { return tokenCacheKey_.authority(); }
    const String& clientId()                const { return tokenCacheKey_.clientId(); }
    DateTimeOffset expiresOn()              const { return resultPtr_->expiresOn(); }
    const String familyName()               const { return resultPtr_->userInfo() != nullptr ? resultPtr_->userInfo()->familyName():""; }
    const String givenName()                const { return resultPtr_->userInfo() != nullptr ? resultPtr_->userInfo()->givenName():""; }
    const String identityProvider()         const { return resultPtr_->userInfo() != nullptr ? resultPtr_->userInfo()->identityProvider():""; }
    bool isMultipleResourceRefreshToken()   const { return resultPtr_->isMultipleResourceRefreshToken(); }
    const String& resource()                const { return tokenCacheKey_.resource(); }
    const String& tenantId()                const { return resultPtr_->tenantId(); }
    const String& uniqueId()                const { return tokenCacheKey_.uniqueId(); }
    //const String& displayableId()           const { return tokenCacheKey_.displayableId(); }
    const String& accessToken()             const { return resultPtr_->accessToken(); }
    const String& refreshToken()            const { return resultPtr_->refreshToken(); }
    void refreshToken(const String& val)    { resultPtr_->refreshToken(val); }
    const String& idToken()                 const { return resultPtr_->idToken(); }
    TokenSubjectType tokenSubjectType()     const { return tokenCacheKey_.tokenSubjectType(); }
};

using TokenCacheItemPtr = ptr<TokenCacheItem>;

} // namespace rmsauth {

#endif // TOKENCACHEITEM_H
