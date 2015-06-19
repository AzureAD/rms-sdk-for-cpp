/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <CallState.h>

namespace rmsauth {

CallState::CallState(Guid& correlationId, bool callSync)
    : correlationId_(correlationId)
    , callSync_(callSync)
{
}

} // namespace rmsauth {
