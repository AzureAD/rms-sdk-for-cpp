/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CLIENTCREDENTIAL_H
#define CLIENTCREDENTIAL_H

#include "types.h"
#include "Exceptions.h"
#include "rmsauthExport.h"

namespace rmsauth {

class RMSAUTH_EXPORT ClientCredential
{
public:
    ClientCredential(const String& clientId, const String& clientSecret);

    const String& clientId() const { return clientId_; }
    const String& clientSecret() const { return clientSecret_; }

private:
    String clientId_;
    String clientSecret_;
};

using ClientCredentialPtr = ptr<ClientCredential>;

} // namespace rmsauth {

#endif // CLIENTCREDENTIAL_H
