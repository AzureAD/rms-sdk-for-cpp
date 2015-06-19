/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AuthenticatorTemplate.h>
#include <utils.h>

namespace rmsauth {

using SELF = AuthenticatorTemplate;

//const String AuthenticatorTemplate::instanceDiscoveryEndpointTemplate()
//{
//    static const String str = "https://{host}/Common/discovery/instance";
//    return str;
//}

const String AuthenticatorTemplate::authorizeEndpointTemplate()
{
    static const String str = "https://{host}/{tenant}/oauth2/authorize";
    return str;

}
const String AuthenticatorTemplate::HOST()
{
    static const String str = "{host}";
    return str;

}
const String AuthenticatorTemplate::TENANT()
{
    static const String str = "{tenant}";
    return str;

}
AuthenticatorTemplatePtr AuthenticatorTemplate::createFromHost(const String& host)
{
    static const String tokenEndpointTemplate = "https://{host}/{tenant}/oauth2/token";
    static const String userRealmEndpointTemplate = "https://{host}/Common/UserRealm";
    static const String authorityTemplate = "https://{host}/{tenant}/";
    static const String instanceDiscoveryEndpointTemplate = "https://{host}/Common/discovery/instance";

    auto res = std::make_shared<AuthenticatorTemplate>();
    res->host_ = host;
    res->authority_ = StringUtils::replace(authorityTemplate, SELF::HOST(), host);
    res->instanceDiscoveryEndpoint_ = StringUtils::replace(instanceDiscoveryEndpointTemplate, SELF::HOST(), host);
    res->authorizeEndpoint_ = StringUtils::replace(SELF::authorizeEndpointTemplate(), SELF::HOST(), host);
    res->tokenEndpoint_ = StringUtils::replace(tokenEndpointTemplate, SELF::HOST(), host);
    res->issuer_ = res->tokenEndpoint_;
    res->userRealmEndpoint_ = StringUtils::replace(userRealmEndpointTemplate, SELF::HOST(), host);
    return res;
}

} // namespace rmsauth {
