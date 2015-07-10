/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PlatformJsonArrayTest.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Json/IJsonParser.h"
#include "../../Platform/Json/IJsonObject.h"
#include "../../Platform/Json/IJsonArray.h"

using namespace rmscore::platform::json;
using namespace rmscore::platform::logger;

void PlatformJsonArrayTest::testGetStringAt_data()
{
  QTest::addColumn<QString>("jsonAsString");
  QTest::addColumn<int>(    "index");
  QTest::addColumn<QString>("expectedValue");

  auto jsonAsString1 = "[\"python\",\"c++\",\"ruby\"]";

  QTest::newRow("0")
    << jsonAsString1
    << 0
    << "python";

  QTest::newRow("1")
    << jsonAsString1
    << 1
    << "c++";

  QTest::newRow("2")
    << jsonAsString1
    << 2
    << "ruby";
}

void PlatformJsonArrayTest::testGetStringAt()
{
  QFETCH(QString, jsonAsString);
  QFETCH(int,     index);
  QFETCH(QString, expectedValue);

  auto jsonArr = jsonAsString.toUtf8();

  auto pparser = IJsonParser::Create();
  auto p_arr   =
    pparser->ParseArray(rmscore::common::ByteArray(jsonArr.begin(),
                                                  jsonArr.end()));
  QVERIFY(p_arr != nullptr);

  try
  {
    QVERIFY(p_arr->GetStringAt(index) == expectedValue.toStdString());
  }
  catch (std::invalid_argument& e)
  {
    Logger::Error("Exception: %s", e.what());
  }
}

// void  PlatformJsonArrayTest::testGetObjectAt()
// {
//    QVERIFY2(false, "Not implemented");
// }
// void  PlatformJsonArrayTest::testGetObjectAt_data()
// {
//    QVERIFY2(false, "Not implemented");
// }

void PlatformJsonArrayTest::testAppendObject()
{
  auto pJsonObject1 = IJsonObject::Create();

  pJsonObject1->SetNamedString("myString", "MyStringValue");
  auto pJsonObject2 = IJsonObject::Create();
  pJsonObject2->SetNamedBool("myBool", true);
  pJsonObject2->SetNamedNumber("myNum", 1515);

  auto pJsonArray = IJsonArray::Create();
  pJsonArray->Append(*pJsonObject1);
  pJsonArray->Append(*pJsonObject2);
  rmscore::common::ByteArray stringified = pJsonArray->Stringify();
  QByteArray actualRes((const char *)stringified.data(), (int)stringified.size());
  QString jsonAsString = "[{\"myString\":\"MyStringValue\"},{\"myBool\":true,\"myNum\":1515}]";
  auto expectedRes = QJsonDocument::fromJson(jsonAsString.toUtf8()).toJson(QJsonDocument::Compact);
  QCOMPARE(actualRes, expectedRes);
}

void PlatformJsonArrayTest::testAppendString()
{
  auto pJsonArray = IJsonArray::Create();

  pJsonArray->Append("string1");
  pJsonArray->Append("string2");
  rmscore::common::ByteArray stringified = pJsonArray->Stringify();
  QByteArray actualRes((const char *)stringified.data(), (int)stringified.size());
  QString jsonAsString = "[\"string1\",\"string2\"]";
  auto expectedRes = QJsonDocument::fromJson(jsonAsString.toUtf8()).toJson(QJsonDocument::Compact);
  QCOMPARE(actualRes, expectedRes);
}
