/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef AUTHENTICATIONRESULT_H
#define AUTHENTICATIONRESULT_H

#include "types.h"
#include "UserInfo.h"
#include "rmsauthExport.h"

namespace rmsauth {

class AuthenticationResult;
using AuthenticationResultPtr = ptr<AuthenticationResult>;

class RMSAUTH_EXPORT AuthenticationResult
{
    static const String& Tag() {static const String tag="AuthenticationResult"; return tag;}

    const String oAuth2AuthorizationHeader_ = "Bearer ";

    String accessTokenType_;
    String accessToken_;
    String resource_;
    String refreshToken_;
    DateTimeOffset expiresOn_;
    String tenantId_;
    UserInfoPtr userInfo_;
    String idToken_;
    bool isMultipleResourceRefreshToken_;

public:
    AuthenticationResult(const String& accessTokenType, const String& accessToken, const String& refreshToken, DateTimeOffset expiresOn);

    const String& accessTokenType() const { return accessTokenType_; }

    const String& accessToken() const { return accessToken_; }
    void accessToken(const String& val){ accessToken_ = val; }

    const String& resource() const { return resource_; }
    void resource(const String& val){ resource_ = val; }

    const String& refreshToken() const { return refreshToken_; }
    void refreshToken(const String& val){ refreshToken_ = val; }

    DateTimeOffset expiresOn() const { return expiresOn_; }

    const String& tenantId() const { return tenantId_; }

    const UserInfoPtr userInfo() const { return userInfo_; }

    const String& idToken() const { return idToken_; }

    bool isMultipleResourceRefreshToken() const { return isMultipleResourceRefreshToken_; }
    void isMultipleResourceRefreshToken(const bool val) { isMultipleResourceRefreshToken_ = val; }


    String createAuthorizationHeader();
    static AuthenticationResultPtr deserialize(const String& /*serializedObject*/);
    String serialize();

    void updateTenantAndUserInfo(const String& tenantId, const String& idToken, UserInfoPtr userInfo);

private:
    friend class OAuth2Response;
    AuthenticationResult() = default;

private:
    static struct _JsonNames {
       const String accessTokenType = "accessTokenType";
       const String accessToken = "accessToken";
       const String resource = "resource";
       const String refreshToken = "refreshToken";
       const String expiresOn = "expiresOn";
       const String tenantId = "tenantId_";
       const String userInfo = "userInfo";
       const String idToken = "idToken";
       const String isMultipleResourceRefreshToken = "isMultipleResourceRefreshToken";
    } JsonNames;

};

} // namespace rmsauth {

#endif // AUTHENTICATIONRESULT_H
