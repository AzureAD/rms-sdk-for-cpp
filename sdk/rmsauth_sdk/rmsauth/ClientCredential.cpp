/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <ClientCredential.h>

namespace rmsauth {

ClientCredential::ClientCredential(const String& clientId, const String& clientSecret)
    : clientId_ (clientId)
    , clientSecret_(clientSecret)
{
    if (clientId.empty())
    {
        throw new IllegalArgumentException("clientId");
    }

    if (clientSecret.empty())
    {
        throw new IllegalArgumentException("clientSecret");
    }
}

} // namespace rmsauth {
