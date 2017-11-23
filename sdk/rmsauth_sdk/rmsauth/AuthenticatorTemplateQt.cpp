/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AuthenticatorTemplate.h>
#include <Exceptions.h>
#include <Constants.h>
#include <utils.h>
#include <RmsauthIdHelper.h>
#include <Logger.h>
#include "HttpHelperQt.h"
#include <QNetworkRequest>
#include <QEventLoop>
#include <QCoreApplication>
#include <QTimer>

namespace rmsauth {

void AuthenticatorTemplate::verifyAnotherHostByInstanceDiscoveryAsync(const String& host, const String& tenant, CallStatePtr callState)
{
    Logger::info(Tag(), "verifyAnotherHostByInstanceDiscoveryAsync");

    String instanceDiscoveryEndpoint = instanceDiscoveryEndpoint_;
    instanceDiscoveryEndpoint += ("?api-version=1.0&authorization_endpoint=" + AuthenticatorTemplate::authorizeEndpointTemplate());
    instanceDiscoveryEndpoint = StringUtils::replace(instanceDiscoveryEndpoint, AuthenticatorTemplate::HOST(), host);
    instanceDiscoveryEndpoint = StringUtils::replace(instanceDiscoveryEndpoint, AuthenticatorTemplate::TENANT(), tenant);

    QNetworkRequest request = HttpHelperQt::createRequest();
    request.setUrl(QUrl(instanceDiscoveryEndpoint.data()));

//    HttpHelperQt::addCorrelationIdHeadersToRequest(request, callState);
    HttpHelperQt::addHeadersToRequest(request, RmsauthIdHelper::getPlatformHeaders());
    HttpHelperQt::addHeadersToRequest(request, RmsauthIdHelper::getProductHeaders());

    InstanceDiscoveryResponsePtr discoveryResponsePtr = nullptr;
    if(QCoreApplication::instance() == nullptr)
    {
        auto fut = std::async(&HttpHelperQt::jobGetRunner, std::ref(request), callState);
        auto body = fut.get();
        discoveryResponsePtr = HttpHelperQt::deserializeInstanceDiscoveryResponse(body);
    }
    else
    {
        auto body = HttpHelperQt::jobGet(request, callState);
        discoveryResponsePtr = HttpHelperQt::deserializeInstanceDiscoveryResponse(body);
    }

    if (discoveryResponsePtr == nullptr || discoveryResponsePtr->tenantDiscoveryEndpoint.empty())
    {
        throw RmsauthException(Constants::rmsauthError().AuthorityNotInValidList);
    }
}

} // namespace rmsauth {
