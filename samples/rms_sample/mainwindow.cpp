/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QFileDialog>

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

AuthCallback::AuthCallback(const string& clientId, const string& redirectUrl)
  : clientId_(clientId)
  , redirectUrl_(redirectUrl)
{
  FileCachePtr = make_shared<FileCache>();
}

string AuthCallback::GetToken(shared_ptr<AuthenticationParameters>& ap) {
  string redirect =
    ap->Scope().empty() ? redirectUrl_ : ap->Scope();

  try
  {
    if (redirect.empty()) {
      throw rmscore::exceptions::RMSInvalidArgumentException(
              "redirect Url is empty");
    }

    if (clientId_.empty()) {
      throw rmscore::exceptions::RMSInvalidArgumentException("client Id is empty");
    }

    AuthenticationContext authContext(
      ap->Authority(), AuthorityValidationType::False, FileCachePtr);

    auto result = authContext.acquireToken(ap->Resource(),
                                           clientId_, redirect,
                                           PromptBehavior::Auto,
                                           ap->UserId());
    return result->accessToken();
  }
  catch (const rmsauth::Exception& /*ex*/)
  {
    // out logs
    throw;
  }
}

ConsentList ConsentCallback::Consents(ConsentList& /*consents*/) {
  ConsentList result;

  return result;
}

size_t TemplatesCallback::SelectTemplate(
  vector<TemplateDescriptor>& templates)
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
  vector<uint8_t> buffer;

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
  string fileIn = SelectFile("Select file to encrypt");

  if (!fileIn.empty()) {
    ConvertToPFILEUsingTemplates(
      fileIn,
      ui->lineEdit_clientId->text().toStdString(),
      ui->lineEdit_redirectUrl->text().toStdString(),
      ui->lineEdit_clientEmail->text().toStdString());
  }
}

void MainWindow::on_fromPFILEButton_clicked()
{
  string fileIn = SelectFile("Select file to decrypt");

  if (!fileIn.empty()) {
    ConvertFromPFILE(
      fileIn,
      ui->lineEdit_clientId->text().toStdString(),
      ui->lineEdit_redirectUrl->text().toStdString(),
      ui->lineEdit_clientEmail->text().toStdString());
  }
}

void MainWindow::on_encryptPFILERightsButton_clicked()
{
  string fileIn = SelectFile("Select file to encrypt");

  if (!fileIn.empty()) {
    vector<UserRights> userRights = openRightsDlg();

    ConvertToPFILEUsingRights(
      fileIn,
      userRights,
      ui->lineEdit_clientId->text().toStdString(),
      ui->lineEdit_redirectUrl->text().toStdString(),
      ui->lineEdit_clientEmail->text().toStdString());
  }
}

