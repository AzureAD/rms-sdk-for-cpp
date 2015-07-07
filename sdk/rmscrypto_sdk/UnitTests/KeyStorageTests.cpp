/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QString>
#include "../Platform/KeyStorage/IKeyStorage.h"
#include "../CryptoAPI/RMSCryptoExceptions.h"
#include "KeyStorageTests.h"

using namespace std;

void KeyStorageTests::KeyUsage_data()
{
  QTest::addColumn<QString>("keyWrapper");
  QTest::addColumn<QString>("key");

  QTest::newRow("0") << "TestWrapperOne" << "TestKeyOne";
  QTest::newRow("1") << "TestWrapperTwo" << "TestKeyTwo";
}

void KeyStorageTests::KeyUsage()
{
  QFETCH(QString, keyWrapper);
  QFETCH(QString, key);

  try {
    auto ks = rmscrypto::platform::keystorage::IKeyStorage::Create();

    // remove old key data
    ks->RemoveKey(keyWrapper.toStdString());

    // store key
    ks->StoreKey(keyWrapper.toStdString(), key.toStdString());

    // find key
    auto findKey = ks->LookupKey(keyWrapper.toStdString());

    QVERIFY2(findKey.get() != nullptr && findKey->compare(
               key.toStdString()) == 0, "Invalid key found!");

    // remove key data
    ks->RemoveKey(keyWrapper.toStdString());

    // find key
    findKey = ks->LookupKey(keyWrapper.toStdString());

    QVERIFY2(findKey.get() == nullptr, "Found removed key!");
  } catch (const rmscrypto::exceptions::RMSCryptoException& e) {
    QTest::qFail(e.what(), __FILE__, __LINE__);
  }
}
