/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "RedirectUrlSchemeHandler.h"
#include <QDebug>
#include <QWebEngineUrlRequestJob>

/**
 * @brief RedirectUrlSchemeHandler::requestStarted
 * This is used to handle the RedirectUrl which has a custom scheme. We capture the url
 * and fail the request.
 * @param request
 */
void RedirectUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob* request){
    request->fail(QWebEngineUrlRequestJob::RequestAborted);

    QUrl url = request->requestUrl();
    qDebug() << "Request URL in RedirectUrlSchemeHandler: " << url;
    emit urlCapture(url);
}
