/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMessageBox>
#include <QFileDialog>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <rmsauth/HttpHelper.h>
#include <rmsauth/Exceptions.h>
#include <ModernAPI/HttpHelper.h>
#include <ModernAPI/RMSExceptions.h>
#include <QString>
#include <QMainWindow>
#include <QList>
#include <QNetworkReply>
#include <QSslError>
#include <QThread>

#include <rmsauth/AuthenticationContext.h>
#include <ModernAPI/AuthenticationCallbackImpl.h>
#include <ModernAPI/IConsentCallback.h>
#include <rmsauth/FileCache.h>
#include <Pole/pole.h>

#include <UserRights.h>
#include <rights.h>
using namespace rmscore::modernapi;
using namespace std;

void postToMainThread(const std::function<void()>& func,
                      QObject                     *mainApp);

class AuthCallback : public IAuthenticationCallback {
private:

  std::shared_ptr<rmsauth::FileCache> FileCachePtr;
  std::string clientId_;
  std::string redirectUrl_;

public:

  AuthCallback(const std::string& clientId,
               const std::string& redirectUrl);
  virtual std::string GetToken(shared_ptr<AuthenticationParameters>& ap) override;
};

class AuthCallbackUI : public IAuthenticationCallback {
private:

  QObject *_mainApp;
  AuthCallback _callback;

public:

  AuthCallbackUI(QObject           *mainApp,
                 const std::string& clientId,
                 const std::string& redirectUrl);
  virtual std::string GetToken(shared_ptr<AuthenticationParameters>& ap) override;
};
class ConsentCallback : public IConsentCallback {
public:

  virtual ConsentList Consents(ConsentList& consents) override;
};
#endif
