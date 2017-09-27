/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include<QCoreApplication>
#include "gtest/gtest.h"
// #include"PlatformUriTest.h"
#include"PlatformHttpClientTest.h"
// #include"PlatformDnsTest.h"
#include"PlatformXmlTest.h"
#include"PlatformJsonObjectTest.h"

using namespace std;
using std::vector;
using std::string;
using ::testing::TestWithParam;
using ::testing::Bool;
using ::testing::Values;
using ::testing::Combine;

bool pHCT = true;
INSTANTIATE_TEST_CASE_P(Enabled, PlatformHttpClientTest, testing::Values(pHCT));

bool pJOTEnabled = true;
INSTANTIATE_TEST_CASE_P(Enabled, PlatformJsonObjectTest, testing::Values(pJOTEnabled));

bool pXTEnabled = true;
INSTANTIATE_TEST_CASE_P(XmlAsString, PlatformXmlTest, testing::Values(pXTEnabled));

 int main(int argc, char *argv[])
 {
     QCoreApplication app(argc, argv);

     testing::InitGoogleTest(&argc, argv);
     RUN_ALL_TESTS();
}
