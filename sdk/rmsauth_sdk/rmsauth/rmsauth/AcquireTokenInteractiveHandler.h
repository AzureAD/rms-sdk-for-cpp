/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ACQUIRETOKENINTERACTIVEHANDLER_H
#define ACQUIRETOKENINTERACTIVEHANDLER_H

#include "AcquireTokenHandlerBase.h"
#include "PromptBehavior.h"
#include "IWebUI.h"
#include "UserIdentifier.h"
#include "Exceptions.h"
#include "Constants.h"
#include "RmsauthIdHelper.h"
#include "AuthorizationResult.h"
#include "Url.h"

namespace rmsauth {

class AcquireTokenInteractiveHandler : public AcquireTokenHandlerBase
{
    static const String& Tag() {static const String tag="AcquireTokenInteractiveHandler"; return tag;}

    AuthorizationResultPtr authorizationResult_ = nullptr;
    String redirectUri_;
    String redirectUriRequestParameter_;
    PromptBehavior promptBehavior_;
    String extraQueryParameters_;
    IWebUIPtr webUi_ = nullptr;
    UserIdentifierPtr userId_ = nullptr;

public:
    AcquireTokenInteractiveHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, const String& clientId, const String& redirectUri, PromptBehavior promptBehavior, UserIdentifierPtr userId, const String& extraQueryParameters, IWebUIPtr webUI, bool callSync);

protected:
    void addAditionalRequestParameters(RequestParameters& requestParameters) override;
    void preTokenRequest() override;
    void postTokenRequest(AuthenticationResultPtr result) override;
    void acquireAuthorization();
    void sendAuthorizeRequest();
    static bool includeFormsAuthParams();
    String createAuthorizationUriAsync(const Guid& correlationId);

private:
    String createAuthorizationUri(bool includeFormsAuthParam);
    RequestParameters createAuthorizationRequest(const String& loginHint, bool includeFormsAuthParam);
    void verifyAuthorizationResult();
    static bool isDomainJoined();
    static bool isUserLocal();
    void setRedirectUriRequestParameter();
    static void addHeadersToRequestParameters(RequestParameters& requestParameters, Headers headers);
};

} // namespace rmsauth {

#endif // ACQUIRETOKENINTERACTIVEHANDLER_H
