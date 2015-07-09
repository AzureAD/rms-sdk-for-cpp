/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PlatformJsonObjectTest.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Json/IJsonParser.h"
#include "../../Platform/Json/IJsonObject.h"
#include "../../Platform/Json/IJsonArray.h"
#include "../../ModernAPI/RMSExceptions.h"

using namespace rmscore::platform::json;
using namespace rmscore::platform::logger;
using namespace rmscore;

static const char *s_strDefaultVal = "someDefaultValue";
static const char *STR_EXCEPTION   = "WHEN STR EXCEPTION SHOULD HAPPEN";

static const bool s_boolDefaultVal = false;
static const bool BOOL_EXCEPTION   = false;

void PlatformJsonObjectTest::testGetNamedString_data()
{
  QTest::addColumn<QString>("jsonAsString");
  QTest::addColumn<QString>("name");
  QTest::addColumn<QString>("expectedValue");

  QString path1 = QString(SRCDIR) + "data/testJson1.json";
  QFile   file1(path1);

  QVERIFY(file1.open(QIODevice::ReadOnly | QIODevice::Text));
  auto jsonAsString1 = QString(file1.readAll());

  QTest::newRow("encoding")
    << jsonAsString1
    << "encoding"
    << "UTF-8";

  QTest::newRow("array key")
    << jsonAsString1
    << "plug-ins"
    << STR_EXCEPTION;

  QTest::newRow("object key")
    << jsonAsString1
    << "indent"
    << STR_EXCEPTION;

  QTest::newRow("default val")
    << jsonAsString1
    << "some undefined key"
    << s_strDefaultVal;

  QTest::newRow("str-int value")
    << jsonAsString1
    << "str-int-value"
    << "158";
}

void PlatformJsonObjectTest::testGetNamedString(bool enabled)
{
  if (!enabled) return;

  QFETCH(QString, jsonAsString);
  QFETCH(QString, name);
  QFETCH(QString, expectedValue);

  auto jsonArr = jsonAsString.toUtf8();

  auto pparser   = IJsonParser::Create();
  auto p_rootObj =
    pparser->Parse(rmscore::common::ByteArray(jsonArr.begin(),
                                             jsonArr.end()));
  QVERIFY(p_rootObj != nullptr);

  std::string value;
  try {
    value = p_rootObj->GetNamedString(name.toStdString(), s_strDefaultVal);
  }
  catch (exceptions::RMSInvalidArgumentException& e) {
    Logger::Hidden("Convertion exception: %s", e.what());
    value = STR_EXCEPTION;
  }
  QVERIFY(expectedValue.toStdString() == value);
}

void PlatformJsonObjectTest::testGetNamedBool_data()
{
  QTest::addColumn<QString>("jsonAsString");
  QTest::addColumn<QString>("name");
  QTest::addColumn<bool>(   "expectedValue");

  QString path1 = QString(SRCDIR) + "data/testJson1.json";
  QFile   file1(path1);

  QVERIFY(file1.open(QIODevice::ReadOnly | QIODevice::Text));
  auto jsonAsString1 = QString(file1.readAll());

  QTest::newRow("encoding")
    << jsonAsString1
    << "bool-value"
    << true;

  QTest::newRow("array key")
    << jsonAsString1
    << "plug-ins"
    << BOOL_EXCEPTION;

  QTest::newRow("object key")
    << jsonAsString1
    << "indent"
    << BOOL_EXCEPTION;

  QTest::newRow("default val")
    << jsonAsString1
    << "some undefined key"
    << s_boolDefaultVal;

  QTest::newRow("int value")
    << jsonAsString1
    << "int-value"
    << BOOL_EXCEPTION;
}

void PlatformJsonObjectTest::testGetNamedBool(bool enabled)
{
  if (!enabled) return;

  QFETCH(QString, jsonAsString);
  QFETCH(QString, name);
  QFETCH(bool,    expectedValue);

  auto jsonArr = jsonAsString.toUtf8();
  auto pparser   = IJsonParser::Create();
  auto p_rootObj = pparser->Parse(rmscore::common::ByteArray(jsonArr.begin(),
                                                            jsonArr.end()));
  QVERIFY(p_rootObj != nullptr);

  try {
    auto value = p_rootObj->GetNamedBool(name.toStdString(), s_boolDefaultVal);
    QVERIFY(expectedValue == value);
  }
  catch (exceptions::RMSInvalidArgumentException& e) {
    Logger::Hidden("Convertion exception: %s", e.what());
    QVERIFY(std::string(e.what()) ==
            std::string("JsonObjectQt::GetNamedBool: convertion error"));
  }
}

