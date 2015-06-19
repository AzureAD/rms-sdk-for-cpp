/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifdef QTFRAMEWORK
#include "JsonArrayQt.h"
#include "JsonObjectQt.h"
#include <QJsonDocument>

namespace rmscore { namespace platform { namespace json {

std::shared_ptr<IJsonArray> IJsonArray::Create()
{
    return std::make_shared<JsonArrayQt>();
}

uint32_t JsonArrayQt::Size()
{
    return static_cast<uint32_t>(this->impl_.size());
}

const std::string JsonArrayQt::GetStringAt(uint32_t index)
{
    QJsonValue val = this->impl_[index];
    return val.toString().toStdString();
}

std::shared_ptr<IJsonObject> JsonArrayQt::GetObjectAt(uint32_t index)
{
    QJsonValue val = this->impl_[index];
    return std::make_shared<JsonObjectQt>(val);
}

void JsonArrayQt::Clear()
{
    QJsonArray::iterator it = this->impl_.begin();
    while(it != this->impl_.end())
        it = this->impl_.erase(it);
}

void JsonArrayQt::Append(const IJsonObject& jsonObject)
{
    auto joi = static_cast<const JsonObjectQt&>(jsonObject);
    this->impl_.append(joi.impl());
}

void JsonArrayQt::Append(const std::string& name)
{
    QJsonValue val(QString::fromStdString(name));
    this->impl_.append(val);
}

common::ByteArray JsonArrayQt::Stringify()
{
    QJsonDocument doc(this->impl_);
    auto res = doc.toJson(QJsonDocument::Compact);
    return common::ByteArray(res.begin(), res.end());

}

}}} // namespace rmscore { namespace platform { namespace json {
#endif

