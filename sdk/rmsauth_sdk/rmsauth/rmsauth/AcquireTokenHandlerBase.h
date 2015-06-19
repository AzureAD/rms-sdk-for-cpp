/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ACQUIRETOKENHANDLERBASE_H
#define ACQUIRETOKENHANDLERBASE_H

#include "types.h"
#include "Authenticator.h"
#include "ClientKey.h"
#include "TokenCache.h"
#include "TokenSubjectType.h"
#include "Exceptions.h"
#include "AuthenticationResult.h"
#include "RequestParameters.h"
#include "Entities.h"
#include "OAuthConstants.h"
#include "Constants.h"
#include "TokenSubjectType.h"
#include "HttpHelper.h"
#include "UserIdentifier.h"

namespace rmsauth {

class AcquireTokenHandlerBase
{
    static const String& Tag() {static const String tag="AcquireTokenHandlerBase"; return tag;}

protected:
    AuthenticatorPtr authenticator_ = nullptr;
    TokenCachePtr tokenCache_ = nullptr;
    String resource_;
    ClientKeyPtr clientKey_ = nullptr;
    TokenSubjectType tokenSubjectType_;
    bool loadFromCache_;
    bool storeToCache_;
    bool supportADFS_;
    CallStatePtr callState_ = nullptr;

    UserIdentifierType userIdentifierType_;

    String uniqueId_;
    String displayableId_;

public:
    AuthenticationResultPtr runAsync();
    static CallStatePtr createCallState(const Guid& correlationId, bool callSync);

protected:
    AcquireTokenHandlerBase(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, ClientKeyPtr clientKey, TokenSubjectType subjectType, bool callSync);
    virtual void preRunAsync();
    virtual void postRunAsync(AuthenticationResultPtr result);
    virtual void preTokenRequest();
    virtual void postTokenRequest(AuthenticationResultPtr result);
    virtual void addAditionalRequestParameters(RequestParameters& requestParameters) = 0;
    virtual AuthenticationResultPtr sendTokenRequestAsync();
    AuthenticationResultPtr sendTokenRequestByRefreshTokenAsync(const String& refreshToken);

private:
    AuthenticationResultPtr refreshAccessTokenAsync(AuthenticationResultPtr result);
    AuthenticationResultPtr sendHttpMessageAsync(const RequestParameters& requestParameters);
    void notifyBeforeAccessCache();
    void notifyAfterAccessCache();
    void logReturnedToken(AuthenticationResultPtr result);
    void validateAuthorityType();
};

} // namespace rmsauth {

#endif // ACQUIRETOKENHANDLERBASE_H
