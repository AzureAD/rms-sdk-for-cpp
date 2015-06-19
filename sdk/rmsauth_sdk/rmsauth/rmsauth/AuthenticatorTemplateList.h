/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef AUTHENTICATORTEMPLATELIST_H
#define AUTHENTICATORTEMPLATELIST_H
#include "types.h"
#include "AuthenticatorTemplate.h"
#include "CallState.h"
#include <algorithm>

namespace rmsauth {

class AuthenticatorTemplateList : public ArrayList<AuthenticatorTemplatePtr>
{
public:
    AuthenticatorTemplateList();

    AuthenticatorTemplatePtr findMatchingItemAsync(bool validateAuthority, const String& host, const String& tenant, CallStatePtr callState);

};

} // namespace rmsauth {

#endif // AUTHENTICATORTEMPLATELIST_H
