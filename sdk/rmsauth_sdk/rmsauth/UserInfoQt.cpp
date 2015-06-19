/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <UserInfo.h>
#include "JsonUtilsQt.h"
#include <QJsonObject>
#include <QJsonDocument>

namespace rmsauth {

UserInfoPtr UserInfo::deserialize(const String& jsonString)
{
    QJsonParseError error;
    auto qdoc = QJsonDocument::fromJson(jsonString.c_str(), &error);
    if( error.error != QJsonParseError::NoError )
    {
        throw RmsauthJsonParsingException("UserInfo::deserialize: ", error.errorString().toStdString());
    }
    QJsonObject qobj = qdoc.object();

    auto userInfo = std::make_shared<UserInfo>();
    userInfo->uniqueId_          = JsonUtilsQt::getStringOrDefault(qobj, UserInfo::jsonNames().uniqueId);
    userInfo->displayableId_     = JsonUtilsQt::getStringOrDefault(qobj, UserInfo::jsonNames().displayableId);
    userInfo->givenName_          = JsonUtilsQt::getStringOrDefault(qobj, UserInfo::jsonNames().givenName);
    userInfo->familyName_         = JsonUtilsQt::getStringOrDefault(qobj, UserInfo::jsonNames().familyName);
    userInfo->identityProvider_   = JsonUtilsQt::getStringOrDefault(qobj, UserInfo::jsonNames().identityProvider);
    userInfo->passwordChangeUrl_  = JsonUtilsQt::getStringOrDefault(qobj, UserInfo::jsonNames().passwordChangeUrl);
    userInfo->passwordExpiresOn_  = (DateTimeOffset)JsonUtilsQt::getIntOrDefault(qobj, UserInfo::jsonNames().passwordExpiresOn);
    userInfo->forcePrompt_        = JsonUtilsQt::getBoolOrDefault(qobj, UserInfo::jsonNames().forcePrompt);

    return userInfo;
}

String UserInfo::serialize()
{
    QJsonObject qobj;
    JsonUtilsQt::insertString(qobj, UserInfo::jsonNames().uniqueId, uniqueId_);
    JsonUtilsQt::insertString(qobj, UserInfo::jsonNames().displayableId, displayableId_);
    JsonUtilsQt::insertString(qobj, UserInfo::jsonNames().givenName, givenName_);
    JsonUtilsQt::insertString(qobj, UserInfo::jsonNames().familyName, familyName_);
    JsonUtilsQt::insertString(qobj, UserInfo::jsonNames().identityProvider, identityProvider_);
    JsonUtilsQt::insertString(qobj, UserInfo::jsonNames().passwordChangeUrl, passwordChangeUrl_);
    qobj.insert(UserInfo::jsonNames().passwordExpiresOn.c_str(), (int)passwordExpiresOn_);
    qobj.insert(UserInfo::jsonNames().forcePrompt.c_str(), forcePrompt_);

    QJsonDocument doc(qobj);
    auto res = doc.toJson(QJsonDocument::Compact);
    return String(res.begin(), res.end());
}

} // namespace rmsauth {
