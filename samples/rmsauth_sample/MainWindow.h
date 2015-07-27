/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <rmsauth/AuthenticationContext.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:

  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:

  void on_pushButtonGetToken_ac_clicked();
  void on_pushButtonGetToken_cc_clicked();
  void on_pushButtonGetToken_pwd_clicked();

private:

  void GetTokenUsingUI(const std::string& authority,
                       const std::string& redirectUri,
                       const std::string& resource,
                       const std::string& clientId,
                       PromptBehavior   & pb);

  void GetTokenUsingSecret(const std::string& authority,
                           const std::string& resource,
                           const std::string& clientId,
                           const std::string& clientSecret);

  void GetTokenUsingUserPassword(const std::string& authority,
                                 const std::string& resource,
                                 const std::string& clientId,
                                 const std::string& userName,
                                 const std::string& userPassword);


  void AddLog(const std::string& tag,
              const char        *message);
  void AddLog(const QString& tag,
              const QString& message);

  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
