/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QNetworkAccessManager>
#include <QSsl>
#include <QSslConfiguration>
#include <QtNetwork>
#include <QPair>
#include <QUrlQuery>
#include <QSharedPointer>
#include <QFileDialog>
#include <QStandardItemModel>

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include <rmsauth/HttpHelper.h>
#include <rmsauth/Exceptions.h>
#include <ModernAPI/HttpHelper.h>
#include <ModernAPI/RMSExceptions.h>

#include "mainwindow.h"
#include "ui_templatesdialog.h"
#include "rightsdialog.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace rmsauth;

AuthCallback::AuthCallback(const QString& clientId, const QString& redirectUrl)
  : clientId_(clientId)
  , redirectUrl_(redirectUrl)
{
  FileCachePtr = std::make_shared<FileCache>();
}

string AuthCallback::GetToken(shared_ptr<AuthenticationParameters>& ap) {
  string redirect =
    ap->Scope().empty() ? redirectUrl_.toStdString() : ap->Scope();

  try
  {
    if (redirect.empty()) {
      throw rmscore::exceptions::RMSInvalidArgumentException(
              "redirect Url is empty");
    }

    if (clientId_.isEmpty()) {
      throw rmscore::exceptions::RMSInvalidArgumentException("client Id is empty");
    }

    AuthenticationContext authContext(
      ap->Authority(), AuthorityValidationType::False, FileCachePtr);

    auto result = authContext.acquireToken(ap->Resource(),
                                           clientId_.toStdString(), redirect,
                                           PromptBehavior::Auto);
    return result->accessToken();
  }
  catch (const rmsauth::Exception& ex)
  {
    cout << "!!!!! Auth error: " << ex.error() << endl;
    throw;
  }
  return "";
}

ConsentList ConsentCallback::Consents(ConsentList& /*consents*/) {
  ConsentList result;

  return result;
}

size_t TemplatesCallback::SelectTemplate(
  std::vector<TemplateDescriptor>& templates)
{
  // show dialog
  auto selectTemplateDlg = new QDialog(0, 0);

  Ui_TemplatesDialog selectTemplate;

  selectTemplate.setupUi(selectTemplateDlg);

  for (size_t pos = 0, last = templates.size(); pos < last; ++pos) {
    selectTemplate.comboBoxTemplates->insertItem(0, QString::fromStdString(
                                                   templates[pos].Name()));
  }

  selectTemplateDlg->exec();

  return static_cast<size_t>(selectTemplate.comboBoxTemplates->currentIndex());
}

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)

