/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef OAUTHCONSTANTS_H
#define OAUTHCONSTANTS_H

#include "types.h"

namespace rmsauth {

struct OAuthConstants
{
    struct OAuthParameter {
        const String ResponseType = "response_type";
        const String GrantType = "grant_type";
        const String ClientId = "client_id";
        const String ClientSecret = "client_secret";
        const String ClientAssertion = "client_assertion";
        const String ClientAssertionType = "client_assertion_type";
        const String RefreshToken = "refresh_token";
        const String RedirectUri = "redirect_uri";
        const String Resource = "resource";
        const String Code = "code";
        const String Scope = "scope";
        const String Assertion = "assertion";
        const String RequestedTokenUse = "requested_token_use";
        const String Username = "username";
        const String Password = "password";
        const String FormsAuth = "amr_values";
        const String LoginHint = "login_hint"; // login_hint is not standard oauth2 parameter
        const String CorrelationId = "client-request-id"; // correlation id is not standard oauth2 parameter
        const String Prompt = "prompt"; // prompt is not standard oauth2 parameter
    };

    static const OAuthParameter& oAuthParameter()
    {
        static const OAuthParameter oAuthParameter{};
        return oAuthParameter;
    }

    struct OAuthGrantType {
        const String AuthorizationCode = "authorization_code";
        const String RefreshToken = "refresh_token";
        const String ClientCredentials = "client_credentials";
        const String Saml11Bearer = "urn:ietf:params:oauth:grant-type:saml1_1-bearer";
        const String Saml20Bearer = "urn:ietf:params:oauth:grant-type:saml2-bearer";
        const String JwtBearer = "urn:ietf:params:oauth:grant-type:jwt-bearer";
        const String Password = "password";
    };

    static const OAuthGrantType& oAuthGrantType()
    {
        static const OAuthGrantType oAuthGrantType{};
        return oAuthGrantType;
    }

    struct OAuthResponseType
    {
        const String Code = "code";

    };

    static const OAuthResponseType& oAuthResponseType()
    {
        static const OAuthResponseType oAuthResponseType{};
        return oAuthResponseType;
    }

    struct OAuthReservedClaim {
        const String Code = "code";
        const String TokenType = "token_type";
        const String AccessToken = "access_token";
        const String RefreshToken = "refresh_token";
        const String Resource = "resource";
        const String IdToken = "id_token";
        const String CreatedOn = "created_on";
        const String ExpiresOn = "expires_on";
        const String ExpiresIn = "expires_in";
        const String Error = "error";
        const String ErrorDescription = "error_description";
        const String ErrorCodes = "error_codes";
    };

    static const OAuthReservedClaim& oAuthReservedClaim()
    {
        static const OAuthReservedClaim oAuthReservedClaim{};
        return oAuthReservedClaim;
    }


    struct IdTokenClaim {
        const String ObjectId = "oid";
        const String Subject = "sub";
        const String TenantId = "tid";
        const String UPN = "upn";
        const String Email = "email";
        const String GivenName = "given_name";
        const String FamilyName = "family_name";
        const String IdentityProvider = "idp";
        const String Issuer = "iss";
        const String PasswordExpiration = "pwd_exp";
        const String PasswordChangeUrl = "pwd_url";
    };

    static const IdTokenClaim& idTokenClaim()
    {
        static const IdTokenClaim idTokenClaim{};
        return idTokenClaim;
    }


    struct OAuthAssertionType {
        const String JwtBearer = "urn:ietf:params:oauth:client-assertion-type:jwt-bearer";
    };

    static const OAuthAssertionType& oAuthAssertionType()
    {
        static const OAuthAssertionType oAuthAssertionType{};
        return oAuthAssertionType;
    }


    struct OAuthRequestedTokenUse {
        const String OnBehalfOf = "on_behalf_of";
    };

    static const OAuthRequestedTokenUse& oAuthRequestedTokenUse()
    {
        static const OAuthRequestedTokenUse oAuthRequestedTokenUse{};
        return oAuthRequestedTokenUse;
    }


    struct OAuthHeader {
        const String CorrelationId = "client-request-id";
        const String RequestCorrelationIdInResponse = "return-client-request-id";
    };

    static const OAuthHeader& oAuthHeader()
    {
        static const OAuthHeader oAuthHeader{};
        return oAuthHeader;
    }


    struct OAuthError {
        const String LoginRequired = "login_required";
        const String InvalidGrant = "invalid_grant";
    } ;

    static const OAuthError& oAuthError()
    {
        static const OAuthError oAuthError{};
        return oAuthError;
    }


    struct OAuthValue {
        const String FormsAuth = "pwd";
        const String ScopeOpenId = "openid";
    };

    static const OAuthValue& oAuthValue()
    {
        static const OAuthValue oAuthValue{};
        return oAuthValue;
    }


    struct PromptValue {
        const String Login = "login";
        const String RefreshSession = "refresh_session";

        // The behavior of this value is identical to prompt=none for managed users; However, for federated users, AAD
        // redirects to ADFS as it cannot determine in advance whether ADFS can login user silently (e.g. via WIA) or not.
        const String AttemptNone = "attempt_none";
    };

    static const PromptValue& promptValue()
    {
        static const PromptValue promptValue{};
        return promptValue;
    }
};

} // namespace rmsauth {

#endif // OAUTHCONSTANTS_H
