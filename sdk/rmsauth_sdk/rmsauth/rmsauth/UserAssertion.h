/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef USERASSERTION
#define USERASSERTION

#include "types.h"

namespace rmsauth {

class UserAssertion final
{
    String assertion_;
    String assertionType_;
    String userName_;

public:
    UserAssertion(const String& assertion);
    UserAssertion(const String& assertion, const String& assertionType);
    UserAssertion(const String& assertion, const String& assertionType, const String& userName);

    const String& assertion()       const { return assertion_; }
    const String& assertionType()   const { return assertionType_; }
    const String& userName()        const { return userName_; }
};

using UserAssertionPtr = ptr<UserAssertion>;

} // namespace rmsauth {

#endif // USERASSERTION

