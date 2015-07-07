/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QString>
#include "../CryptoAPI/CryptoAPI.h"
#include "../CryptoAPI/RMSCryptoExceptions.h"
#include "CryptoAPITests.h"

using namespace std;

void CryptoAPITests::EncryptDecryptBlockTest_data() {
  QTest::addColumn<QString>("keyWrapper");
  QTest::addColumn<QString>("testData");

  QTest::newRow("0") << "TestWrapperOne" << "Source String One";
  QTest::newRow("1") << "TestWrapperTwo" <<
    "To sorry world an at do spoil along. Incommode he depending do frankness remainder to.";
}

void CryptoAPITests::EncryptDecryptBlockTest() {
  QFETCH(QString, keyWrapper);
  QFETCH(QString, testData);
  try {
    auto testDataStr = testData.toStdString();
    auto dataIn      =
      std::make_shared<std::vector<uint8_t> >(testDataStr.length());
    memcpy(dataIn->data(), testDataStr.data(), dataIn->size());


    auto encData = rmscrypto::api::EncryptWithAutoKey(dataIn,
                                                      rmscrypto::api::CIPHER_MODE_CBC4K,
                                                      keyWrapper.toStdString());
    QVERIFY2(encData.get() != nullptr &&
             encData->size() != 0, "Failed to encrypt data!");

    auto decData = rmscrypto::api::DecryptWithAutoKey(encData,
                                                      rmscrypto::api::CIPHER_MODE_CBC4K,
                                                      keyWrapper.toStdString());

    QVERIFY2(decData.get() != nullptr &&
             decData->size() == dataIn->size(), "Failed to decrypt data!");

    QVERIFY2(memcmp(decData->data(), dataIn->data(),
                    dataIn->size()) == 0, "Failed to decrypt data!");
  } catch (rmscrypto::exceptions::RMSCryptoException& e) {
    QTest::qFail(e.what(), __FILE__, __LINE__);
  }
}
