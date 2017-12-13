/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include<QCoreApplication>
#include"FileAPIProtectorSelectorTest.h"
#include <gtest/gtest.h>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int res = 0;
    res += QTest::qExec(new FileAPIProtectorSelectorTest(), argc, argv);

    testing::GTEST_FLAG(output) = "xml:./TestReport.xml";
    testing::GTEST_FLAG(filter) ="**.*";
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
