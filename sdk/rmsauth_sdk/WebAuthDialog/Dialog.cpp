/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "Dialog.h"
#include "ui_Dialog.h"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMovie>
#include <QTimer>
#include "CookieJar.h"
#include <types.h>
#include <Exceptions.h>
//#include <Logger.h>

using namespace rmsauth;

Dialog::Dialog(const QString& requestUrl, const QString& redirectUrl, bool useCookie, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , requestUrl_(requestUrl)
    , redirectUrl_(redirectUrl)
    , useCookie_(useCookie)

{
    ui->setupUi(this);

    while (redirectUrl_.endsWith('/'))
    {
        redirectUrl_ = redirectUrl_.remove(redirectUrl_.length()-1, 1);
    }

    qDebug() << Tag() << ": requestUrl_:" << requestUrl_;
    qDebug() << Tag() << ": redirectUrl_:" << redirectUrl_;

    QObject::connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(slot_webView_loadFinished(bool)));
    auto authManager = ui->webView->page()->networkAccessManager();
    if(useCookie_)
    {
        authManager->setCookieJar(new CookieJar);
    }

    QObject::connect(authManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_authManager_finished(QNetworkReply*)));

    ui->webView->hide();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::clearCookie()
{
    auto authManager = ui->webView->page()->networkAccessManager();
    auto cookieJarPtr = authManager->cookieJar();
    if (cookieJarPtr != nullptr)
    {
        QTimer::singleShot(0, cookieJarPtr, SLOT(clear()));
    }
}

void Dialog::showEvent(QShowEvent* /*event*/)
{
    StringStream ss; ss << "==> requestUrl_: " << requestUrl_.toStdString();
//    Logger::info(Tag().toStdString(), ss.str());
    QUrl url(requestUrl_);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "MSOpenTechWebAuthenticationDialog");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    QMovie *movie = new QMovie(":/images/loading.gif");
    if (!movie->isValid())
    {
        qDebug() << Tag() << "Can't load image";
    }

    ui->lblLoading->setMovie(movie);
    movie->start();

    ui->webView->load(request);
}

void Dialog::slot_webView_loadFinished(bool ok)
{
    if(ok)
    {
        ui->lblLoading->hide();
        ui->lblLoading->movie()->stop();
        ui->webView->show();
    }
}

void Dialog::slot_authManager_finished(QNetworkReply* pReply)
{
//   Logger::info(Tag().toStdString(), "on_authManager_finished");
/*
    if (pReply->rawHeaderPairs().length() > 0)
    {
        Logger::info(Tag().toStdString(), "--> Header:");
        foreach (const QNetworkReply::RawHeaderPair& pair, pReply->rawHeaderPairs())
        {
             StringStream ss; ss << pair.first.toStdString() << ": " <<  pair.second.toStdString();
             Logger::info(Tag().toStdString(), ss.str());
        }
    }

    QByteArray body = pReply->readAll();
    if (body.length() > 0)
    {
        Logger::info(Tag().toStdString(), "==> Body:");
        Logger::info(Tag().toStdString(), String(body.begin(), body.end()));
    }
*/
    QNetworkReply::NetworkError error_type = pReply->error();
    if (error_type == QNetworkReply::NoError)
    {
        this->processAuthReply(pReply);
    }
    else
    {
        const QString ERROR("error");
        const QString ACCESS_DENIED("access_denied");
        const QString ERROR_SUBCODE("error_subcode");
        const QString CANCELED("cancel");

        QUrl replyUri = QUrl(pReply->url());
//        Logger::info(Tag().toStdString(), replyUri.toString().toStdString());
        qDebug() << Tag() << ": " << replyUri.toString();

        const QString redirectUri = replyUri.scheme() + "://" + replyUri.host();
        if (redirectUri.compare(redirectUrl_, Qt::CaseInsensitive) == 0)
        {
            QUrlQuery query(replyUri);
            if(query.hasQueryItem(ERROR)
                && query.queryItemValue(ERROR).compare(ACCESS_DENIED, Qt::CaseInsensitive) == 0
                && query.hasQueryItem(ERROR_SUBCODE)
                && query.queryItemValue(ERROR_SUBCODE).compare(CANCELED, Qt::CaseInsensitive) == 0)
            {
                this->reject();
            }
        }
        else
        {
            QString errMsg = QString("WebAuthenticationDialog error: %1").arg(pReply->errorString());
            qDebug() << Tag() << ": " << errMsg;
//            Logger::info(Tag().toStdString(), errMsg.toStdString());
            this->reject();
            throw RmsauthException(String(errMsg.toStdString()));
        }
    }

    pReply->deleteLater();
}

void Dialog::processAuthReply(QNetworkReply* pReply)
{
    if (pReply->rawHeaderPairs().length() > 0)
    {
        QByteArray location = pReply->rawHeader("Location");
        if(!location.isEmpty())
        {
            QUrl locationUri = QUrl(location);
            const QString redirectUri = locationUri.scheme() + "://" + locationUri.host();
            if(redirectUri.compare(redirectUrl_, Qt::CaseInsensitive) == 0)
            {
//                Logger::info(Tag().toStdString(), "==> pReply->url: %", pReply->url().toString().toStdString());
//                Logger::info(Tag().toStdString(), "==> locationUri: %", location.toStdString());

                respondUrl_ = locationUri.toString();
                this->accept();
            }
        }
    }
}
