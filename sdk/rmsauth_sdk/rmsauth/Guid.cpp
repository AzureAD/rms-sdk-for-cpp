/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <Guid.h>

namespace rmsauth {

String Guid::toString() const
{
   return this->pImpl->toString();
}
bool Guid::empty() const
{
    return this->pImpl->empty();
}

} // namespace rmsauth {
