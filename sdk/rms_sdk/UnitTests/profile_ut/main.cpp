/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <QCoreApplication>
#include "profile_test.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  int res = 0;
  res += QTest::qExec(new ProfileTest(), argc, argv);

  return res;
}
