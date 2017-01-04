/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "dialog.h"
#include "RedirectUrlSchemeHandler.h"
#include "RequestInterceptor.h"
#include "ui_Dialog.h"
#include <QWebEngineProfile>

Dialog::Dialog(const QString& requestUrl, const QString& redirectUrl, bool useCookie, QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::Dialog)
    , mRequestUrl(requestUrl)
    , mRedirectUrl(redirectUrl)
{
    mUi->setupUi(this);

    while (mRedirectUrl.endsWith('/'))
    {
        mRedirectUrl = mRedirectUrl.remove(mRedirectUrl.length()-1, 1);
    }

    qDebug() << ": RequestUrl:" << mRequestUrl;
    qDebug() << ": RedirectUrl:" << mRedirectUrl;

    std::unique_ptr<QWebEngineProfile> profile (new QWebEngineProfile(mUi->webEngineView));

    std::unique_ptr<RequestInterceptor> interceptor (new RequestInterceptor(mUi->webEngineView));
    QObject::connect(interceptor.get(), SIGNAL(redirectUrlCapture(QUrl)), this, SLOT(processAuthReply(QUrl)));
    profile->setRequestInterceptor(interceptor.release());

    std::unique_ptr<RedirectUrlSchemeHandler> redirectUrlSchemeInterceptor
            (new RedirectUrlSchemeHandler(mUi->webEngineView));
    QObject::connect(redirectUrlSchemeInterceptor.get(), SIGNAL(urlCapture(QUrl)), this, SLOT(processAuthReply(QUrl)));

    QUrl redirectUrlFromString(mRedirectUrl);
    if (redirectUrlFromString.scheme() != "https")
    {
        profile->installUrlSchemeHandler(
                    redirectUrlFromString.scheme().toStdString().c_str(),
                    redirectUrlSchemeInterceptor.release());
    }

    std::unique_ptr<QWebEnginePage> page (new QWebEnginePage(profile.release(), mUi->webEngineView));
    mUi->webEngineView->setPage(page.release());
}

Dialog::~Dialog() {}

void Dialog::showEvent(QShowEvent* /*event*/)
{
    QUrl url(mRequestUrl);
    mUi->webEngineView->load(url);
    mUi->webEngineView->show();
}

void Dialog::processAuthReply(QUrl locationUri)
{
    const QString redirectUri = locationUri.scheme() + "://" + locationUri.host();
    if(redirectUri.compare(mRedirectUrl, Qt::CaseInsensitive) == 0)
    {
        mRespondUrl = locationUri.toString();
        this->accept();
    }
}
