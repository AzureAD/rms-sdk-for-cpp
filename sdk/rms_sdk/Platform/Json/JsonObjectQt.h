/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef IJSONOBJECTQTIMPL
#define IJSONOBJECTQTIMPL

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

#include "IJsonObject.h"

namespace rmscore {
namespace platform {
namespace json {
class IJsonArray;
class JsonObjectQt : public IJsonObject {
private:

  virtual bool                       IsNull(const std::string& name) override;

  virtual std::shared_ptr<IJsonArray>GetArray() override;

  virtual bool                       HasName(const std::string& name) override;

  virtual std::string                GetNamedString(const std::string& name,
                                                    const std::string& defaultValue)
  override;
  virtual void                       SetNamedString(const std::string& name,
                                                    const std::string& value)
  override;

  virtual bool GetNamedBool(const std::string& name,
                            bool               bDefaultValue =
                              false) override;
  virtual void SetNamedBool(const std::string& name,
                            bool               bValue)
  override;

  virtual double GetNamedNumber(const std::string& name,
                                double             fDefaultValue =
                                  false) override;
  virtual void   SetNamedNumber(const std::string& name,
                                double             fValue)
  override;

  virtual std::shared_ptr<IJsonObject>GetNamedObject(const std::string& name)
  override;
  virtual void                        SetNamedObject(const std::string& name,
                                                     const IJsonObject& jsonObject)
  override;

  virtual std::shared_ptr<IJsonArray>GetNamedArray(const std::string& name)
  override;
  virtual void                       SetNamedArray(const std::string& name,
                                                   const IJsonArray & jsonArray)
  override;

  virtual void                      SetNamedValue(const std::string      & name,
                                                  const common::ByteArray& value)
  override;
  virtual common::ByteArray         GetNamedValue(const std::string& name)
  override;

  virtual StringArray               GetNamedStringArray(const std::string& name)
  override;

  virtual modernapi::AppDataHashMap ToStringDictionary() override;

  virtual common::ByteArray         Stringify() override;

private:

  QJsonValue impl_;

public:

  JsonObjectQt() {}

  JsonObjectQt(const QJsonValue& impl) : impl_(impl) {}

  const QJsonValue& impl() {
    return this->impl_;
  }
};
}
}
} // namespace rmscore { namespace platform { namespace json {

#endif // IJSONOBJECTQTIMPL
