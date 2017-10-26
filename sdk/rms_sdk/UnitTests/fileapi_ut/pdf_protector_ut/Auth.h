/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_AUTH_H_
#define RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_AUTH_H_

#include <QMessageBox>
#include <QFileDialog>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include <QString>
#include <QMainWindow>
#include <QList>
#include <QNetworkReply>
#include <QSslError>
#include <QThread>

#include <rmsauth/HttpHelper.h>
#include <rmsauth/Exceptions.h>
#include <ModernAPI/HttpHelper.h>
#include <ModernAPI/RMSExceptions.h>

#include <rmsauth/AuthenticationContext.h>
#include <ModernAPI/AuthenticationCallbackImpl.h>
#include <ModernAPI/IConsentCallback.h>
#include <rmsauth/FileCache.h>
#include <Pole/pole.h>

#include <UserRights.h>
#include <rights.h>

void postToMainThread(const std::function<void()>& func,
                      QObject                     *mainApp);

class AuthCallback : public rmscore::modernapi::IAuthenticationCallback {
private:

  std::shared_ptr<rmsauth::FileCache> FileCachePtr;
  std::string clientId_;
  std::string redirectUrl_;

public:

  AuthCallback(const std::string& clientId,
               const std::string& redirectUrl);
  virtual std::string GetToken(std::shared_ptr<rmscore::modernapi::AuthenticationParameters>& ap) override;
};

class AuthCallbackUI : public rmscore::modernapi::IAuthenticationCallback {
private:

  QObject *_mainApp;
  AuthCallback _callback;

public:

  AuthCallbackUI(QObject           *mainApp,
                 const std::string& clientId,
                 const std::string& redirectUrl);
  virtual std::string GetToken(std::shared_ptr<rmscore::modernapi::AuthenticationParameters>& ap) override;
};
class ConsentCallback : public rmscore::modernapi::IConsentCallback {
public:

  virtual rmscore::modernapi::ConsentList Consents(rmscore::modernapi::ConsentList& consents) override;
};
#endif // RMSSDK_UNITTESTS_FILEAPI_UT_PDF_PROTECTOR_UT_AUTH_H_
