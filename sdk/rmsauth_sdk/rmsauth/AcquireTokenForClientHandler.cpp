/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "AcquireTokenForClientHandler.h"

namespace rmsauth {

AcquireTokenForClientHandler::AcquireTokenForClientHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String &resource, ClientKeyPtr clientKey, bool callSync)
    : AcquireTokenHandlerBase(authenticator, tokenCache, resource, clientKey, TokenSubjectType::Client, callSync)
{
    supportADFS_ = true;
}

void AcquireTokenForClientHandler::addAditionalRequestParameters(RequestParameters& requestParameters)
{
    requestParameters.addParam(OAuthConstants::oAuthParameter().GrantType, OAuthConstants::oAuthGrantType().ClientCredentials);
}

} // namespace rmsauth {
