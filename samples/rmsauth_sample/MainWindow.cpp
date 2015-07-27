/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

using namespace rmsauth;

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButtonGetToken_ac_clicked()
{
  std::vector<PromptBehavior> intToPromptBehavior { PromptBehavior::Auto,
                                                    PromptBehavior::Always,
                                                    PromptBehavior::Never };
  PromptBehavior pb(
    intToPromptBehavior[ui->comboBox_promptBehavior->currentIndex()]);

  auto authority   = ui->lineEdit_authority_ac->text().toStdString();
  auto redirectUri = ui->lineEdit_redirectUri_ac->text().toStdString();
  auto resource    = ui->lineEdit_resource_ac->text().toStdString();
  auto clientId    = ui->lineEdit_clientId_ac->text().toStdString();

  GetTokenUsingUI(authority, redirectUri, resource, clientId, pb);
}

void MainWindow::on_pushButtonGetToken_cc_clicked()
{
  auto authority    = ui->lineEdit_authority_cc->text().toStdString();
  auto resource     = ui->lineEdit_resource_cc->text().toStdString();
  auto clientId     = ui->lineEdit_clientId_cc->text().toStdString();
  auto clientSecret = ui->lineEdit_clientSecret->text().toStdString();

  GetTokenUsingSecret(authority, resource, clientId, clientSecret);
}

void MainWindow::on_pushButtonGetToken_pwd_clicked()
{
  auto authority    = ui->lineEdit_authority_pwd->text().toStdString();
  auto clientId     = ui->lineEdit_clientId_pwd->text().toStdString();
  auto resource     = ui->lineEdit_resource_pwd->text().toStdString();
  auto userName     = ui->lineEdit_userName->text().toStdString();
  auto userPassword = ui->lineEdit_password->text().toStdString();

  GetTokenUsingUserPassword(authority, resource, clientId, userName,
                            userPassword);
}

void MainWindow::GetTokenUsingUI(const std::string& authority,
                                 const std::string& redirectUri,
                                 const std::string& resource,
                                 const std::string& clientId,
                                 PromptBehavior   & pb) {
  try
  {
    // create authentication context
    AuthenticationContext authContext(authority,
                                      AuthorityValidationType::False,
                                      nullptr);

    // acquire token using ui
    auto result = authContext.acquireToken(resource, clientId, redirectUri, pb);

    AddLog("Access token: ", result->accessToken().c_str());
  }
  catch (const Exception& ex)
  {
    AddLog("Exception: ", ex.error().c_str());
  }
  catch (...)
  {
    AddLog("Exception: ", "Unknown");
  }
}

void MainWindow::GetTokenUsingSecret(const std::string& authority,
                                     const std::string& resource,
                                     const std::string& clientId,
                                     const std::string& clientSecret) {
  try
  {
    // create authentication context
    AuthenticationContext authContext(authority,
                                      AuthorityValidationType::False,
                                      nullptr);

    // create user credentials using secret
    auto clientCredential = std::make_shared<ClientCredential>(clientId,
                                                               clientSecret);

    // acquire token using secret
    auto result = authContext.acquireToken(resource, clientCredential);
    AddLog("Access token: ", result->accessToken().c_str());
  }
  catch (const Exception& ex)
  {
    AddLog("Exception: ", ex.error().c_str());
  }
  catch (...)
  {
    AddLog("Exception: ", "Unknown");
  }
}

void MainWindow::GetTokenUsingUserPassword(const std::string& authority,
                                           const std::string& resource,
                                           const std::string& clientId,
                                           const std::string& userName,
                                           const std::string& userPassword) {
  try
  {
    // create authentication context
    AuthenticationContext authContext(authority,
                                      AuthorityValidationType::False,
                                      nullptr);

    // create user credentials using user name and password
    auto userCredential =
      std::make_shared<UserCredential>(userName, userPassword);

    // acquire token using user name and password
    auto result = authContext.acquireToken(resource,
                                           clientId,
                                           userCredential);
    AddLog("Access token: ", result->accessToken().c_str());
  }
  catch (const Exception& ex)
  {
    AddLog("Exception: ", ex.error().c_str());
  }
  catch (...)
  {
    AddLog("Exception: ", "Unknown");
  }
}

void MainWindow::AddLog(const std::string& tag,
                        const char        *message) {
  AddLog(QString::fromStdString(tag), QString::fromLatin1(message));
}

void MainWindow::AddLog(const QString& tag,
                        const QString& message) {
  ui->textBrowser_cc->append(tag + message);
}
