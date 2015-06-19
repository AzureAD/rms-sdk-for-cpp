/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CLIENTASSERTION_H
#define CLIENTASSERTION_H

#include "types.h"

namespace rmsauth {

class ClientAssertion
{
public:
    ClientAssertion();
    const String& clientId() const { return clientId_; }

private:
    String clientId_;

};

using ClientAssertionPtr = ptr<ClientAssertion>;

} // namespace rmsauth {

#endif // CLIENTASSERTION_H