void MainWindow::ConvertToPFILEUsingTemplates(const string& fileIn,
                                              const string& clientId,
                                              const string& redirectUrl,
                                              const string& clientEmail) {
  // generate output filename
  string fileOut = fileIn + ".pfile";

  // add trusted certificates using HttpHelpers of RMS and Auth SDKs
  addCertificates();

  // create shared in/out streams
  auto inFile = make_shared<ifstream>(
    fileIn, ios_base::in | ios_base::binary);
  auto outFile = make_shared<fstream>(
    fileOut, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

  if (!inFile->is_open()) {
    AddLog("ERROR: Failed to open ", fileIn.c_str());
    return;
  }

  if (!outFile->is_open()) {
    AddLog("ERROR: Failed to open ", fileOut.c_str());
    return;
  }

  // find file extension
  string fileExt;
  auto   pos = fileIn.find_last_of('.');

  if (pos != string::npos) {
    fileExt = fileIn.substr(pos);
  }

  try {
    // create authentication callback
    AuthCallback auth(clientId, redirectUrl);

    // process convertion
    PFileConverter::ConvertToPFileTemplates(
      clientEmail, inFile, fileExt, outFile, auth,
      this->consent, this->templates);

    AddLog("Successfully converted to ", fileOut.c_str());
  }
  catch (const rmsauth::Exception& e) {
    AddLog("ERROR: ", e.error().c_str());
    outFile->close();
    remove(fileOut.c_str());
  }
  catch (const rmscore::exceptions::RMSException& e) {
    AddLog("ERROR: ", e.what());

    outFile->close();
    remove(fileOut.c_str());
  }
  inFile->close();
  outFile->close();
}

void MainWindow::ConvertToPFILEUsingRights(const string            & fileIn,
                                           const vector<UserRights>& userRights,
                                           const string            & clientId,
                                           const string            & redirectUrl,
                                           const string            & clientEmail)
{
  // generate output filename
  string fileOut = fileIn + ".pfile";

  // add trusted certificates using HttpHelpers of RMS and Auth SDKs
  addCertificates();

  // create shared in/out streams
  auto inFile = make_shared<ifstream>(
    fileIn, ios_base::in | ios_base::binary);
  auto outFile = make_shared<fstream>(
    fileOut, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

  if (!inFile->is_open()) {
    AddLog("ERROR: Failed to open ", fileIn.c_str());
    return;
  }

  if (!outFile->is_open()) {
    AddLog("ERROR: Failed to open ", fileOut.c_str());
    return;
  }

  // find file extension
  string fileExt;
  auto   pos = fileIn.find_last_of('.');

  if (pos != string::npos) {
    fileExt = fileIn.substr(pos);
  }

  // is anything to add
  if (userRights.size() == 0) {
    AddLog("ERROR: ", "Please fill email and check rights");
    return;
  }


  try {
    // create authentication callback
    AuthCallback auth(clientId, redirectUrl);

    // process convertion
    PFileConverter::ConvertToPFilePredefinedRights(
      clientEmail,
      inFile,
      fileExt,
      outFile,
      auth,
      this->consent,
      userRights);

    AddLog("Successfully converted to ", fileOut.c_str());
  }
  catch (const rmsauth::Exception& e) {
    AddLog("ERROR: ", e.error().c_str());

    outFile->close();
    remove(fileOut.c_str());
  }
  catch (const rmscore::exceptions::RMSException& e) {
    AddLog("ERROR: ", e.what());

    outFile->close();
    remove(fileOut.c_str());
  }
  inFile->close();
  outFile->close();
}

void MainWindow::ConvertFromPFILE(const string& fileIn,
                                  const string& clientId,
                                  const string& redirectUrl,
                                  const string& clientEmail) {
  // add trusted certificates using HttpHelpers of RMS and Auth SDKs
  addCertificates();

  // create shared in/out streams
  auto inFile = make_shared<ifstream>(
    fileIn, ios_base::in | ios_base::binary);

  if (!inFile->is_open()) {
    AddLog("ERROR: Failed to open ", fileIn.c_str());
    return;
  }

  string fileOut;

  // generate output filename
  auto pos = fileIn.find_last_of('.');

  if (pos != string::npos) {
    fileOut = fileIn.substr(0, pos);
  }

  // create streams
  auto outFile = make_shared<fstream>(
    fileOut, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

  if (!outFile->is_open()) {
    AddLog("ERROR: Failed to open ", fileOut.c_str());
    return;
  }

  try
  {
    // create authentication context
    AuthCallback auth(clientId, redirectUrl);

    // process convertion
    auto pfs = PFileConverter::ConvertFromPFile(
      clientEmail,
      inFile,
      outFile,
      auth,
      this->consent);

    switch (pfs->m_status) {
    case Success:
      AddLog("Successfully converted to ", fileOut.c_str());
      break;

    case NoRights:
      AddLog("User has no rights to PFILE!", "");
      break;

    case Expired:
      AddLog("Content has expired!", "");
      break;
    }
  }
  catch (const rmsauth::Exception& e)
  {
    AddLog("ERROR: ", e.error().c_str());
  }
  catch (const rmscore::exceptions::RMSException& e) {
    AddLog("ERROR: ", e.what());
  }
  inFile->close();
  outFile->close();
}

string MainWindow::SelectFile(const string& msg) {
  QString fileSel =
    QFileDialog::getOpenFileName(this, QString::fromStdString(msg));

  return fileSel.toStdString();
}

vector<UserRights>MainWindow::openRightsDlg() {
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

void MainWindow::AddLog(const string& tag, const char *message) {
  AddLog(QString::fromStdString(tag), QString::fromLatin1(message));
}

void MainWindow::AddLog(const QString& tag, const QString& message) {
  this->ui->textBrowser->insertPlainText(tag);
  this->ui->textBrowser->insertPlainText(message);
  this->ui->textBrowser->insertPlainText("\n");
}
