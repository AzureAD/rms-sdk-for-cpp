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

    QTest::qExec(new CryptoAPITests(), argc, argv);
    QTest::qExec(new KeyStorageTests(), argc, argv);
    QTest::qExec(new CryptedStreamTests(), argc, argv);

    return 0;
}
