/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <sstream>
#include "CryptedStreamTests.h"
#include "../CryptoAPI/CryptoAPI.h"
#include "../CryptoAPI/RMSCryptoExceptions.h"

using namespace std;
void CryptedStreamTests::CryptedStreamToMemory_data() {
  QTest::addColumn<QString>("aesKey");
  QTest::addColumn<QString>("plainData");
  QTest::addColumn<QString>("iv");
  QTest::addColumn<QString>("encryptedData");
  QTest::addColumn<qint8>(  "algorithm");

  QTest::newRow("AES_CBC4K") << "7bddca3c7ff9bdc8eea7a8eb63d01f11" <<
    "Plain Text" << "00000000000000000000000000000000"
                             << "ea21b48f530be23b6d654db9302dd1ff" <<
    static_cast<qint8>(rmscrypto::api::CIPHER_MODE_CBC4K);
}

void CryptedStreamTests::CryptedStreamToMemory() {
  shared_ptr<stringstream> backingBuffer = make_shared<stringstream>(
    ios::in | ios::out | ios::binary);
  vector<uint8_t> key(16);

  QFETCH(QString, aesKey);
  QFETCH(QString, plainData);
  QFETCH(QString, iv);
  QFETCH(QString, encryptedData);
  QFETCH(qint8,   algorithm);

  try {
    QByteArray qKey = QByteArray::fromHex(aesKey.toUtf8());
    QByteArray qData(plainData.toUtf8());
    QByteArray qIV                  = QByteArray::fromHex(iv.toUtf8());
    QByteArray qEncryptedData       = QByteArray::fromHex(encryptedData.toUtf8());
    rmscrypto::api::CipherMode algo =
      static_cast<rmscrypto::api::CipherMode>(algorithm);

    memcpy(key.data(), qKey.data(), key.size());

    auto backingStream =
      rmscrypto::api::CreateStreamFromStdStream(static_pointer_cast<iostream>(
                                                  backingBuffer));

    auto cryptoStream = rmscrypto::api::CreateCryptoStream(
      algo, key, backingStream);

    cryptoStream->Write(reinterpret_cast<const uint8_t *>(qData.data()),
                        qData.size());
    cryptoStream->Flush();

    auto res = backingBuffer->str();
    QVERIFY2(
      static_cast<int>(
        res.length()) == qEncryptedData.size(), "Invalid encrypted size!");
    QVERIFY2(memcmp(res.data(), qEncryptedData.data(), res.length()) == 0,
             "Invalid encrypted data!");

    cryptoStream->Seek(0);
    QByteArray dst(qData.size(), 0);
    auto read = cryptoStream->Read(
      reinterpret_cast<uint8_t *>(dst.data()), dst.size());
    QVERIFY2(read == dst.size(),
             "Invalid decrypted size!");
    QVERIFY2(memcmp(dst.data(), qData.data(), dst.size()) == 0,
             "Invalid decrypted data!");
  } catch (const rmscrypto::exceptions::RMSCryptoException& e) {
    QTest::qFail(e.what(), __FILE__, __LINE__);
  }
}
