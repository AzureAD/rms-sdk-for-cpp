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
// #include "include/gtest/gtest.h"
// 
// class IsPrime : public::testing::TestWithParam<int>
// {
// 
// };
// 
// TEST_P(IsPrime, checkPrime){
//     EXPECT_TRUE(true);
// }
// 
// INSTANTIATE_TEST_CASE_P(TrueReturn, IsPrime, testing::Values(1,3));

 int main(int argc, char *argv[])
 {
     QCoreApplication app(argc, argv);
//    testing::InitGoogleTest(&argc, argv);
//    RUN_ALL_TESTS();

    int res = 0;
    res += QTest::qExec(new PlatformHttpClientTest(), argc, argv);
    res += QTest::qExec(new PlatformXmlTest(), argc, argv);
    res += QTest::qExec(new PlatformJsonObjectTest(), argc, argv);
    res += QTest::qExec(new PlatformJsonArrayTest(), argc, argv);
    res += QTest::qExec(new PlatformFileSystemTest(), argc, argv);
    res += QTest::qExec(new PlatformFileTest(), argc, argv);

    return res;
}
