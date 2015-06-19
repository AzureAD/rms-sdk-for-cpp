/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <rmsauth/AuthenticationContext.h>


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
    auto authority = ui->lineEdit_authority_ac->text().toStdString();
    auto redirectUri = ui->lineEdit_redirectUri_ac->text().toStdString();
    auto resource = ui->lineEdit_resource_ac->text().toStdString();
    auto clientId = ui->lineEdit_clientId_ac->text().toStdString();
    std::vector<PromptBehavior> intToPromptBehavior{PromptBehavior::Auto, PromptBehavior::Always, PromptBehavior::Never};
    PromptBehavior pb(intToPromptBehavior[ui->comboBox_promptBehavior->currentIndex()]);

    try
    {
        AuthenticationContext authContext(authority, AuthorityValidationType::False, nullptr);
        auto result = authContext.acquireToken(resource, clientId, redirectUri, pb);

        ui->textBrowser_ac->append("Access token: " + QString::fromStdString(result->accessToken()));
    }
    catch(const Exception& ex)
    {
        ui->textBrowser_ac->append("Exception: " + QString::fromStdString(ex.error()));
    }
    catch(...)
    {
        ui->textBrowser_ac->append("Exception: Unknown");
    }
}

void MainWindow::on_pushButtonGetToken_cc_clicked()
{
    auto authority = ui->lineEdit_authority_cc->text().toStdString();
    auto clientId = ui->lineEdit_clientId_cc->text().toStdString();
    auto redirectUri = ui->lineEdit_redirectUri_cc->text().toStdString();
    auto resource = ui->lineEdit_resource_cc->text().toStdString();
    auto clientSecret = ui->lineEdit_clientSecret->text().toStdString();

    try
    {
        AuthenticationContext authContext(authority, AuthorityValidationType::False, nullptr);
        auto clientCredential = std::make_shared<ClientCredential>(clientId, clientSecret);
        auto result = authContext.acquireToken(resource, clientCredential);
        ui->textBrowser_cc->append("Access token: " + QString::fromStdString(result->accessToken()));
    }
    catch(const Exception& ex)
    {
        ui->textBrowser_cc->append("Exception: " + QString::fromStdString(ex.error()));
    }
    catch(...)
    {
        ui->textBrowser_cc->append("Exception: Unknown");
    }
}

void MainWindow::on_pushButtonGetToken_pwd_clicked()
{
    auto authority = ui->lineEdit_authority_pwd->text().toStdString();
    auto clientId = ui->lineEdit_clientId_pwd->text().toStdString();
    auto redirectUri = ui->lineEdit_redirectUri_pwd->text().toStdString();
    auto resource = ui->lineEdit_resource_pwd->text().toStdString();
    auto userName = ui->lineEdit_userName->text().toStdString();
    auto userPassword = ui->lineEdit_password->text().toStdString();
    try
    {
        AuthenticationContext authContext(authority, AuthorityValidationType::False, nullptr);
        auto userCredential = std::make_shared<UserCredential>(userName, userPassword);
        auto result = authContext.acquireToken(resource, clientId, userCredential);
        ui->textBrowser_pwd->append("Access token: " + QString::fromStdString(result->accessToken()));
    }
    catch(const Exception& ex)
    {
        ui->textBrowser_pwd->append("Exception: " + QString::fromStdString(ex.error()));
    }
    catch(...)
    {
        ui->textBrowser_pwd->append("Exception: Unknown");
    }

}
