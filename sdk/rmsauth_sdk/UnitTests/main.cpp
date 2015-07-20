/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QDir>
#include <QApplication>
#include <QCommandLineParser>
#include <fstream>
#include "../rmsauth/HttpHelper.h"
#include "NonInteractiveTests.h"
#include "InteractiveTests.h"

using namespace std;

void addCertificates(QString path);

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QCommandLineParser parser;

  parser.addHelpOption();

  // A boolean option with multiple names (-i, --interactive)
  QCommandLineOption interactiveOption(
    QStringList() << "i" << "interactive", "Run interactive test cases.");

  QCommandLineOption clientId({ "client_id", "cl" }, "Client <id>", "id");
  QCommandLineOption resource({ "resource", "r" }, "Requested <resource>",
                              "resource");
  QCommandLineOption authority({ "authority", "a" }, "Access <authority>",
                               "authority");
  QCommandLineOption userName({ "user", "u" }, "Use user <name>", "name");
  QCommandLineOption userPassword({ "password", "p" }, "Use user <password>",
                                  "password");
  QCommandLineOption clientSecret({ "clientSecret", "s" }, "Client <secret>",
                                  "secret");
  QCommandLineOption certificatesPath({ "certificates", "cp" }, "Certificates <path>",
                                  "path");


  parser.addOption(interactiveOption);
  parser.addOption(clientId);
  parser.addOption(resource);
  parser.addOption(authority);
  parser.addOption(userName);
  parser.addOption(userPassword);
  parser.addOption(clientSecret);
  parser.addOption(certificatesPath);

  // Process the actual command line arguments given by the user
  parser.process(app);

  bool interactive        = parser.isSet(interactiveOption);
  QString clientIdStr     = parser.value(clientId);
  QString resourceStr     = parser.value(resource);
  QString authorityStr    = parser.value(authority);
  QString userNameStr     = parser.value(userName);
  QString userPasswordStr = parser.value(userPassword);
  QString clientSecretStr = parser.value(clientSecret);
  QString certificatesPathStr = parser.value(certificatesPath);

  if(!certificatesPathStr.isEmpty()) {
      addCertificates(certificatesPathStr);
  }

  qDebug() << "interactiveOption: " << interactive;
  qDebug() << "clientId: " << clientIdStr;
  qDebug() << "resource: " << resourceStr;
  qDebug() << "authority: " << authorityStr;
  qDebug() << "userName: " << userNameStr;
  qDebug() << "userPassword: " << userPasswordStr;
  qDebug() << "clientSecret: " << clientSecretStr;
  qDebug() << "CertificatesPath: " << certificatesPathStr;

  int res = 0;

  if (interactive)
  {
    res += QTest::qExec(new InteractiveTests());
  }

  res +=
    QTest::qExec(new NonInteractiveTests(clientIdStr, resourceStr, authorityStr,
                                         userNameStr, userPasswordStr,
                                         clientSecretStr));

  return res;
}

void addCertificates(QString path) {
  QDir dir(path);
  QStringList filters;

  filters << "*.cer" << "*.der" << "*.pem";
  auto filesList = dir.entryInfoList(filters);
  std::vector<uint8_t> buffer;

  int cnt = 0;

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
        if(rmsauth::HttpHelper::addCACertificateDer(buffer)) {
            cnt ++;
        }
      } else {
          cnt ++;
      }
    }
    file.close();
  }

  qDebug() << "Loaded " << cnt << "certificates";
}

