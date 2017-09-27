/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <string>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Json/IJsonParser.h"
#include "../../Platform/Json/IJsonObject.h"
#include "../../Platform/Json/IJsonArray.h"

using namespace rmscore::platform::json;
using namespace rmscore::platform::logger;
using std::pair;
using std::string;
using std::vector;

#define TestData vector<pair<int, string>>

TEST (PlatformJsonArrayTest, testGetStringAt) {

  TestData test_data {
    std::make_pair(0, "python"),
    std::make_pair(1, "c++"),
    std::make_pair(2, "ruby")
  };

  auto pparser = IJsonParser::Create();

  // jsonAsString is ["python","c++","ruby"]
  rmscore::common::ByteArray jsonAsString {
    0x5B, 0x22, 0x70, 0x79, 0x74, 0x68, 0x6F, 0x6E, 0x22, 0x2C, 0x22, 0x63, 0x2B, 0x2B, 0x22,
    0x2C, 0x22, 0x72, 0x75, 0x62, 0x79, 0x22, 0x5D
  };
  auto p_arr   =
    pparser->ParseArray(jsonAsString);

  EXPECT_NE(p_arr, nullptr);
  for(TestData::iterator it = test_data.begin(); it != test_data.end(); it++) {
    try
    {
      EXPECT_EQ(p_arr->GetStringAt(it->first), it->second);
    }
    catch (std::invalid_argument& e)
    {
      Logger::Error("Exception: %s", e.what());
    }
  }
  
}

// TEST (PlatformJsonArrayTest, testGetObjectAt) {
//    EXPECT_TRUE(false) << "Not implemented";
// }

TEST (PlatformJsonArrayTest, testAppendObject) {
  auto pJsonObject1 = IJsonObject::Create();

  pJsonObject1->SetNamedString("myString", "MyStringValue");
  auto pJsonObject2 = IJsonObject::Create();
  pJsonObject2->SetNamedBool("myBool", true);
  pJsonObject2->SetNamedNumber("myNum", 1515);

  auto pJsonArray = IJsonArray::Create();
  pJsonArray->Append(*pJsonObject1);
  pJsonArray->Append(*pJsonObject2);
  rmscore::common::ByteArray stringified = pJsonArray->Stringify();

  // expectedRes is [{"myString":"MyStringValue"},{"myBool":true,"myNum":1515}]
  rmscore::common::ByteArray expectedRes {
      0x5B, 0x7B, 0x22, 0x6D, 0x79, 0x53, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x22, 0x3A, 0x22,
      0x4D, 0x79, 0x53, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x56, 0x61, 0x6C, 0x75, 0x65, 0x22,
      0x7D, 0x2C, 0x7B, 0x22, 0x6D, 0x79, 0x42, 0x6F, 0x6F, 0x6C, 0x22, 0x3A, 0x74, 0x72,
      0x75, 0x65, 0x2C, 0x22, 0x6D, 0x79, 0x4E, 0x75, 0x6D, 0x22, 0x3A, 0x31, 0x35, 0x31,
      0x35, 0x7D, 0x5D
  };
  EXPECT_THAT(stringified, testing::ContainerEq(expectedRes));
}

TEST (PlatformJsonArrayTest, testAppendString) {
  auto pJsonArray = IJsonArray::Create();

  pJsonArray->Append("string1");
  pJsonArray->Append("string2");
  rmscore::common::ByteArray stringified = pJsonArray->Stringify();
  
  // expectedRes is ["string1","string2"]
  rmscore::common::ByteArray expectedRes { 
      0x5B, 0x22, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x31, 0x22, 0x2C, 0x22, 0x73, 0x74, 0x72,
      0x69, 0x6E, 0x67, 0x32, 0x22, 0x5D
  };
  EXPECT_THAT(stringified, testing::ContainerEq(expectedRes));
}
