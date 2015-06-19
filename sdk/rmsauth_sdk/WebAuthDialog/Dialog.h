/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtGlobal>
#include <QLabel>

#if defined(RMSAUTH_WEB_AUTH_DIALOG_LIBRARY)
#  define MYSHAREDLIB_EXPORT Q_DECL_EXPORT
#else
#  define MYSHAREDLIB_EXPORT Q_DECL_IMPORT
#endif

namespace Ui {
class Dialog;
}

class QNetworkReply;

class MYSHAREDLIB_EXPORT Dialog : public QDialog
{
    Q_OBJECT
    static const QString& Tag() {static const QString tag="Dialog"; return tag;}
public:
    explicit Dialog(const QString& requestUrl, const QString& redirectUrl, bool useCookie = false, QWidget *parent = 0);
    ~Dialog();

    const QString& respondUrl() const {return respondUrl_;}
    void clearCookie();

protected:
    void showEvent(QShowEvent * event) override;

private:
    Ui::Dialog *ui;
    QString requestUrl_;
    QString redirectUrl_;
    QString respondUrl_;
    bool useCookie_;

private slots:
    void slot_webView_loadFinished(bool);
    void slot_authManager_finished(QNetworkReply *);

private:
    void processAuthReply(QNetworkReply *reply);

};

#endif // DIALOG_H
