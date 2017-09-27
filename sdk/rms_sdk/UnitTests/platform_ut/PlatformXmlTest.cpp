/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PlatformXmlTest.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Xml/IDomDocument.h"
#include "../../Platform/Xml/IDomNode.h"
#include "../../Platform/Xml/IDomElement.h"

using namespace rmscore::platform::logger;
using std::string;
using std::tuple;
using std::vector;

#define TestData vector<tuple<string, string, string>>

TEST_P (PlatformXmlTest, testSelectSingleNode) {
  bool enabled = GetParam();
  if (!enabled) return;

  string path1 = string(SRCDIR) + "data/testXPath1.xml";
  std::ifstream file1(path1, std::ifstream::in);
  EXPECT_TRUE(file1.is_open());
  std::stringstream ss;
  ss << file1.rdbuf();
  auto file1AsString = ss.str();

  ss.str(std::string());
  ss.clear();
  string path2 = string(SRCDIR) + "data/testXPath2.xml";
  std::ifstream file2(path2, std::ifstream::in);
  EXPECT_TRUE(file2.is_open());
  ss << file2.rdbuf();
  auto file2AsString = ss.str();

  TestData test_data { 
      std::make_tuple (
          file1AsString, 
          "kml/Document/Placemark[last()]/GeometryCollection/LineString/coordinates", 
          "0.000010,0.000020,0.000030"
        ),
      std::make_tuple (
          file2AsString,
          "bookstore/book/author[last-name = \"Bob\" and first-name = \"Joe\"]/award",
          "Trenton Literary Review Honorable Mention"
      )
  };
  
  auto doc = IDomDocument::create();
  std::string errorMsg;
  int errorLine = 0;
  int errorColumn = 0;

  for(TestData::iterator it = test_data.begin(); it != test_data.end(); it++) {
    auto ok = doc->setContent(std::get<0>(*it), errorMsg, errorLine, errorColumn);
    EXPECT_TRUE(ok) << errorMsg.data();

    auto pnode = doc->SelectSingleNode(std::get<1>(*it));
    auto realResult = pnode->toElement()->text();
    Logger::Hidden("expc: %s", std::get<2>(*it).data());
    Logger::Hidden("real: %s", realResult.data());
    EXPECT_EQ(realResult, std::get<2>(*it));
  }
}
