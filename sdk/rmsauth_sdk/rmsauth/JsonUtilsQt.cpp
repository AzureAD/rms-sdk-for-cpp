/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "JsonUtilsQt.h"
#include <Exceptions.h>
#include <Logger.h>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace rmsauth {

String JsonUtilsQt::getStringOrDefault(const QJsonObject& qobj, const String& key, const String defval)
{
    if (qobj.contains(key.data()))
    {
        auto res = qobj.value(key.data());
        if(!res.isString())
        {
            Logger::error(Tag(), "getStringOrDefault: The value for the key '%' is not a string", key);
            throw RmsauthJsonParsingException("JsonUtilsQt::getStringOrDefault", "value is not a string");
        }

        return res.toString().toStdString();
    }
    return defval;
}

int JsonUtilsQt::getIntOrDefault(const QJsonObject& qobj, const String& key, int defval)
{
    if (qobj.contains(key.data()))
    {
        auto res = qobj.value(key.data());
        if(!res.isDouble())
        {
            Logger::error(Tag(), "getIntOrDefault: The value for the key '%' is not int", key);
            throw RmsauthJsonParsingException("JsonUtilsQt::getIntOrDefault", "value is not int");
        }

        return res.toInt();
    }
    return defval;
}

int JsonUtilsQt::getStringAsIntOrDefault(const QJsonObject& qobj, const String& key, int defval)
{
    if (qobj.contains(key.data()))
    {
        auto res = qobj.value(key.data());

        if(res.isDouble())
        {
            return res.toDouble(defval);
        }

        if(!res.isString())
        {
            Logger::error(Tag(), "getStringAsIntOrDefault: The value for the key '%' is not a string", key);
            throw RmsauthJsonParsingException("JsonUtilsQt::getStringAsIntOrDefault", "value is not a string");
        }

        auto strRes = res.toString();
        bool ok;
        auto intRes = strRes.toInt(&ok);

        if(!ok)
        {
            Logger::error(Tag(), "The value '%' for the key '%' can't be converted to int", strRes.toStdString(), key);
            throw RmsauthJsonParsingException("JsonUtilsQt::getStringAsIntOrZero", "value can't be converted to int");
        }

        return intRes;
    }
    return defval;
}

bool JsonUtilsQt::getBoolOrDefault(const QJsonObject& qobj, const String& key, bool defval)
{
    if (qobj.contains(key.data()))
    {
        auto res = qobj.value(key.data());
        if(!res.isBool())
        {
            Logger::error(Tag(), "getBoolOrDefault: The value for the key '%' is not bool", key);
            throw RmsauthJsonParsingException("JsonUtilsQt::getBoolOrDefault", "value is not bool");
        }

        return res.toBool();
    }

    return defval;
}

IntArray JsonUtilsQt::getIntArrayOrEmpty(const QJsonObject& qobj, const String& key)
{
    IntArray sa;
    if (qobj.contains(key.data()))
    {
        QJsonValue val =  qobj.value(key.data());
        if(val.isArray())
        {
            QJsonArray arr = val.toArray();
            for( QJsonValue elem : arr)
            {
                if(elem.isDouble())
                {
                    sa.push_back( elem.toInt());
                }
                else
                {
                    StringStream ss;
                    ss << "getIntArrayOrEmpty: the array element for the key '" <<  key << "' in not an int";
                    throw RmsauthException(ss.str());
                }
            }
        }
        else if(val.isNull())
        {
            return std::move(sa);
        }
        else
        {
            StringStream ss;
            ss << "getIntArrayOrEmpty: the value for the key '" <<  key << "' in not an array";
            throw RmsauthException(ss.str());
        }
    }
    return std::move(sa);
}

void JsonUtilsQt::insertObject(QJsonObject& qobj, const String& key, const String& jsonString)
{
    QJsonParseError error;
    auto qdoc = QJsonDocument::fromJson(jsonString.data(), &error);
    if( error.error != QJsonParseError::NoError )
    {
        throw RmsauthException(String("AuthenticationResult::deserialize: ") + error.errorString().toStdString());
    }
    QJsonObject qobjToInsert  = qdoc.object();
    qobj.insert(key.data(), qobjToInsert);
}

void JsonUtilsQt::insertString(QJsonObject& qobj, const String& key, const String& str)
{
   qobj.insert(QString::fromStdString(key), QString::fromStdString(str));
}

} // namespace rmsauth {
