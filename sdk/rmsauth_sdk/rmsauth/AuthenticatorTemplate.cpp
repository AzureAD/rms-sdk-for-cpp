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

const String AuthenticatorTemplate::V1_ENDPOINT() 
{
    static const String str = "";
    return str;
}
const String AuthenticatorTemplate::V2_ENDPOINT()
{
    static const String str = "v2.0/";
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
const String AuthenticatorTemplate::VERSION()
{
    static const String str = "{version}";
    return str;
}

AuthenticatorTemplatePtr AuthenticatorTemplate::create(const String& host, const EndpointVersion endpointVersion)
{
    static const String tokenEndpointTemplate = "https://{host}/{tenant}/oauth2/{version}token";
    static const String userRealmEndpointTemplate = "https://{host}/Common/UserRealm";
    static const String authorityTemplate = "https://{host}/{tenant}/";
    static const String instanceDiscoveryEndpointTemplate = "https://{host}/Common/discovery/instance";

    auto res = std::make_shared<AuthenticatorTemplate>();
    String endpointString;

    switch (endpointVersion) {
    case V1: 
        endpointString = SELF::V1_ENDPOINT();
        break;
    case V2:
        endpointString = SELF::V2_ENDPOINT();
        break;
    }

    String tokenEndpointWithVersion = StringUtils::replace(tokenEndpointTemplate, SELF::VERSION(), endpointString);

    res->host_ = host;
    res->authority_ = StringUtils::replace(authorityTemplate, SELF::HOST(), host);
    res->instanceDiscoveryEndpoint_ = StringUtils::replace(instanceDiscoveryEndpointTemplate, SELF::HOST(), host);
    res->authorizeEndpoint_ = StringUtils::replace(SELF::authorizeEndpointTemplate(), SELF::HOST(), host);
    res->tokenEndpoint_ = StringUtils::replace(tokenEndpointWithVersion, SELF::HOST(), host);
    res->issuer_ = res->tokenEndpoint_;
    res->userRealmEndpoint_ = StringUtils::replace(userRealmEndpointTemplate, SELF::HOST(), host);
    return res;
}
AuthenticatorTemplatePtr AuthenticatorTemplate::createFromHost(const String& host)
{
    return create(host, V1);
}

AuthenticatorTemplatePtr AuthenticatorTemplate::createFromHost(const String& host, const EndpointVersion version) 
{
    return create(host, version);
}

AuthenticatorTemplate::EndpointVersion AuthenticatorTemplate::getEndpointVersion(const String& url) 
{
    int v2Index = url.find("/v2.0");
    if (v2Index > 0) {
        return V2;
    }
    return V1;
}


} // namespace rmsauth {
