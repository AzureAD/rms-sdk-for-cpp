/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ENTITIES_H
#define ENTITIES_H

#include "types.h"
#include "OAuthConstants.h"

namespace rmsauth {

struct TokenResponse
{
    struct JsonNames {

        const String tokenType = OAuthConstants::oAuthReservedClaim().TokenType;
        const String accessToken = OAuthConstants::oAuthReservedClaim().AccessToken;
        const String refreshToken = OAuthConstants::oAuthReservedClaim().RefreshToken;
        const String resource = OAuthConstants::oAuthReservedClaim().Resource;
        const String idToken = OAuthConstants::oAuthReservedClaim().IdToken;
        const String createdOn = OAuthConstants::oAuthReservedClaim().CreatedOn;
        const String expiresOn = OAuthConstants::oAuthReservedClaim().ExpiresOn;
        const String expiresIn = OAuthConstants::oAuthReservedClaim().ExpiresIn;
        const String correlationId  = "correlation_id";
        const String error = OAuthConstants::oAuthReservedClaim().Error;
        const String errorDescription = OAuthConstants::oAuthReservedClaim().ErrorDescription;
        const String errorCodes  = OAuthConstants::oAuthReservedClaim().ErrorCodes;

    };

    static const JsonNames& jsonNames()
    {
        static const JsonNames jsonNames{};
        return jsonNames;
    }

    String tokenType;
    String accessToken;
    String refreshToken;
    String resource;
    String idToken;
    long createdOn;
    long expiresOn;
    long expiresIn;
    String correlationId;

    String error;
    String errorDescription;
    IntArray errorCodes;
};
using TokenResponsePtr = ptr<TokenResponse>;

struct IdToken
{
    struct JsonNames {
       const String objectId= OAuthConstants::idTokenClaim().ObjectId;
       const String subject = OAuthConstants::idTokenClaim().Subject;
       const String tenantId = OAuthConstants::idTokenClaim().TenantId;
       const String UPN = OAuthConstants::idTokenClaim().UPN;
       const String givenName = OAuthConstants::idTokenClaim().GivenName;
       const String familyName = OAuthConstants::idTokenClaim().FamilyName;
       const String email = OAuthConstants::idTokenClaim().Email;
       const String passwordExpiration = OAuthConstants::idTokenClaim().PasswordExpiration;
       const String passwordChangeUrl = OAuthConstants::idTokenClaim().PasswordChangeUrl;
       const String identityProvider = OAuthConstants::idTokenClaim().IdentityProvider;
       const String issuer = OAuthConstants::idTokenClaim().Issuer;
    };

    static const JsonNames& jsonNames()
    {
        static const JsonNames jsonNames{};
        return jsonNames;
    }


   String objectId;
   String subject;
   String tenantId;
   String UPN;
   String givenName;
   String familyName;
   String email;
   long passwordExpiration;
   String passwordChangeUrl;
   String identityProvider;
   String issuer;
};
using IdTokenPtr = ptr<IdToken>;

struct InstanceDiscoveryResponse
{
    struct JsonNames {
       const String tenantDiscoveryEndpoint= "tenant_discovery_endpoint";
    };

    static const JsonNames& jsonNames()
    {
        static const JsonNames jsonNames{};
        return jsonNames;
    }


    String tenantDiscoveryEndpoint;
};
using InstanceDiscoveryResponsePtr = ptr<InstanceDiscoveryResponse>;

struct ErrorResponse
{
    struct JsonNames {
        const String error = OAuthConstants::oAuthReservedClaim().Error;
        const String errorDescription = OAuthConstants::oAuthReservedClaim().ErrorDescription;
        const String errorCodes  = OAuthConstants::oAuthReservedClaim().ErrorCodes;
    };

    static const JsonNames& jsonNames()
    {
        static const JsonNames jsonNames{};
        return jsonNames;
    }

    String error;
    String errorDescription;
    IntArray errorCodes;
};

using ErrorResponsePtr = ptr<ErrorResponse>;

} // namespace rmsauth {

#endif // ENTITIES_H
