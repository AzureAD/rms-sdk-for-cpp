/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QApplication>
#include <QCommandLineParser>
#include "NonInteractiveTests.h"
#include "InteractiveTests.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QCommandLineParser parser;

  parser.addHelpOption();

  // A boolean option with multiple names (-i, --interactive)
  QCommandLineOption interactiveOption(
    QStringList() << "i" << "interactive", "Run interactive test cases.");

  QCommandLineOption clientId({ "client_id", "c" }, "Client <id>", "id");
  QCommandLineOption resource({ "resource", "r" }, "Requested <resource>",
                              "resource");
  QCommandLineOption authority({ "authority", "a" }, "Access <authority>",
                               "authority");
  QCommandLineOption userName({ "user", "u" }, "Use user <name>", "name");
  QCommandLineOption userPassword({ "password", "p" }, "Use user <password>",
                                  "password");
  QCommandLineOption clientSecret({ "clientSecret", "s" }, "Client <secret>",
                                  "secret");


  parser.addOption(interactiveOption);
  parser.addOption(clientId);
  parser.addOption(resource);
  parser.addOption(authority);
  parser.addOption(userName);
  parser.addOption(userPassword);
  parser.addOption(clientSecret);

  // Process the actual command line arguments given by the user
  parser.process(app);

  bool interactive        = parser.isSet(interactiveOption);
  QString clientIdStr     = parser.value(clientId);
  QString resourceStr     = parser.value(resource);
  QString authorityStr    = parser.value(authority);
  QString userNameStr     = parser.value(userName);
  QString userPasswordStr = parser.value(userPassword);
  QString clientSecretStr = parser.value(clientSecret);

  qDebug() << "interactiveOption: " << interactive;

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
