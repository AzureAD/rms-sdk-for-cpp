/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include "JsonParserQt.h"
#include "JsonObjectQt.h"
#include "JsonArrayQt.h"
#include "../Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace platform {
namespace json {
std::shared_ptr<IJsonParser>IJsonParser::Create()
{
  return std::make_shared<JsonParserQt>();
}

std::shared_ptr<IJsonObject>JsonParserQt::Parse(
  const common::ByteArray& jsonObject)
{
  QJsonParseError error;
  auto data = jsonObject.data();
  QByteArray inArray(reinterpret_cast<const char *>(data),
                     static_cast<int>(jsonObject.size()));
  auto qdoc = QJsonDocument::fromJson(inArray, &error);

  if (error.error != QJsonParseError::NoError)
  {
    Logger::Error("JsonParserQt::Parse: %s",
                  error.errorString().toStdString().data());
    return nullptr;
  }

  if (!qdoc.isObject())
  {
    Logger::Error("JsonParserQt::Parse: %s", "given json is not a json object");
    return nullptr;
  }
  return std::make_shared<JsonObjectQt>(QJsonValue(qdoc.object()));
}

std::shared_ptr<IJsonArray>JsonParserQt::ParseArray(
  const common::ByteArray& jsonArray)
{
  QJsonParseError error;
  auto qdoc =
    QJsonDocument::fromJson(
      QByteArray(reinterpret_cast<const char *>(jsonArray.data()),
                 static_cast<int>(jsonArray.size())), &error);

  if (error.error != QJsonParseError::NoError)
  {
    Logger::Error("JsonParserQt::Parse: %s",
                  error.errorString().toStdString().data());
    return nullptr;
  }

  if (!qdoc.isArray())
  {
    Logger::Error("JsonParserQt::Parse: %s", "given json is not a json array");
    return nullptr;
  }
  return std::make_shared<JsonArrayQt>(qdoc.array());
}
}
}
} // namespace rmscore { namespace platform { namespace json {
#endif // ifdef QTFRAMEWORK
