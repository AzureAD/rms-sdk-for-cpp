/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IJSONARRAYQTIMPL
#define IJSONARRAYQTIMPL

#include <QJsonArray>
#include <QJsonValue>

#include "IJsonArray.h"
#include "IJsonObject.h"
#include "JsonObjectQt.h"

namespace rmslib { namespace platform { namespace json {

class JsonArrayQt : public IJsonArray
{
private:

    virtual uint32_t Size() override;

    virtual const std::string GetStringAt(uint32_t index) override;
    virtual std::shared_ptr<IJsonObject> GetObjectAt(uint32_t index) override;

    virtual void Clear() override;

    virtual void Append(const IJsonObject& jsonObject) override;
    virtual void Append(const std::string& name) override;

private:
    QJsonArray impl_;

public:
    JsonArrayQt(){}
    JsonArrayQt(const QJsonArray& impl):impl_(impl){}
    const QJsonArray& impl(){return this->impl_;}

};

}}} // namespace rmslib { namespace platform { namespace json {
#endif // IJSONARRAYQTIMPL

