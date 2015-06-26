/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IJSONOBJECT
#define IJSONOBJECT

#include <string>
#include <vector>
#include <memory>
#include "../../Common/CommonTypes.h"
#include "../../ModernAPI/PolicyDescriptor.h"

namespace rmscore {
namespace platform {
namespace json {
using StringArray = std::vector<std::string>;

class IJsonArray;
class IJsonObject
{
public:
    virtual bool IsNull(const std::string &name) = 0;

    virtual std::shared_ptr<IJsonArray> GetArray() = 0;

    virtual bool HasName(const std::string &name) = 0;

    virtual std::string GetNamedString(const std::string &name, const std::string &defaultValue = std::string("")) = 0;
    virtual void SetNamedString(const std::string &name, const std::string &value) = 0;

    virtual bool GetNamedBool(const std::string &name, bool bDefaultValue = false) = 0;
    virtual void SetNamedBool(const std::string &name, bool bValue) = 0;

    virtual double GetNamedNumber(const std::string &name, double fDefaultValue = false) = 0;
    virtual void SetNamedNumber(const std::string &name, double fValue) = 0;

    virtual std::shared_ptr<IJsonObject> GetNamedObject(const std::string &name) = 0;
    virtual void SetNamedObject(const std::string &name, const IJsonObject &jsonObject) = 0;

    virtual std::shared_ptr<IJsonArray> GetNamedArray(const std::string &name) = 0;
    virtual void SetNamedArray(const std::string &name, const IJsonArray &jsonArray) = 0;

    virtual void SetNamedValue(const std::string &name, const common::ByteArray &value) = 0;
    virtual common::ByteArray GetNamedValue(const std::string &name) = 0;

    virtual StringArray GetNamedStringArray(const std::string &name) = 0;

    virtual modernapi::AppDataHashMap ToStringDictionary() = 0;

    virtual common::ByteArray Stringify() = 0;

public:
    static std::shared_ptr<IJsonObject> Create();
};

}}} // namespace rmscore { namespace platform { namespace json {

#endif // IJSONOBJECT
