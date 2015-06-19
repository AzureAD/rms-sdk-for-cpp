/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef COMMONTESTA_H
#define COMMONTESTA_H
#include <QTest>

class KeyStorageTests : public QObject {
  Q_OBJECT

private Q_SLOTS:

  void KeyUsage_data();
  void KeyUsage();
};

#endif // COMMONTESTA_H
