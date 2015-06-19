/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ACQUIRETOKENNONINTERACTIVEHANDLER_H
#define ACQUIRETOKENNONINTERACTIVEHANDLER_H

#include "AcquireTokenHandlerBase.h"
#include "UserCredential.h"
#include "UserAssertion.h"
#include "UserIdentifier.h"
#include "Exceptions.h"
#include "Constants.h"
#include "AuthorizationResult.h"

namespace rmsauth {

class AcquireTokenNonInteractiveHandler : public AcquireTokenHandlerBase
{
    static const String& Tag() {static const String tag="AcquireTokenNonInteractiveHandler"; return tag;}

    UserCredentialPtr userCredential_ = nullptr;
    UserAssertionPtr userAssertion_ = nullptr;

public:
    AcquireTokenNonInteractiveHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, const String& clientId, UserCredentialPtr userCredential, bool callSync);
    AcquireTokenNonInteractiveHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, const String& clientId, UserAssertionPtr userAssertion, bool callSync);

protected:
    void addAditionalRequestParameters(RequestParameters& requestParameters) override;
    void preRunAsync() override;
    void preTokenRequest() override;
};

} // namespace rmsauth {

#endif // ACQUIRETOKENNONINTERACTIVEHANDLER_H
