/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "RequestInterceptor.h"
#include <QUrl>

/**
 * @brief RequestInterceptor::interceptRequest
 * This is used to handle the RedirectUrl which has a http/https scheme. We do not fail the
 * request as this captures other internet traffic as well.
 * @param info
 */

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info)
{
    QUrl url = info.requestUrl();
    qDebug() << "Request URL in interceptRequest: " << url;

    emit redirectUrlCapture(url);
}

