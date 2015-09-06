/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QMainWindow>
#include <QList>
#include <QNetworkReply>
#include <QSslError>
#include <QThread>

#include <rmsauth/AuthenticationContext.h>
#include <rmsauth/FileCache.h>

#include <UserRights.h>
#include <rights.h>
#include "pfileconverter.h"

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

class TemplatesCallbackUI : public ITemplatesCallback {
private:

  QObject *_mainApp;

public:

  TemplatesCallbackUI(QObject *mainApp);

  virtual size_t SelectTemplate(
    std::shared_ptr<std::vector<TemplateDescriptor> >templates)
  override;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow,
                   public std::enable_shared_from_this<MainWindow>{
  Q_OBJECT

public:

  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:

  void on_certificatesPathButton_clicked();
  void on_encryptPFILETemplatesButton_clicked();
  void on_fromPFILEButton_clicked();
  void on_encryptPFILERightsButton_clicked();

private:

  Ui::MainWindow *ui;
  ConsentCallback consent;
  TemplatesCallbackUI templatesUI;
  std::shared_ptr<std::atomic<bool> > cancelState;

  void                   addCertificates();
  std::vector<UserRights>openRightsDlg();


  void                   ConvertToPFILEUsingTemplates(const string& fileIn,
                                                      const string& clientId,
                                                      const string& redirectUrl,
                                                      const string& clientEmail);

  void ConvertToPFILEUsingRights(const string            & fileIn,
                                 const vector<UserRights>& userRights,
                                 const string            & clientId,
                                 const string            & redirectUrl,
                                 const string            & clientEmail);

  void ConvertFromPFILE(const string& fileIn,
                        const string& clientId,
                        const string& redirectUrl,
                        const string& clientEmail);

  void   AddLog(const QString& tag,
                const QString& message);
  void   AddLog(const string& tag,
                const char   *message);
  string SelectFile(const string& msg);
};
#endif // MAINWINDOW_H
