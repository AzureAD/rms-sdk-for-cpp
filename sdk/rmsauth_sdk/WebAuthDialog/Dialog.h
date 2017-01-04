/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef WEBAUTHDIALOG_DIALOG_H
#define WEBAUTHDIALOG_DIALOG_H

#include <memory>
#include <QDialog>
#include <QUrl>

#if defined(RMSAUTH_WEB_AUTH_DIALOG_LIBRARY)
#  define MYSHAREDLIB_EXPORT Q_DECL_EXPORT
#else
#  define MYSHAREDLIB_EXPORT Q_DECL_IMPORT
#endif

namespace Ui {
class Dialog;
}

class MYSHAREDLIB_EXPORT Dialog : public QDialog
{
    Q_OBJECT

public:
    const QString& requestUrl() const {return mRequestUrl;}
    const QString& redirectUrl() const {return mRedirectUrl;}
    const QString& respondUrl() const {return mRespondUrl;}

    explicit Dialog(
            const QString& requestUrl,
            const QString& redirectUrl,
            bool useCookie = false,
            QWidget* parent = 0);

    ~Dialog();

protected:
    void showEvent(QShowEvent* event) override;

private:
    /**
     * mRequestUrl: represents the Url requested for this Dialog
     * mRedirectUrl: represents the Redirect Url passed to the Dialog
     * mRespondUrl: used to capture the token returned from the ADAL auth flow
     */
    std::unique_ptr<Ui::Dialog> mUi;
    QString mRequestUrl;
    QString mRedirectUrl;
    QString mRespondUrl;

private slots:
    void processAuthReply(QUrl);

};

#endif // WEBAUTHDIALOG_DIALOG_H
