/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <TokenCache.h>
#include <QByteArray>
#include <QDataStream>

namespace rmsauth {

ByteArray TokenCache::serialize()
{
    Logger::info(Tag(), "serialize");

    QByteArray qba;

    QDataStream qds(&qba, QIODevice::WriteOnly);

    Logger::info(Tag(), "Serializing token cache with % items.", tokenCacheDictionary_.size());

    qds << SchemaVersion_;
    qds << count();
    for( auto kvp : tokenCacheDictionary_)
    {
        auto tokenCacheKey = kvp.first;
        AuthenticationResultPtr authenticationResultPtr = kvp.second;

        qds << QString::fromStdString(tokenCacheKey.authority());
        qds << QString::fromStdString(tokenCacheKey.resource());
        qds << QString::fromStdString(tokenCacheKey.clientId());
        qds << static_cast<int>(tokenCacheKey.tokenSubjectType());

        qds << QString::fromStdString(authenticationResultPtr->serialize());
    }

    ByteArray ba(qba.begin(), qba.end());
    return std::move(ba);
}

void TokenCache::deserialize(const ByteArray& state)
{
    Logger::info(Tag(), "deserialize");

    if (state.empty())
    {
       tokenCacheDictionary_.clear();
       return;
    }

    QByteArray qba(state.data(), (int)state.size());
    QDataStream qds(&qba, QIODevice::ReadOnly);

    int schemaVersion;
    qds >> schemaVersion;
    if (schemaVersion != SchemaVersion_)
    {
        Logger::warning(Tag(), "The version of the persistent state of the cache does not match the current schema, so skipping deserialization.");
        return;
    }

    int count;
    qds >> count;

    for(int i=0; i<count; ++i)
    {
        QString authority;
        QString resource;
        QString clientId;
        int tokenSubjectType;

        qds >> authority;
        qds >> resource;
        qds >> clientId;
        qds >> tokenSubjectType;

        QString authenticationResultString;
        qds >> authenticationResultString;
        AuthenticationResultPtr result = AuthenticationResult::deserialize(authenticationResultString.toStdString());

        if(result == nullptr)
        {
            throw RmsauthParsingException("AuthenticationResult::deserialize returned nullptr");
        }

        TokenCacheKey tokenCacheKey(
            authority.toStdString()
            ,resource.toStdString()
            ,clientId.toStdString()
            ,TokenSubjectType(tokenSubjectType)
            ,result->userInfo());


        tokenCacheDictionary_.insert(std::make_pair(std::move(tokenCacheKey), result));
    }

    Logger::info(Tag(), "Deserialized % items to token cache.", this->count());
}

} // namespace rmsauth {
