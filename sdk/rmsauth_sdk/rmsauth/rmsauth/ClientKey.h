/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CLIENTKEY_H
#define CLIENTKEY_H

#include "types.h"
#include "ClientCredential.h"
#include "ClientAssertionCertificate.h"
#include "ClientAssertion.h"
#include "Authenticator.h"

namespace rmsauth {

class ClientKey
{
public:
    ClientKey(ClientCredentialPtr clientCredential);
    ClientKey(ClientAssertionCertificatePtr clientCertificate, AuthenticatorPtr authenticator);
    ClientKey(ClientAssertionPtr clientAssertion);
    ClientKey(const String& clientId);

    ClientCredentialPtr credential() const            { return credential_; }
    ClientAssertionCertificatePtr certificate() const { return certificate_; }
    ClientAssertionPtr assertion() const              { return assertion_; }
    AuthenticatorPtr authenticator() const            { return authenticator_; }
    const String& clientId() const                    { return clientId_; }
    bool hasCredential() const                        { return hasCredential_; }

private:
    ClientCredentialPtr credential_ = nullptr;
    ClientAssertionCertificatePtr certificate_ = nullptr;
    ClientAssertionPtr assertion_ = nullptr;
    AuthenticatorPtr authenticator_ = nullptr;
    String clientId_;
    bool hasCredential_;
};

using ClientKeyPtr = ptr<ClientKey>;

} // namespace rmsauth {

#endif // CLIENTKEY_H