void PlatformJsonObjectTest::testGetNamedObject_data()
{
  QTest::addColumn<QString>("jsonAsString");
  QTest::addColumn<QString>("name");
  QTest::addColumn<bool>(   "expectedExsists");

  QString path1 = QString(SRCDIR) + "data/testJson1.json";
  QFile   file1(path1);

  QVERIFY(file1.open(QIODevice::ReadOnly | QIODevice::Text));
  auto jsonAsString1 = QString(file1.readAll());

  QTest::newRow("exists")
    << jsonAsString1
    << "obj2"
    << true;

  QTest::newRow("not exists")
    << jsonAsString1
    << "obj233"
    << false;

  QTest::newRow("exists")
    << jsonAsString1
    << "obj1"
    << true;
}

void PlatformJsonObjectTest::testGetNamedObject(bool enabled)
{
  if (!enabled) return;

  QFETCH(QString, jsonAsString);
  QFETCH(QString, name);
  QFETCH(bool,    expectedExsists);

  auto jsonArr = jsonAsString.toUtf8();

  auto pparser   = IJsonParser::Create();
  auto p_rootObj = pparser->Parse(rmscore::common::ByteArray(jsonArr.begin(),
                                                            jsonArr.end()));
  QVERIFY(p_rootObj != nullptr);

  auto p_namedObj = p_rootObj->GetNamedObject(name.toStdString());
  bool exsists    = p_namedObj != nullptr;

  if (exsists)
  {
    try
    {
      Logger::Hidden("val: %s", p_namedObj->GetNamedString("str-value").data());
      Logger::Hidden("val: %i", p_namedObj->GetNamedBool("bool-value"));
    }
    catch (exceptions::RMSInvalidArgumentException& e)
    {
      auto res = std::string(e.what());
    }
  }
  QVERIFY(exsists == expectedExsists);
}

void PlatformJsonObjectTest::testStringify_data()
{
  QTest::addColumn<QString>("jsonAsString");

  QString path1 = QString(SRCDIR) + "data/testJson1.json";
  QFile   file1(path1);

  QVERIFY(file1.open(QIODevice::ReadOnly | QIODevice::Text));
  auto jsonAsString1 = QString(file1.readAll());

  QTest::newRow("root obj")
    << jsonAsString1;
}

void PlatformJsonObjectTest::testStringify()
{
  QFETCH(QString, jsonAsString);

  auto jsonArr = jsonAsString.toUtf8();
  auto pJsonParser = IJsonParser::Create();
  auto pJsonObject = pJsonParser->Parse(rmscore::common::ByteArray(jsonArr.begin(), jsonArr.end()));
  QVERIFY(pJsonObject != nullptr);
  auto stringified = pJsonObject->Stringify();
  QByteArray actualRes((const char *)stringified.data(), (int)stringified.size());
  auto expectedRes = QJsonDocument::fromJson(jsonAsString.toUtf8()).toJson(QJsonDocument::Compact);

  QCOMPARE(actualRes, expectedRes);
}

void PlatformJsonObjectTest::testSetNamedString()
{
  auto pJsonObject = IJsonObject::Create();
  pJsonObject->SetNamedString("myString", "MyStringValue");
  auto stringified = pJsonObject->Stringify();
  QByteArray actualRes((const char *)stringified.data(), (int)stringified.size());
  QString jsonAsString = "{\"myString\":\"MyStringValue\"}";
  auto expectedRes = QJsonDocument::fromJson(jsonAsString.toUtf8()).toJson(QJsonDocument::Compact);

  QCOMPARE(actualRes, expectedRes);
}

void PlatformJsonObjectTest::testSetNamedObject()
{
  auto pJsonObject = IJsonObject::Create();
  pJsonObject->SetNamedString("myString", "MyStringValue");
  auto pJsonObject1 = IJsonObject::Create();
  pJsonObject1->SetNamedObject("myObject", *pJsonObject);
  auto stringified = pJsonObject1->Stringify();
  QByteArray actualRes((const char *)stringified.data(), (int)stringified.size());
  QString jsonAsString = "{\"myObject\":{\"myString\":\"MyStringValue\"}}";
  auto expectedRes = QJsonDocument::fromJson(jsonAsString.toUtf8()).toJson(QJsonDocument::Compact);

  QCOMPARE(actualRes, expectedRes);
}

void PlatformJsonObjectTest::testSetNamedArray()
{
  auto pJsonArray = IJsonArray::Create();
  pJsonArray->Append("string1");
  pJsonArray->Append("string2");
  auto pJsonObject = IJsonObject::Create();
  pJsonObject->SetNamedArray("myArray", *pJsonArray);
  auto stringified = pJsonObject->Stringify();
  QByteArray actualRes((const char *)stringified.data(), (int)stringified.size());
  QString jsonAsString = "{\"myArray\":[\"string1\",\"string2\"]}";
  auto expectedRes = QJsonDocument::fromJson(jsonAsString.toUtf8()).toJson(QJsonDocument::Compact);

  QCOMPARE(actualRes, expectedRes);
}
