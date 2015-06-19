/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef JSONUTILSQT_H
#define JSONUTILSQT_H

#include <types.h>
#include <Exceptions.h>
#include <QJsonObject>
#include <QJsonDocument>

namespace rmsauth {

class JsonUtilsQt
{
    static const String Tag(){static const String tag = "JsonUtilsQt"; return tag;}
    template<typename P>
    using Deserializer = P (*)(const String& jsonString);

public:
    static String getStringOrDefault(const QJsonObject& qobj, const String& key, const String defval = "");
    static int getIntOrDefault(const QJsonObject& qobj, const String& key, int defval = 0 );
    static int getStringAsIntOrDefault(const QJsonObject& qobj, const String& key, int defval = 0);
    static bool getBoolOrDefault(const QJsonObject& qobj, const String& key, bool defval = false);
    static IntArray getIntArrayOrEmpty(const QJsonObject& qobj, const String& key);
    template<typename T>
    static T parseObject(const QJsonObject& qobj, const String& key, Deserializer<T> d);
    static void insertObject(QJsonObject& qobj, const String& key, const String& jsonString);
    static void insertString(QJsonObject& qobj, const String& key, const String& str);
};

template<typename P>
P JsonUtilsQt::parseObject(const QJsonObject& qobj, const String& key, Deserializer<P> deserialize)
{
    if (!qobj.contains(key.data()))
    {
        return nullptr;
    }

    auto qjo = qobj.value(key.data());
    QJsonDocument doc(qjo.toObject());

    auto jsonData = doc.toJson(QJsonDocument::Compact);

    return deserialize(String(jsonData.begin(), jsonData.end()));
}

} // namespace rmsauth {

#endif // JSONUTILSQT_H
