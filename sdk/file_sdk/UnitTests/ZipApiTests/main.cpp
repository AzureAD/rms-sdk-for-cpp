/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <QCoreApplication>
#include "test_zipapiteststest.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int res = 0;
    res += QTest::qExec(new ZipApiTestsTest(), argc, argv);

    return res;
}
