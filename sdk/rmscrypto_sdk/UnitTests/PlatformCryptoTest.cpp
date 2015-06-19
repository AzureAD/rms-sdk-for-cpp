/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <CryptoAPI.h>
#include "TestHelpers.h"
#include "PlatformCryptoTest.h"

using namespace rmscrypto::api;
namespace rmscore {
namespace test {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-comparison"
#endif

QString HashString(std::shared_ptr<ICryptoHash>& hashProv,
                   const QString                       & inStr) {
  std::vector<uint8_t> res;

  res.resize(hashProv->GetOutputSize());
  uint32_t resSize = static_cast<uint32_t>(res.size());
  auto     strCopy = inStr.toStdString();

  hashProv->Hash(reinterpret_cast<const uint8_t *>(strCopy.data()),
                 static_cast<uint32_t>(strCopy.size()), &res[0], resSize);

  // convert to hex
  QByteArray arr(
    reinterpret_cast<const char *>(res.data()), static_cast<int>(res.size()));
  QString resStr(arr.toHex());

  return resStr;
}

QString TransformData(bool                    encrypt,
                      CryptoAlgorithm algo,
                      const QString         & iv,
                      const QString         & key,
                      const QString         & inData) {
  QByteArray keyArray =
    QByteArray::fromHex(key.toUtf8());
  QByteArray ivArray =
    QByteArray::fromHex(iv.toUtf8());
  QByteArray dataArray =
    QByteArray::fromHex(inData.toUtf8());

  std::vector<uint8_t> res;

  res.resize(dataArray.size() + 64);
  uint32_t resSize = static_cast<uint32_t>(res.size());

  auto pcrypto = CreateCryptoEngine();
  auto aesEcb  =
    pcrypto->CreateKey(
      reinterpret_cast<const uint8_t *>(keyArray.data()),
      static_cast<uint32_t>(keyArray.size()), algo);

  if (encrypt) {
    aesEcb->Encrypt(reinterpret_cast<const uint8_t *>(dataArray.data()),
                    static_cast<uint32_t>(dataArray.size()), &res[0], resSize,
                    reinterpret_cast<const uint8_t *>(ivArray.data()),
                    static_cast<uint32_t>(ivArray.size()));
  } else {
    aesEcb->Decrypt(reinterpret_cast<const uint8_t *>(dataArray.data()),
                    static_cast<uint32_t>(dataArray.size()), &res[0], resSize,
                    reinterpret_cast<const uint8_t *>(ivArray.data()),
                    static_cast<uint32_t>(ivArray.size()));
  }

  // convert to hex
  QByteArray arr(
    reinterpret_cast<const char *>(res.data()), static_cast<int>(resSize));
  QString resStr(arr.toHex());

  return resStr;
}

PlatformCryptoTest::PlatformCryptoTest()
{}

PlatformCryptoTest::~PlatformCryptoTest()
{}

void PlatformCryptoTest::testSHA_data() {
  // SHA data
  QTest::addColumn<QString>("data");
  QTest::addColumn<QString>("result_sha1");
  QTest::addColumn<QString>("result_sha256");

  QTest::newRow("Test1") <<
    "You disposal strongly quitting his endeavor two settling him."
                         << "beace21418f741d6b32680ecd8620a66d3ba5687"
                         <<
    "453fbf8b062ab5a96fc074edfc428e1cf97498ec5f154aa2bbbeb1aefd0f9c22";

  QTest::newRow("Test2") << "Manners ham him hearted hundred expense."
                         << "2eab5bf8dd1fe4c92f1318f6faf4f7005cc4e567"
                         <<
    "e4e4325eb9de4083c84a6adb7f7d231c0d73e892552760eb064771caf4507452";
}

void PlatformCryptoTest::testSHA()
{
  auto pcrypto    = CreateCryptoEngine();
  auto hashSha1   = pcrypto->CreateHash(CRYPTO_HASH_ALGORITHM_SHA1);
  auto hashSha256 = pcrypto->CreateHash(CRYPTO_HASH_ALGORITHM_SHA256);

  QFETCH(QString, data);
  QFETCH(QString, result_sha1);
  QFETCH(QString, result_sha256);

  QCOMPARE(HashString(hashSha1, data),   result_sha1);
  QCOMPARE(HashString(hashSha256, data), result_sha256);
}

void PlatformCryptoTest::testAESECB_data() {
  // AES_ECB data
  QTest::addColumn<QString>("key");
  QTest::addColumn<QString>("data");
  QTest::addColumn<QString>("result");

  QTest::newRow("Test1") << "7bddca3c7ff9bdc8eea7a8eb63d01f11"
                         <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4d1619e"
                         <<
    "ce3d1036732f9f972376abf20dda87e0323a33637ed1ad1059f1531417b4c8927042ba3cbfafabbbeefba2e527777efc47fae5642a5dbb3da5abbacc7353c51d";

  QTest::newRow("Test2") << "01c15b1521add51f2e6f5df073c9e8cb"
                         <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4d1619e"
                         <<
    "4de93d1165fc0fc958cb36b7016ca9166be18b64872f5a9230920f842b07487acda0173cfa8c13f0b31d1f8f62e45169d0c84a755009bd88d8056d583dded294";
}

void PlatformCryptoTest::testAESECB()
{
  QFETCH(QString, key);
  QFETCH(QString, data);
  QFETCH(QString, result);

  QCOMPARE(TransformData(true,
                         CRYPTO_ALGORITHM_AES_ECB, QString(), key,
                         data),   result);
  QCOMPARE(TransformData(false,
                         CRYPTO_ALGORITHM_AES_ECB, QString(), key,
                         result), data);
}

void PlatformCryptoTest::testAESECB_Bad_data() {
  // AES_ECB Bad data
  QTest::addColumn<QString>("key");
  QTest::addColumn<QString>("data");
  QTest::addColumn<QString>("result");

  QTest::newRow("BadTest1") << "01c15b1521add51f2e6f5df073c9e8"
                            <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4d1619e"
                            <<
    "4de93d1165fc0fc958cb36b7016ca9166be18b64872f5a9230920f842b07487acda0173cfa8c13f0b31d1f8f62e45169d0c84a755009bd88d8056d583dded294";
}

void PlatformCryptoTest::testAESECB_Bad()
{
  QFETCH(QString, key);
  QFETCH(QString, data);
  QFETCH(QString, result);

  QVERIFY_THROW(TransformData(true,
                              CRYPTO_ALGORITHM_AES_ECB, QString(), key,
                              data) != result, std::exception);
  QVERIFY_THROW(TransformData(false,
                              CRYPTO_ALGORITHM_AES_ECB, QString(), key,
                              result) != data, std::exception);
}

void PlatformCryptoTest::testAESCBC_data() {
  // AES_ECB data
  QTest::addColumn<QString>("key");
  QTest::addColumn<QString>("iv");
  QTest::addColumn<QString>("data");
  QTest::addColumn<QString>("result");

  QTest::newRow("Test1") << "7bddca3c7ff9bdc8eea7a8eb63d01f11"
                         << "c506d1092830c5a3a3db0792589f2dba"
                         <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4d1619e"
                         <<
    "c1c5345e988975bf2ce28395f5af450758993b51e593a074fab737736191e4f81e06686f2ee839e0c1a62f98eac5754f7006bee30631ab1c113180238ef008d8";

  QTest::newRow("Test2") << "01c15b1521add51f2e6f5df073c9e8cb"
                         << "c506d1092830c5a3a3db0792589f2dba"
                         <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4d1619e"
                         <<
    "1f9cf5f21cb44f96c32848038b138379c6647100740afa8eb9c5bfd76c724d504f067d28c5d5bbcccad95b2b4f60e663210b58e8514a5c0a383ea211ff44136a";
}

void PlatformCryptoTest::testAESCBC()
{
  QFETCH(QString, key);
  QFETCH(QString, iv);
  QFETCH(QString, data);
  QFETCH(QString, result);

  QCOMPARE(TransformData(true, CRYPTO_ALGORITHM_AES_CBC, iv, key,
                         data),   result);
  QCOMPARE(TransformData(false, CRYPTO_ALGORITHM_AES_CBC, iv, key,
                         result), data);
}

void PlatformCryptoTest::testAESCBC_Bad_data() {
  // AES_ECB Bad data
  QTest::addColumn<QString>("key");
  QTest::addColumn<QString>("iv");
  QTest::addColumn<QString>("data");
  QTest::addColumn<QString>("result");

  QTest::newRow("BadTest1") << "7bddca3c7ff9bdc8eea7a8eb63d01f"
                            << "c506d1092830c5a3a3db0792589f2dba"
                            <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4d1619e"
                            <<
    "c1c5345e988975bf2ce28395f5af450758993b51e593a074fab737736191e4f81e06686f2ee839e0c1a62f98eac5754f7006bee30631ab1c113180238ef008d8";

  QTest::newRow("BadTest2") << "01c15b1521add51f2e6f5df073c9e8cb"
                            << "c506d1092830c5a3a3db0792589f2d"
                            <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4d1619e"
                            <<
    "1f9cf5f21cb44f96c32848038b138379c6647100740afa8eb9c5bfd76c724d504f067d28c5d5bbcccad95b2b4f60e663210b58e8514a5c0a383ea211ff44136a";
}

void PlatformCryptoTest::testAESCBC_Bad()
{
  QFETCH(QString, key);
  QFETCH(QString, iv);
  QFETCH(QString, data);
  QFETCH(QString, result);

  QVERIFY_THROW(TransformData(true,
                              CRYPTO_ALGORITHM_AES_CBC, iv, key,
                              data) != result, std::exception);
  QVERIFY_THROW(TransformData(false,
                              CRYPTO_ALGORITHM_AES_CBC, iv, key,
                              result) != data, std::exception);
}

void PlatformCryptoTest::testAESCBCPKCS7_data() {
  // AES_ECB PKCS7 data
  QTest::addColumn<QString>("key");
  QTest::addColumn<QString>("iv");
  QTest::addColumn<QString>("data");
  QTest::addColumn<QString>("result");

  QTest::newRow("Test1") << "7bddca3c7ff9bdc8eea7a8eb63d01f11"
                         << "c506d1092830c5a3a3db0792589f2dba"
                         <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4"
                         <<
    "c1c5345e988975bf2ce28395f5af450758993b51e593a074fab737736191e4f81e06686f2ee839e0c1a62f98eac5754f8e06bd5cbbbea04e2db99ca495394f08";

  QTest::newRow("Test2") << "01c15b1521add51f2e6f5df073c9e8cb"
                         << "c506d1092830c5a3a3db0792589f2dba"
                         <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4"
                         <<
    "1f9cf5f21cb44f96c32848038b138379c6647100740afa8eb9c5bfd76c724d504f067d28c5d5bbcccad95b2b4f60e6638b528feb4251506a995df5e6313a45e2";
}

void PlatformCryptoTest::testAESCBCPKCS7() {
  QFETCH(QString, key);
  QFETCH(QString, iv);
  QFETCH(QString, data);
  QFETCH(QString, result);

  QCOMPARE(TransformData(true, CRYPTO_ALGORITHM_AES_CBC_PKCS7, iv, key,
                         data),   result);
  QCOMPARE(TransformData(false, CRYPTO_ALGORITHM_AES_CBC_PKCS7, iv, key,
                         result), data);
}

void PlatformCryptoTest::testAESCBCPKCS7_Bad_data() {
  // AES_ECB PKCS7 data
  QTest::addColumn<QString>("key");
  QTest::addColumn<QString>("iv");
  QTest::addColumn<QString>("data");
  QTest::addColumn<QString>("result");

  QTest::newRow("BadTest1") << "7bddca3c7ff9bdc8eea7a8eb63d01f"
                            << "c506d1092830c5a3a3db0792589f2dba"
                            <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4"
                            <<
    "c1c5345e988975bf2ce28395f5af450758993b51e593a074fab737736191e4f81e06686f2ee839e0c1a62f98eac5754f8e06bd5cbbbea04e2db99ca495394f08";

  QTest::newRow("BadTest2") << "01c15b1521add51f2e6f5df073c9e8cb"
                            << "c506d1092830c5a3a3db0792589f2d"
                            <<
    "5b87acdac4539856a91cb8de24e0b96e1ad2a7bfcd5f77456fba98ff9786f4131ed96b20585d5d930f9bdf6234e64fd0c76a7fef01b1c7abaa9b1cd1a4"
                            <<
    "1f9cf5f21cb44f96c32848038b138379c6647100740afa8eb9c5bfd76c724d504f067d28c5d5bbcccad95b2b4f60e6638b528feb4251506a995df5e6313a45e2";
}

void PlatformCryptoTest::testAESCBCPKCS7_Bad() {
  QFETCH(QString, key);
  QFETCH(QString, iv);
  QFETCH(QString, data);
  QFETCH(QString, result);

  QVERIFY_THROW(TransformData(true,
                              CRYPTO_ALGORITHM_AES_CBC_PKCS7, iv, key,
                              data) != result, std::exception);
  QVERIFY_THROW(TransformData(false,
                              CRYPTO_ALGORITHM_AES_CBC_PKCS7, iv, key,
                              result) != data, std::exception);
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
} // namespace test
} // namespace rmscore
