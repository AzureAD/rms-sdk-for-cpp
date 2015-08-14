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

using namespace rmscore::modernapi;
int main(int argc, char *argv[])
{
  // set RMS Environment
  auto env = IRMSEnvironment::Environment();
  env->LogOption(IRMSEnvironment::LoggerOption::Never);


  QApplication a(argc, argv);
  MainWindow window;

  window.show();

  a.exec();
}