{
  ui->setupUi(this);


  qDebug() << "---- Start! -----";
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_certificatesPathButton_clicked()
{
  QString dir =
    QFileDialog::getExistingDirectory(this, tr(
                                        "Select certificates directory"),
                                      ui->ineEdit_certificatesPath->text(),
                                      QFileDialog::ShowDirsOnly |
                                      QFileDialog::DontResolveSymlinks);

  if (!dir.isEmpty()) {
    ui->ineEdit_certificatesPath->setText(dir);
  }
}

void MainWindow::addCertificates() {
  auto dirStr = ui->ineEdit_certificatesPath->text();
  QDir dir(dirStr);
  QStringList filters;
  filters << "*.cer" << "*.der" << "*.pem";
  auto filesList = dir.entryInfoList(filters);
  std::vector<uint8_t> buffer;

  for (auto& fileName : filesList) {
    ifstream file(
      fileName.absoluteFilePath().toStdString(),
      ios_base::in | ios_base::binary | ios_base::ate);

    if (!file.is_open()) continue;

    buffer.resize(file.tellg());
    file.seekg(0, ios::beg);

    if (file.read(reinterpret_cast<char *>(buffer.data()), buffer.size())) {
      // add certificate to RmsAuth
      if (!rmsauth::HttpHelper::addCACertificateBase64(buffer)) {
        rmsauth::HttpHelper::addCACertificateDer(buffer);
      }

      // add certificate to RMS
      if (!rmscore::modernapi::HttpHelper::addCACertificateBase64(buffer)) {
        rmscore::modernapi::HttpHelper::addCACertificateDer(buffer);
      }
    }
    file.close();
  }
}

void MainWindow::on_encryptPFILETemplatesButton_clicked()
{
  QString fileIn = QFileDialog::getOpenFileName(this, tr(
                                                  "Select file to encrypt"));
  QString fileOut = fileIn + ".pfile";

  if (!fileIn.isEmpty()) {
    // add certificates
    addCertificates();

    auto inFile = std::make_shared<ifstream>(
      fileIn.toStdString(), ios_base::in | ios_base::binary);
    auto outFile = std::make_shared<fstream>(
      fileOut.toStdString(),
      ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

    if (!inFile->is_open()) {
      this->ui->textBrowser->insertPlainText("ERROR: Failed to open '");
      this->ui->textBrowser->insertPlainText(fileIn);
      this->ui->textBrowser->insertPlainText("\n");
      return;
    }

    if (!outFile->is_open()) {
      this->ui->textBrowser->insertPlainText("ERROR: Failed to open '");
      this->ui->textBrowser->insertPlainText(fileOut);
      this->ui->textBrowser->insertPlainText("\n");
      return;
    }

    auto fileParts  = fileIn.split(".");
    QString fileExt = fileParts.at(fileParts.size() - 1);
    fileExt = "." + fileExt;

    try {
      AuthCallback auth(
        ui->lineEdit_clientId->text(), ui->lineEdit_redirectUrl->text());
      PFileConverter::ConvertToPFileTemplates(
        ui->lineEdit_clientEmail->text().toStdString(),
        inFile,
        fileExt.toStdString(),
        outFile,
        auth,
        this->consent,
        this->templates);
      this->ui->textBrowser->insertPlainText("Successfully converted to '");
      this->ui->textBrowser->insertPlainText(fileOut);
      this->ui->textBrowser->insertPlainText("'\n");
    }
    catch (const rmsauth::Exception& e) {
      this->ui->textBrowser->insertPlainText("ERROR: ");
      this->ui->textBrowser->insertPlainText(e.error().c_str());
      this->ui->textBrowser->insertPlainText("\n");

      outFile->close();
      std::remove(fileOut.toStdString().c_str());
    }
    catch (const rmscore::exceptions::RMSException& e) {
      this->ui->textBrowser->insertPlainText("ERROR: ");
      this->ui->textBrowser->insertPlainText(e.what());
      this->ui->textBrowser->insertPlainText("\n");

      outFile->close();
      std::remove(fileOut.toStdString().c_str());
    }
    inFile->close();
    outFile->close();
  }
}

void MainWindow::on_fromPFILEButton_clicked()
{
  QString fileIn =
    QFileDialog::getOpenFileName(this, tr("Select file to decrypt"));
  QString fileOut;

  if (!fileIn.isEmpty()) {
    // add certificates
    addCertificates();

    auto inFile = std::make_shared<ifstream>(
      fileIn.toStdString(), ios_base::in | ios_base::binary);

    if (!inFile->is_open()) {
      this->ui->textBrowser->insertPlainText("ERROR: Failed to open '");
      this->ui->textBrowser->insertPlainText(fileIn);
      this->ui->textBrowser->insertPlainText("\n");
      return;
    }

    // check extension
    QStringList fileParts = fileIn.split(".");
    fileOut = fileIn.mid(0, fileIn.size() - fileParts.at(
                           fileParts.size() - 1).size() - 1);

    // create streams
    auto outFile = std::make_shared<fstream>(
      fileOut.toStdString(),
      ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

    if (!outFile->is_open()) {
      this->ui->textBrowser->insertPlainText("ERROR: Failed to open '");
      this->ui->textBrowser->insertPlainText(fileOut);
      this->ui->textBrowser->insertPlainText("\n");
      return;
    }

    try
    {
      AuthCallback auth(
        ui->lineEdit_clientId->text(), ui->lineEdit_redirectUrl->text());
      auto pfs = PFileConverter::ConvertFromPFile(
        this->ui->lineEdit_clientEmail->text().toStdString(),
        inFile,
        outFile,
        auth,
        this->consent);

      cout << "Policy Name: " << pfs->m_stream->Policy()->Name() << endl;
      cout << "Policy Description: " << pfs->m_stream->Policy()->Description() <<
        endl;
      time_t tmp = std::chrono::system_clock::to_time_t(
        pfs->m_stream->Policy()->ContentValidUntil());
      std::tm time;
      memset(&time, 0, sizeof(time));
#if defined _WIN32
      localtime_s(&time, &tmp);
#else // if defined _WIN32
      time = *std::localtime(&tmp);
#endif // if defined _WIN32
      cout << "Policy ContentValidUntil: " <<
        time.tm_mon << "-" << time.tm_mday << "-" << time.tm_year << " " <<
        time.tm_hour << ":" << time.tm_min << "." << time.tm_sec << endl;

      // cout << "Policy ContentValidUntil: " <<
      // pfs->m_stream->Policy()->ContentValidUntil() << endl;

      this->ui->textBrowser->insertPlainText(QString(
                                               "Successfully converted to '%1'\n").arg(
                                               fileOut));
    }
    catch (const rmsauth::Exception& e)
    {
      this->ui->textBrowser->insertPlainText("ERROR: ");
      this->ui->textBrowser->insertPlainText(e.error().c_str());
      this->ui->textBrowser->insertPlainText("\n");
    }
    catch (const rmscore::exceptions::RMSException& e) {
      this->ui->textBrowser->insertPlainText("ERROR: ");
      this->ui->textBrowser->insertPlainText(e.what());
      this->ui->textBrowser->insertPlainText("\n");
    }
    inFile->close();
    outFile->close();
  }
}

void MainWindow::on_encryptPFILERightsButton_clicked()
{
  QString fileIn = QFileDialog::getOpenFileName(this, tr(
                                                  "Select file to encrypt"));
  QString fileOut = fileIn + ".pfile";

  if (!fileIn.isEmpty()) {
    // add certificates
    addCertificates();

    auto inFile = std::make_shared<ifstream>(
      fileIn.toStdString(), ios_base::in | ios_base::binary);
    auto outFile = std::make_shared<fstream>(
      fileOut.toStdString(),
      ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

    if (!inFile->is_open()) {
      this->ui->textBrowser->insertPlainText("ERROR: Failed to open '");
      this->ui->textBrowser->insertPlainText(fileIn);
      this->ui->textBrowser->insertPlainText("\n");
      return;
    }

    if (!outFile->is_open()) {
      this->ui->textBrowser->insertPlainText("ERROR: Failed to open '");
      this->ui->textBrowser->insertPlainText(fileOut);
      this->ui->textBrowser->insertPlainText("\n");
      return;
    }

    auto fileParts  = fileIn.split(".");
    QString fileExt = fileParts.at(fileParts.size() - 1);
    fileExt = "." + fileExt;

    vector<UserRights> userRights = openRightsDlg();

    // is anything to add
    if (userRights.size() == 0) {
      this->ui->textBrowser->insertPlainText(
        "Please fill email and check rights\n'");
      return;
    }


    try {
      AuthCallback auth(
        ui->lineEdit_clientId->text(), ui->lineEdit_redirectUrl->text());
      PFileConverter::ConvertToPFilePredefinedRights(
        ui->lineEdit_clientEmail->text().toStdString(),
        inFile,
        fileExt.toStdString(),
        outFile,
        auth,
        this->consent,
        userRights);
      this->ui->textBrowser->insertPlainText("Successfully converted to '");
      this->ui->textBrowser->insertPlainText(fileOut);
      this->ui->textBrowser->insertPlainText("'\n");
    }
    catch (const rmsauth::Exception& e) {
      this->ui->textBrowser->insertPlainText("ERROR: ");
      this->ui->textBrowser->insertPlainText(e.error().c_str());
      this->ui->textBrowser->insertPlainText("\n");

      outFile->close();
      std::remove(fileOut.toStdString().c_str());
    }
    catch (const rmscore::exceptions::RMSException& e) {
      this->ui->textBrowser->insertPlainText("ERROR: ");
      this->ui->textBrowser->insertPlainText(e.what());
      this->ui->textBrowser->insertPlainText("\n");

      outFile->close();
      std::remove(fileOut.toStdString().c_str());
    }
    inFile->close();
    outFile->close();
  }
}

std::vector<UserRights>MainWindow::openRightsDlg() {
  RightsDialog dlg;
  QStandardItemModel model(&dlg);
  QStringList columnsNames { "User email" };

  vector<UserRights> userRights;

  columnsNames.push_back(QString::fromStdString(CommonRights::Owner()));
  columnsNames.push_back(QString::fromStdString(CommonRights::View()));
  columnsNames.push_back(QString::fromStdString(EditableDocumentRights::Edit()));
  columnsNames.push_back(QString::fromStdString(EditableDocumentRights::Export()));
  columnsNames.push_back(QString::fromStdString(EditableDocumentRights::Extract()));
  columnsNames.push_back(QString::fromStdString(EditableDocumentRights::Print()));
  columnsNames.push_back(QString::fromStdString(EditableDocumentRights::Comment()));


  model.setHorizontalHeaderLabels(columnsNames);

  for (int row = 0; row < 10; ++row) {
    for (int col = 0, colMax = columnsNames.size(); col < colMax; ++col) {
      QStandardItem *item = new QStandardItem;

      if (col > 0) {
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setFlags(
          Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      }
      model.setItem(row, col, item);
    }
  }

  dlg.InitRightsList(model);

  if (dlg.exec() != QDialog::Accepted) {
    return userRights;
  }

  // now create rights list
  for (auto row = 0, rowMax = model.rowCount(); row < rowMax; row++) {
    rmscore::modernapi::UserList  users;
    rmscore::modernapi::RightList rights;
    QString tmpStr;

    for (int col = 0; col < 6; ++col) {
      QStandardItem *item = model.item(row, col);

      if (item == nullptr) continue;

      if (col == 0) {
        tmpStr = item->text();

        if (!tmpStr.isEmpty()) {
          users.push_back(tmpStr.toStdString());
        }
      } else {
        rights.push_back(columnsNames[col].toStdString());
      }
    }

    // check for validity
    if ((users.size() > 0) && (rights.size() > 0)) {
      userRights.push_back(UserRights(users, rights));
    }
  }
  return userRights;
}
