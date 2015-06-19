/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include "types.h"
#include "AuthorityType.h"
#include "Guid.h"
#include "CallState.h"
#include "Exceptions.h"
#include "AuthenticatorTemplateList.h"
#include "Constants.h"
#include "utils.h"
#include "Url.h"

namespace rmsauth {

class Authenticator
{
    static const String& Tag() {static const String tag="Authenticator"; return tag;}

    static const String tenantlessTenantName();

    static AuthenticatorTemplateList authenticatorTemplateList;
    bool updatedFromTemplate_ = false;

    bool validateAuthority_;
    bool default_ = true;
    String authority_;
    AuthorityType authorityType_;
    bool isTenantless_;
    String authorizationUri_;
    String tokenUri_;
    String userRealmUri_;
    String selfSignedJwtAudience_;
    Guid correlationId_;

public:
    Authenticator():default_(true){}
    Authenticator(const String& authority, bool validateAuthority);

    const String& authority()               const {return authority_;}
    const AuthorityType& authorityType()    const {return authorityType_;}
    bool validateAuthority()                const {return validateAuthority_;}
    bool isTenantless()                     const {return isTenantless_;}
    const String& authorizationUri()        const {return authorizationUri_;}
    const String& tokenUri()                const {return tokenUri_;}
    const String& userRealmUri()            const {return userRealmUri_;}
    const String& selfSignedJwtAudience()   const {return selfSignedJwtAudience_;}
    const Guid& correlationId()             const {return correlationId_;}
    void correlationId(const Guid& val)     {correlationId_ = val;}

    void updateFromTemplateAsync(CallStatePtr callState);
    void updateTenantId(const String& tenantId);

private:
    static AuthorityType detectAuthorityType(const String& authority);
    static String canonicalizeUri(const String& uri);
    static String replaceTenantlessTenant(const String& authority, const String& tenantId);
    static bool isAdfsAuthority(const String& firstPath);

};

using AuthenticatorPtr = ptr<Authenticator>;

} // namespace rmsauth {

#endif // AUTHENTICATOR_H
