/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AuthenticationContext.h>

namespace rmsauth {

AuthenticationContext::AuthenticationContext(const String& authority, TokenCachePtr tokenCache)
    : AuthenticationContext{authority, AuthorityValidationType::NotProvided, tokenCache}
{
}

AuthenticationContext::AuthenticationContext(const String& authority, AuthorityValidationType validateAuthority, TokenCachePtr tokenCache)
    : tokenCache_(tokenCache)
{
   Logger::info(Tag(), "AuthenticationContext");
    // If authorityType is not provided (via first constructor), we validate by default (except for ASG and Office tenants).
    authenticator_ = std::make_shared<Authenticator>(authority, (validateAuthority != AuthorityValidationType::False));
}

AuthenticationResultPtr AuthenticationContext::acquireToken(const String& resource, const String& clientId, const String& redirectUri, PromptBehavior promptBehavior, const String &userId)
{
    Logger::info(Tag(), "acquireToken");
    String extraQueryParameters = "";
    bool callSync = true;
    UserIdentifierPtr uid = userId.empty() ? UserIdentifier::anyUser() : std::make_shared<UserIdentifier>(userId, UserIdentifierType::OptionalDisplayableId);
    return acquireTokenCommonAsync(resource, clientId, redirectUri, promptBehavior, uid, extraQueryParameters, callSync);
}

AuthenticationResultPtr AuthenticationContext::acquireToken(const String& resource, const String& clientId, UserCredentialPtr userCredentiar)
{
    Logger::info(Tag(), "acquireToken");
    bool callSync = true;
    return acquireTokenCommonAsync(resource, clientId, userCredentiar, callSync);
}

AuthenticationResultPtr AuthenticationContext::acquireToken(const String& resource, const String& clientId, UserAssertionPtr userAssertion)
{
    Logger::info(Tag(), "acquireToken");
    bool callSync = true;
    return acquireTokenCommonAsync(resource, clientId, userAssertion, callSync);
}

AuthenticationResultPtr AuthenticationContext::acquireToken(const String& resource, ClientCredentialPtr clientCredential)
{
    Logger::info(Tag(), "acquireToken");
    bool callSync = true;
    return acquireTokenCommonAsync(resource, clientCredential, callSync);
}

AuthenticationResultPtr AuthenticationContext::acquireTokenCommonAsync(const String& resource, const String& clientId, const String& redirectUri, PromptBehavior promptBehavior, UserIdentifierPtr userId, const String& extraQueryParameters, bool callSync/* = false*/)
{
    Logger::info(Tag(), "acquireTokenCommonAsync");
    AcquireTokenInteractiveHandler handler(authenticator_, tokenCache_, resource, clientId, redirectUri, promptBehavior, userId, extraQueryParameters, createWebAuthenticationDialog(promptBehavior), callSync);
    return handler.runAsync();
}

AuthenticationResultPtr AuthenticationContext::acquireTokenCommonAsync(const String& resource, const String& clientId, UserCredentialPtr userCredential, bool callSync/* = false*/)
{
    Logger::info(Tag(), "acquireTokenCommonAsync");
    AcquireTokenNonInteractiveHandler handler(authenticator_, tokenCache_, resource, clientId, userCredential, callSync);
    return handler.runAsync();
}

AuthenticationResultPtr AuthenticationContext::acquireTokenCommonAsync(const String& resource, const String& clientId, UserAssertionPtr userAssertionl, bool callSync/* = false*/)
{
    Logger::info(Tag(), "acquireTokenCommonAsync");
    AcquireTokenNonInteractiveHandler handler(authenticator_, tokenCache_, resource, clientId, userAssertionl, callSync);
    return handler.runAsync();
}

AuthenticationResultPtr AuthenticationContext::acquireTokenCommonAsync(const String& resource, ClientCredentialPtr clientCredential, bool callSync/* = false*/)
{
    Logger::info(Tag(), "acquireTokenCommonAsync");
    AcquireTokenForClientHandler handler(authenticator_, tokenCache_, resource,  std::make_shared<ClientKey>(clientCredential), callSync);
    return handler.runAsync();
}

IWebUIPtr AuthenticationContext::createWebAuthenticationDialog(PromptBehavior promptBehavior)
{
    Logger::info(Tag(), "createWebAuthenticationDialog");
    return std::make_shared<WebUI>(promptBehavior);
}

} // namespace rmsauth {
