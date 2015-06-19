/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef IDTOKEN
#define IDTOKEN
#include "types.h"

struct IdToken
{
    String mSubject;
    String mTenantId;
    String mUpn;
    String mGivenName;
    String mFamilyName;
    String mEmail;
    String mIdentityProvider;
    String mObjectId;
    int64_t mPasswordExpiration;
    String mPasswordChangeUrl;
};

#endif // IDTOKEN

