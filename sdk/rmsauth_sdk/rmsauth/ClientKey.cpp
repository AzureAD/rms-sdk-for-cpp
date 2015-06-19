/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <ClientKey.h>

namespace rmsauth {

ClientKey::ClientKey(ClientCredentialPtr clientCredential)
    : credential_(clientCredential)
    , clientId_(clientCredential->clientId())
    , hasCredential_(true)
{
}

ClientKey::ClientKey(ClientAssertionCertificatePtr clientCertificate, AuthenticatorPtr authenticator)
    : certificate_(clientCertificate)
    , authenticator_(authenticator)
    , clientId_(clientCertificate->clientId())
    , hasCredential_(true)
{
}

ClientKey::ClientKey(ClientAssertionPtr clientAssertion)
    : assertion_(clientAssertion)
    , clientId_(clientAssertion->clientId())
    , hasCredential_(true)
{
}

ClientKey::ClientKey(const String& clientId)
    : clientId_(clientId)
    , hasCredential_(false)
{
    if (clientId.empty())
    {
        throw new IllegalArgumentException("clientId");
    }
}

} // namespace rmsauth {
