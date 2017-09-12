/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include<QApplication>
#include "KeyStorageTests.h"
#include "CryptedStreamTests.h"
#include "CryptoAPITests.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int res = 0;

    res += QTest::qExec(new CryptoAPITests(), argc, argv);
    res += QTest::qExec(new KeyStorageTests(), argc, argv);
    res += QTest::qExec(new CryptedStreamTests(), argc, argv);

    return res;
}
