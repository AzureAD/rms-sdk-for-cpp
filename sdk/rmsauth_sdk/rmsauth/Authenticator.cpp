/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Authenticator.h>
#include <Exceptions.h>
#include <Logger.h>
#include <utils.h>

namespace rmsauth {

AuthenticatorTemplateList Authenticator::authenticatorTemplateList;

const String Authenticator::tenantlessTenantName()
{
    static const String str = "Common";
    return str;
}

Authenticator::Authenticator(const String& authority, bool validateAuthority)
    : validateAuthority_(validateAuthority)
    , default_(false)
{
    authority_ = canonicalizeUri(authority);
    authorityType_ = detectAuthorityType(authority_);
    if (authorityType_ != AuthorityType::AAD && validateAuthority)
    {
        Logger::error(Tag(), Constants::rmsauthErrorMessage().UnsupportedAuthorityValidation);
        throw IllegalArgumentException(Constants::rmsauthErrorMessage().UnsupportedAuthorityValidation, "validateAuthority");
    }
}

void Authenticator::updateFromTemplateAsync(CallStatePtr callState)
{
    Logger::info(Tag(), "updateFromTemplateAsync");

    if (!updatedFromTemplate_)
    {
        Url authorityUri(authority_);
        String host = authorityUri.authority();
        String path = authorityUri.path().substr(1);
        String tenant = path.substr(0, path.find("/"));
        auto matchingTemplate = Authenticator::authenticatorTemplateList.findMatchingItemAsync(validateAuthority_, host, tenant, callState);
        authorizationUri_ = StringUtils::replace(matchingTemplate->authorizeEndpoint(), "{tenant}", tenant);
        tokenUri_ = StringUtils::replace(matchingTemplate->tokenEndpoint(), "{tenant}", tenant);
        userRealmUri_ = canonicalizeUri(matchingTemplate->userRealmEndpoint());
        isTenantless_ = tenant.compare(tenantlessTenantName()) == 0;
        selfSignedJwtAudience_ = StringUtils::replace(matchingTemplate->issuer(), "{tenant}", tenant);
        updatedFromTemplate_ = true;
        default_ = false;
    }
}
void Authenticator::updateTenantId(const String& tenantId)
{
    Logger::info(Tag(), "updateTenantId");

    if (isTenantless_ && !tenantId.empty())
    {
        authority_ = replaceTenantlessTenant(authority_, tenantId);
        updatedFromTemplate_ = false;
    }
}

AuthorityType Authenticator::detectAuthorityType(const String& authority)
{
    Logger::info(Tag(), "detectAuthorityType");

    if (authority.empty())
    {
        throw IllegalArgumentException("authority");
    }

    if (!Url(authority).isValid())
    {
        Logger::error(Tag(), Constants::rmsauthErrorMessage().AuthorityInvalidUriFormat);
        throw IllegalArgumentException(Constants::rmsauthErrorMessage().AuthorityInvalidUriFormat, authority);
    }

    Url authorityUri(authority);
    if (authorityUri.scheme() != "https")
    {
        Logger::error(Tag(), Constants::rmsauthErrorMessage().AuthorityUriInsecure);
        throw IllegalArgumentException(Constants::rmsauthErrorMessage().AuthorityUriInsecure, authority);
    }

    String path = authorityUri.path().substr(1);
    if (path.empty())
    {
        Logger::error(Tag(), Constants::rmsauthErrorMessage().AuthorityUriInvalidPath);
        throw IllegalArgumentException(Constants::rmsauthErrorMessage().AuthorityUriInvalidPath, authority);
    }

    String firstPath = path.substr(0, path.find("/"));
    AuthorityType authorityType = Authenticator::isAdfsAuthority(firstPath) ? AuthorityType::ADFS : AuthorityType::AAD;

    return authorityType;
}

String Authenticator::canonicalizeUri(const String& uri)
{
    if (!uri.empty() && !StringUtils::endsWith(uri, "/"))
    {
        return uri + "/";
    }

    return uri;
}

String Authenticator::replaceTenantlessTenant(const String& authority, const String& tenantId)
{
    Logger::info(Tag(), "replaceTenantlessTenant");

    // TODO.shch: porting
//        auto regex = new Regex(Regex.Escape(TenantlessTenantName), RegexOptions.IgnoreCase);
//        return regex.Replace(authority, tenantId, 1);
    return StringUtils::replace(authority, tenantlessTenantName(), tenantId);
}

bool Authenticator::isAdfsAuthority(const String& firstPath)
{
    bool res = StringUtils::compareIC(firstPath, "adfs") == 0;
    Logger::info(Tag(), "isAdfsAuthority: '%'", res);
    return res;
}

} // namespace rmsauth {
