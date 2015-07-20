/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef NONINTERACTIVETEST_H
#define NONINTERACTIVETEST_H
#include <QTest>

class NonInteractiveTests : public QObject {
  Q_OBJECT

public:

  NonInteractiveTests(QString clientId,
                      QString resource,
                      QString authority,
                      QString userName,
                      QString password,
                      QString clientSecret);

private Q_SLOTS:

  void AuthenticationResultTest_data();
  void AuthenticationResultTest();
  void CacheTest_data();
  void CacheTest();
  void FileCacheEncryptedTest_data();
  void FileCacheEncryptedTest();

  void AcquireTokenNonInteractiveHandlerTest();
  void AcquireTokenForClientHandlerTest();

private:
  QString clientId_;
  QString resource_;
  QString authority_;
  QString userName_;
  QString password_;
  QString clientSecret_;
};

#endif // NONINTERACTIVETEST_H
