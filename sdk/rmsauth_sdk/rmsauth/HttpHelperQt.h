/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef HTTPHELPERQT
#define HTTPHELPERQT

#include <CallState.h>
#include <RequestParameters.h>
#include <Entities.h>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace rmsauth {

class HttpHelperQt
{
    static const String& Tag() {static const String tag="HttpHelperQt"; return tag;}

public:
    static QNetworkRequest createRequest();
    static void addHeadersToRequest(QNetworkRequest& request, const Headers& headers);

    static TokenResponsePtr deserializeTokenResponse(const QByteArray& body);
    static InstanceDiscoveryResponsePtr deserializeInstanceDiscoveryResponse(const QByteArray& body);
    static ErrorResponsePtr parseResponseError(QNetworkReply* pReply);
    static void verifyCorrelationIdHeaderInReponse(QNetworkReply* pReply, CallStatePtr callState);

    static void addCorrelationIdHeadersToRequest(QNetworkRequest& request, CallStatePtr callState);

    static QByteArray jobPost(QNetworkRequest& request, const RequestParameters& requestParameters, CallStatePtr callState);
    static QByteArray jobPostRunner(QNetworkRequest& request, const RequestParameters& requestParameters, CallStatePtr callState);

    static QByteArray jobGet(QNetworkRequest& request, CallStatePtr callState);
    static QByteArray jobGetRunner(QNetworkRequest& request, CallStatePtr callState);

    static void logRequestHeaders(const QNetworkRequest& req);
    static void logResponseHeaders(QNetworkReply *pReply);
    static void logResponseBody(const QByteArray& body);
};

} //namespace rmsauth {

#endif // HTTPHELPERQT

