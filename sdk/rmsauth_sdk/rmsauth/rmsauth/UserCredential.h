/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef USERCREDENTIAL
#define USERCREDENTIAL

#include "types.h"
#include "rmsauthExport.h"

namespace rmsauth {

enum class UserAuthType
{
    IntegratedAuth,
    UsernamePassword
};

class RMSAUTH_EXPORT UserCredential
{
    UserAuthType userAuthType_;
    String userName_;
    String password_;

public:
    UserCredential();
    UserCredential(const String& userName);
    UserCredential(const String& userName, const String& password);

    const String& userName() const      { return userName_; }
    void userName(const String& val)    { userName_= val; }

    UserAuthType userAuthType() const   { return userAuthType_; }

    const String& password() const      { return password_; }
};

using UserCredentialPtr = ptr<UserCredential>;

} // namespace rmsauth {

#endif // USERCREDENTIAL
