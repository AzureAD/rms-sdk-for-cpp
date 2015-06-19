/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_PLATFORMCRYPTOTEST_H
#define _RMS_LIB_PLATFORMCRYPTOTEST_H
#include <QtTest>

namespace rmscore {
namespace test {
class PlatformCryptoTest : public QObject {
  Q_OBJECT

public:

  PlatformCryptoTest();
  ~PlatformCryptoTest();

private Q_SLOTS:

  void testSHA_data();
  void testSHA();
  void testAESECB_data();
  void testAESECB();
  void testAESECB_Bad_data();
  void testAESECB_Bad();
  void testAESCBC_data();
  void testAESCBC();
  void testAESCBC_Bad_data();
  void testAESCBC_Bad();
  void testAESCBCPKCS7_data();
  void testAESCBCPKCS7();
  void testAESCBCPKCS7_Bad_data();
  void testAESCBCPKCS7_Bad();
};
} // namespace test
} // namespace rmscore
#endif // _RMS_LIB_PLATFORMCRYPTOTEST_H
