/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <RequestParameters.h>
#include <utils.h>
#include <vector>
#include <qpair>
#include <QUrlQuery>

namespace rmsauth {

RequestParameters::RequestParameters(const String& resource, ClientKeyPtr clientKeyPtr)
{
    addParam(OAuthConstants::oAuthParameter().Resource, resource);
    if(clientKeyPtr != nullptr)
        addClientKey(*clientKeyPtr);
}

String& RequestParameters::operator[](const String & key)
{
    return params_[key];
}

const String& RequestParameters::operator[](const String & key) const
{
    auto it = params_.find(key);
    if(it == params_.end())
        throw IllegalArgumentException("operator[]");
    return it->second;
}

void RequestParameters::addParam(const String & key, const String & value)
{
    params_.insert(make_pair(key, value));
}


void RequestParameters::setServerProvidedParameters(const String& param)
{
    serverProvidedParameters_ = StringUtils::urlSplitParameters(param);
}

String RequestParameters::serverProvidedQueryParametersToString() const
{
    StringStream ss;
    bool first = true;
    for (auto& pair : serverProvidedParameters_)
    {
        if (!first) 
            ss << "&";
        ss << pair.first << "=" << pair.second;
        first = false;
    }
    return ss.str();
}


String RequestParameters::toString() const
{
    StringStream ss;
    bool first = true;
    //extra params coming from server should override params_
    StringMap aggregatedParams = serverProvidedParameters_;
    for (auto& pair : params_)
    {
        aggregatedParams.insert(pair);
    }

    for(auto& pair : aggregatedParams)
    {
        if (!first) ss << "&";
        ss << pair.first << "=" << pair.second;
        first = false;
    }
    return ss.str();
}

void RequestParameters::addClientKey(const ClientKey& clientKey)
{
    addParam(OAuthConstants::oAuthParameter().ClientId, clientKey.clientId());

    if (clientKey.credential() != nullptr)
    {
        addParam(OAuthConstants::oAuthParameter().ClientSecret, RequestParameters::uriEncode(clientKey.credential()->clientSecret()));
    }
    else if (clientKey.certificate() != nullptr)
    {
// TODO.shch: complete
//            addParam(OAuthConstants::oAuthParameter().ClientAssertionType, clientKey.assertion().assertionType());
//            addParam(OAuthConstants::oAuthParameter().ClientAssertion, clientKey.assertion().assertion());
    }
    else if (clientKey.assertion() != nullptr)
    {
//            JsonWebToken jwtToken = new JsonWebToken(clientKey.Certificate, clientKey.Authenticator.SelfSignedJwtAudience);
//            ClientAssertion clientAssertion = jwtToken.Sign(clientKey.Certificate);
//            this[OAuthParameter.ClientAssertionType] = clientAssertion.AssertionType;
//            this[OAuthParameter.ClientAssertion] = clientAssertion.Assertion;
    }
}

} // namespace rmsauth {
