/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CLIENTASSERTIONCERTIFICATE_H
#define CLIENTASSERTIONCERTIFICATE_H

#include "types.h"

namespace rmsauth {

class ClientAssertionCertificate
{
public:
    ClientAssertionCertificate();
    const String& clientId() const { return clientId_; }

private:
    String clientId_;
};

using ClientAssertionCertificatePtr = ptr<ClientAssertionCertificate>;

} // namespace rmsauth {

#endif // CLIENTASSERTIONCERTIFICATE_H
