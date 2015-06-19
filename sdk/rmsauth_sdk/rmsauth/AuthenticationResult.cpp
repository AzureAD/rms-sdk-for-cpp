/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AuthenticationResult.h>
#include <Logger.h>

namespace rmsauth {

AuthenticationResult::_JsonNames AuthenticationResult::JsonNames;

AuthenticationResult::AuthenticationResult(const String& accessTokenType, const String& accessToken, const String& refreshToken, DateTimeOffset expiresOn)
    : accessTokenType_(accessTokenType)
    , accessToken_(accessToken)
    , refreshToken_(refreshToken)
    , expiresOn_(expiresOn)
{
}

String AuthenticationResult::createAuthorizationHeader()
{
    return oAuth2AuthorizationHeader_ + accessToken_;
}

void AuthenticationResult::updateTenantAndUserInfo(const String& tenantId, const String& idToken, UserInfoPtr userInfo)
{
    Logger::info(Tag(), "updateTenantAndUserInfo");
    tenantId_ = tenantId;
    idToken_ = idToken;
    if (userInfo != nullptr)
    {
        userInfo_ = userInfo;
    }
}

} // namespace rmsauth {
