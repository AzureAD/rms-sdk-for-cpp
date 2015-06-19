/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef TOKENSUBJECTTYPE
#define TOKENSUBJECTTYPE

#include "types.h"

namespace rmsauth {

enum class TokenSubjectType
{
    User = 0,
    Client,
    UserPlusClient
};

} // namespace rmsauth {

#endif // TOKENSUBJECTTYPE

