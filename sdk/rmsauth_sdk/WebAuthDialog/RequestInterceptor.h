/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef REQUESTINTERCEPTOR_H
#define REQUESTINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include <QDebug>

class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit RequestInterceptor(QObject* parent = Q_NULLPTR) : QWebEngineUrlRequestInterceptor(parent) {}
    virtual void interceptRequest(QWebEngineUrlRequestInfo& info) Q_DECL_OVERRIDE;

signals:
    void redirectUrlCapture(QUrl);
};

#endif // REQUESTINTERCEPTOR_H
