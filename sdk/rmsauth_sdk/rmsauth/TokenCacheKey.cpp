/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <TokenCacheKey.h>

namespace rmsauth {

TokenCacheKey::TokenCacheKey(const String& authority, const String& resource, const String& clientId, const TokenSubjectType tokenSubjectType, const UserInfoPtr userInfo)
     : TokenCacheKey{authority, resource, clientId, tokenSubjectType, (userInfo != nullptr) ? userInfo->uniqueId() : ""/*, (userInfo != nullptr) ? userInfo->displayableId() : ""*/}
 {
 }

 TokenCacheKey::TokenCacheKey(const String& authority, const String& resource, const String& clientId, const TokenSubjectType tokenSubjectType, const String& uniqueId/*, const String& displayableId*/)
     : authority_(authority)
     , resource_(resource)
     , clientId_(clientId)
     , uniqueId_(uniqueId)
//     , displayableId_(displayableId)
     , tokenSubjectType_(tokenSubjectType)
 {
 }

 bool TokenCacheKey::equals(const TokenCacheKey& other) const
 {
     return StringUtils::equalsIC(other.authority_, authority_)
         && StringUtils::equalsIC(other.resource_, resource_)
         && StringUtils::equalsIC(other.clientId_, clientId_)
         && StringUtils::equalsIC(other.uniqueId_, uniqueId_)
//         && StringUtils::equalsIC(other.displayableId_, displayableId_)
         && (other.tokenSubjectType_ == tokenSubjectType_);
 }

bool TokenCacheKey::operator==(const TokenCacheKey &other) const
{
    return equals(other);
}

size_t TokenCacheKey::getHashCode() const
{
     const String DELIM = ":::";
     StringStream ss;
     ss << StringUtils::toLower(authority_) << DELIM
        << StringUtils::toLower(resource_) << DELIM
        << StringUtils::toLower(clientId_) << DELIM
        << StringUtils::toLower(uniqueId_) << DELIM
//        << StringUtils::toLower(displayableId_) << DELIM
        << static_cast<int>(tokenSubjectType_);

    auto hashcode = std::hash<String>()(ss.str());
    Logger::info(Tag(),
        "TokenCacheKey::getHashCode(): authority: %; resource: %; clientId: % uniqueId: %; tokenSubjectType_: %; hashcode: %;",
        authority_, resource_, clientId_, uniqueId_, /*displayableId_, */(int)tokenSubjectType_, hashcode);

    return hashcode;
}

} // namespace rmsauth {
