/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <IRMSEnvironment.h>
#include <rmsauth/IRMSAuthEnvironment.h>

int main(int argc, char *argv[])
{
  // set RMS Environment
  auto envRMS = rmscore::modernapi::RMSEnvironment();
  auto envAuth = rmsauth::RMSAuthEnvironment();
  envRMS->LogOption(rmscore::modernapi::IRMSEnvironment::LoggerOption::Always);
  envAuth->LogOption(rmsauth::IRMSAuthEnvironment::LoggerOption::Always);


  QApplication a(argc, argv);
  auto window = std::make_shared<MainWindow>();

  window->show();

  a.exec();
}
