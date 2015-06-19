/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include<QCoreApplication>
#include"PlatformHttpClientTest.h"
#include"PlatformJsonArrayTest.h"
#include"PlatformXmlTest.h"
#include"PlatformJsonObjectTest.h"
#include"PlatformJsonArrayTest.h"
#include"PlatformFileTest.h"
#include"PlatformFileSystemTest.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

//    QTest::qExec(new PlatformHttpClientTest(), argc, argv);
//    QTest::qExec(new PlatformCryptoTest(), argc, argv);
//    QTest::qExec(new PlatformXmlTest(), argc, argv);
    QTest::qExec(new PlatformJsonObjectTest(), argc, argv);
    QTest::qExec(new PlatformJsonArrayTest(), argc, argv);
//    QTest::qExec(new PlatformFileSystemTest(), argc, argv);
//    QTest::qExec(new PlatformFileTest(), argc, argv);

    return 0;
}
