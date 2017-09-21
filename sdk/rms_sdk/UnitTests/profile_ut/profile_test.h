/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PROFILETEST_H
#define PROFILETEST_H

#include <QObject>
#include <QTest>

class ProfileTest : public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void TestProfileLoad();
};

#endif // PROFILETEST_H
