/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "RequestInterceptor.h"
#include <QUrl>

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info)
{
    // Intercepting the requested URL
    QUrl url = info.requestUrl();
    qDebug() << "Request URL in interceptRequest: " << url;

    emit redirectUrlCapture(url);
}

