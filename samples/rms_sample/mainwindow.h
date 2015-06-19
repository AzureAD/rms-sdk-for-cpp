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

#include <rmsauth/AuthenticationContext.h>
#include <rmsauth/FileCache.h>

#include <UserRights.h>
#include <rights.h>
#include "pfileconverter.h"

using namespace rmscore::modernapi;
using namespace std;

class AuthCallback : public IAuthenticationCallback {
private:

  std::shared_ptr<rmsauth::FileCache> FileCachePtr;
  QString clientId_;
  QString redirectUrl_;

public:

  AuthCallback(const QString& clientId, const QString& redirectUrl);
  virtual string GetToken(shared_ptr<AuthenticationParameters>& ap) override;
};

class ConsentCallback : public IConsentCallback {
public:

  virtual ConsentList Consents(ConsentList& consents) override;
};

class TemplatesCallback : public ITemplatesCallback {
public:

  virtual size_t SelectTemplate(std::vector<TemplateDescriptor>& templates) override;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
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
  TemplatesCallback templates;

  void addCertificates();
  std::vector<UserRights> openRightsDlg();
};

#endif // MAINWINDOW_H
