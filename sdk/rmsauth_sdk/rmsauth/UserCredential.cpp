/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <UserCredential.h>

namespace rmsauth {

UserCredential::UserCredential()
    : userAuthType_(UserAuthType::IntegratedAuth)
{
}

UserCredential::UserCredential(const String& userName)
    : userAuthType_(UserAuthType::IntegratedAuth)
    , userName_(userName)
{
}

UserCredential::UserCredential(const String& userName, const String& password)
    : userAuthType_(UserAuthType::UsernamePassword)
    , userName_(userName)
    , password_(password)
{
}

} // namespace rmsauth {
