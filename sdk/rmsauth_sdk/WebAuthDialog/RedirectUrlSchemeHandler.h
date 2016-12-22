/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef REDIRECTURLSCHEMEHANDLER_H
#define REDIRECTURLSCHEMEHANDLER_H

#include <QUrl>
#include <QWebEngineUrlSchemeHandler>

class RedirectUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT

public:
    explicit RedirectUrlSchemeHandler(QObject *parent = 0) : QWebEngineUrlSchemeHandler(parent) {}
    void requestStarted(QWebEngineUrlRequestJob *request);

signals:
    void urlCapture(QUrl);
};

#endif // REDIRECTURLSCHEMEHANDLER_H
