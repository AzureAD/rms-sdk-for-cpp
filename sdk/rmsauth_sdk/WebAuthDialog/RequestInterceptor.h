/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef REQUESTINTERCEPTOR_H
#define REQUESTINTERCEPTOR_H

#include <QDebug>
#include <QWebEngineUrlRequestInterceptor>

class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit RequestInterceptor(QObject* parent = Q_NULLPTR) : QWebEngineUrlRequestInterceptor(parent) {}
    /**
     * @brief RequestInterceptor::interceptRequest
     * This is used to handle the RedirectUrl which has a http/https scheme. We do not fail the
     * request as this captures other internet traffic as well.
     * @param info
     */
    virtual void interceptRequest(QWebEngineUrlRequestInfo& info) Q_DECL_OVERRIDE;

signals:
    void redirectUrlCapture(QUrl);
};

#endif // REQUESTINTERCEPTOR_H
