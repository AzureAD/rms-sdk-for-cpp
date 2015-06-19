/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CRYPTOAPITEST
#define CRYPTOAPITEST

#include <QTest>

class CryptoAPITests : public QObject
{
    Q_OBJECT

private Q_SLOTS:

  void EncryptDecryptBlockTest_data();
  void EncryptDecryptBlockTest();
};

#endif // CRYPTOAPITEST

