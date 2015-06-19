/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <UserAssertion.h>
#include <Exceptions.h>

namespace rmsauth {

UserAssertion::UserAssertion(const String& assertion)
{
    if (assertion.empty())
    {
        throw RmsauthException("assertion", "the value is empty");
    }

    assertion_ = assertion;
}

UserAssertion::UserAssertion(const String& assertion, const String& assertionType)
    : UserAssertion(assertion, assertionType, "")
{}

UserAssertion::UserAssertion(const String& assertion, const String& assertionType, const String& userName)
{
    if (assertion.empty())
    {
        throw RmsauthException("assertion", "the value is empty");
    }

    if (assertionType.empty())
    {
         throw RmsauthException("assertionType", "the value is empty");
    }

    assertion_ = assertion;
    assertionType_ = assertionType;
    userName_ = userName;
}

} // namespace rmsauth {
