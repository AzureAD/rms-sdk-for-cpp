/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef AUTHENTICATIONCONTEXT_H
#define AUTHENTICATIONCONTEXT_H

#include "types.h"
#include "AuthenticationResult.h"
#include "Authenticator.h"
#include "PromptBehavior.h"
#include "UserIdentifier.h"
#include "TokenCache.h"
#include "Guid.h"
#include "AcquireTokenInteractiveHandler.h"
#include "AcquireTokenNonInteractiveHandler.h"
#include "AcquireTokenForClientHandler.h"
#include "rmsauthExport.h"

namespace rmsauth {

enum class AuthorityValidationType
{
    True,
    False,
    NotProvided
};


class RMSAUTH_EXPORT AuthenticationContext
{
    static const String& Tag() {static const String tag="AuthenticationContext"; return tag;}

    AuthenticatorPtr authenticator_;
    TokenCachePtr tokenCache_;

public:
    AuthenticationContext(const String& authority, TokenCachePtr tokenCache);
    AuthenticationContext(const String& authority, AuthorityValidationType validateAuthority, TokenCachePtr tokenCache);

    AuthenticationResultPtr acquireToken(const String& resource, const String& clientId, const String& redirectUri, PromptBehavior promptBehavior, const String& userId = "");
    AuthenticationResultPtr acquireToken(const String& resource, const String& clientId, UserCredentialPtr userCredentiar);
    AuthenticationResultPtr acquireToken(const String& resource, const String& clientId, UserAssertionPtr userAssertion);
    AuthenticationResultPtr acquireToken(const String& resource, ClientCredentialPtr clientCredential);

private:
    AuthenticationResultPtr acquireTokenCommonAsync(const String& resource, const String& clientId, const String& redirectUri, PromptBehavior promptBehavior, UserIdentifierPtr userId, const String& extraQueryParameters, bool callSync = false);
    AuthenticationResultPtr acquireTokenCommonAsync(const String& resource, const String& clientId, UserCredentialPtr userCredential, bool callSync = false);
    AuthenticationResultPtr acquireTokenCommonAsync(const String& resource, const String& clientId, UserAssertionPtr userAssertionl, bool callSync = false);
    AuthenticationResultPtr acquireTokenCommonAsync(const String& resource, ClientCredentialPtr clientCredential, bool callSync = false);

    static IWebUIPtr createWebAuthenticationDialog(PromptBehavior promptBehavior);
};

} // namespace rmsauth {

#endif // AUTHENTICATIONCONTEXT_H
