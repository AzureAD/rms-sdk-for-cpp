/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ACQUIRETOKENFORCLIENTHANDLER_H
#define ACQUIRETOKENFORCLIENTHANDLER_H

#include "AcquireTokenHandlerBase.h"

namespace rmsauth {

class AcquireTokenForClientHandler : public AcquireTokenHandlerBase
{
    static const String& Tag() {static const String tag="AcquireTokenForClientHandler"; return tag;}

public:
    AcquireTokenForClientHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, ClientKeyPtr clientKey, bool callSync);

protected:
    void addAditionalRequestParameters(RequestParameters& requestParameters) override;
};

} // namespace rmsauth {

#endif // ACQUIRETOKENFORCLIENTHANDLER_H
