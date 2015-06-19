/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef USERINFO
#define USERINFO

#include "types.h"
#include "Entities.h"
#include "DateTime.h"
#include "Url.h"

namespace rmsauth {

class UserInfo;
using UserInfoPtr = ptr<UserInfo>;

class UserInfo
{
    String uniqueId_;
    String displayableId_;
    String givenName_;
    String familyName_;
    String identityProvider_;
    String passwordChangeUrl_;
    DateTimeOffset passwordExpiresOn_;
    bool forcePrompt_;

public:
    const String& uniqueId()                    const { return uniqueId_; }
    void uniqueId(const String& val)            { uniqueId_ = val; }

    const String& displayableId()               const { return displayableId_; }
    void displayableId(const String& val)       { displayableId_ = val; }

    const String& givenName()                   const { return givenName_; }
    void givenName(const String& val)           { givenName_ = val; }

    const String& familyName()                  const { return familyName_; }
    void familyName(const String& val)          { familyName_ = val; }

    const String& identityProvider()            const { return identityProvider_; }
    void identityProvider(const String& val)    { identityProvider_ = val; }

    const String& passwordChangeUrl()           const { return passwordChangeUrl_; }
    void passwordChangeUrl(const String& val)   { passwordChangeUrl_ = val; }

    const DateTimeOffset& passwordExpiresOn()   const { return passwordExpiresOn_; }
    void passwordExpiresOn(const DateTimeOffset& val)   { passwordExpiresOn_ = val; }

    const bool& forcePrompt()                   const { return forcePrompt_; }
    void forcePrompt(const bool& val)           { forcePrompt_ = val; }

    String serialize();
    static UserInfoPtr deserialize(const String& jsonString);

private:
    struct JsonNames {
       const String uniqueId = "uniqueId";
       const String displayableId = "displayableId";
       const String givenName = "givenName";
       const String familyName = "familyName";
       const String identityProvider = "identityProvider";
       const String passwordChangeUrl = "passwordChangeUrl";
       const String passwordExpiresOn = "passwordExpiresOn";
       const String forcePrompt = "forcePrompt";
    };

    static const JsonNames& jsonNames()
    {
        static const JsonNames jsonNames{};
        return jsonNames;
    }

};

} // namespace rmsauth {

#endif // USERINFO
