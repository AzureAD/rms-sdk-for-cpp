/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AuthenticationResult.h>
#include <Exceptions.h>
#include <Logger.h>
#include "JsonUtilsQt.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace rmsauth {

AuthenticationResultPtr AuthenticationResult::deserialize(const String& jsonString)
{
    Logger::info(Tag(), "deserialize");
    AuthenticationResultPtr result = nullptr;

    QJsonParseError error;
    auto qdoc = QJsonDocument::fromJson(jsonString.c_str(), &error);
    if( error.error != QJsonParseError::NoError )
    {
        throw RmsauthException(String("AuthenticationResult::deserialize: ") + error.errorString().toStdString());
    }
    QJsonObject qobj  = qdoc.object();

    result.reset(new AuthenticationResult());

    result->accessTokenType_                   = JsonUtilsQt::getStringOrDefault(qobj, AuthenticationResult::JsonNames.accessTokenType);
    result->accessToken_                       = JsonUtilsQt::getStringOrDefault(qobj, AuthenticationResult::JsonNames.accessToken);
    result->resource_                          = JsonUtilsQt::getStringOrDefault(qobj, AuthenticationResult::JsonNames.resource);
    result->refreshToken_                      = JsonUtilsQt::getStringOrDefault(qobj, AuthenticationResult::JsonNames.refreshToken);
    result->expiresOn_                         = JsonUtilsQt::getIntOrDefault(qobj, AuthenticationResult::JsonNames.expiresOn);
    result->tenantId_                          = JsonUtilsQt::getStringOrDefault(qobj, AuthenticationResult::JsonNames.tenantId);

    result->userInfo_                          = JsonUtilsQt::parseObject<UserInfoPtr>(qobj, AuthenticationResult::JsonNames.userInfo, &UserInfo::deserialize);

    result->idToken_                           = JsonUtilsQt::getStringOrDefault(qobj, AuthenticationResult::JsonNames.idToken);
    result->isMultipleResourceRefreshToken_    = JsonUtilsQt::getBoolOrDefault(qobj, AuthenticationResult::JsonNames.isMultipleResourceRefreshToken);

    return result;
}
String AuthenticationResult::serialize()
{
    Logger::info(Tag(), "serialize");
    QJsonObject qobj;
    JsonUtilsQt::insertString(qobj, AuthenticationResult::JsonNames.accessTokenType, accessTokenType_);
    JsonUtilsQt::insertString(qobj, AuthenticationResult::JsonNames.accessToken, accessToken_);
    JsonUtilsQt::insertString(qobj, AuthenticationResult::JsonNames.resource, resource_);
    JsonUtilsQt::insertString(qobj, AuthenticationResult::JsonNames.refreshToken, refreshToken_);
    qobj.insert(AuthenticationResult::JsonNames.expiresOn.c_str(), (int)expiresOn_);
    JsonUtilsQt::insertString(qobj, AuthenticationResult::JsonNames.tenantId, tenantId_);
    if(userInfo_ != nullptr)
        JsonUtilsQt::insertObject(qobj, AuthenticationResult::JsonNames.userInfo, userInfo_->serialize());
    JsonUtilsQt::insertString(qobj, AuthenticationResult::JsonNames.idToken, idToken_);
    qobj.insert(AuthenticationResult::JsonNames.isMultipleResourceRefreshToken.c_str(), isMultipleResourceRefreshToken_);

    QJsonDocument doc(qobj);
    auto res = doc.toJson(QJsonDocument::Compact);
    return String(res.begin(), res.end());
}

} // namespace rmsauth {
