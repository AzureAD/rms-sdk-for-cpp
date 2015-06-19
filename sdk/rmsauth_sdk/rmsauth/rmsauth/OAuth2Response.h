/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef OAUTH2RESPONSE_H
#define OAUTH2RESPONSE_H

#include "types.h"
#include "Entities.h"
#include "CallState.h"
#include "AuthenticationResult.h"
#include "AuthorizationResult.h"

namespace rmsauth {

class OAuth2Response
{
    static const String& Tag() {static const String tag="OAuth2Response"; return tag;}

public:
    static AuthenticationResultPtr parseTokenResponse(TokenResponsePtr tokenResponse, CallStatePtr callState);
    static AuthorizationResultPtr parseAuthorizeResponse(const String& webAuthenticationResult, CallStatePtr callState);

private:
    static IdTokenPtr parseIdToken(const String& idToken);
};

} // namespace rmsauth {

#endif // OAUTH2RESPONSE_H
