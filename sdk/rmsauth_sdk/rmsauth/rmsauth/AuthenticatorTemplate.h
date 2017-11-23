/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef AUTHENTICATORTEMPLATE_H
#define AUTHENTICATORTEMPLATE_H
#include <string>
#include "CallState.h"
#include "types.h"

namespace rmsauth {

class AuthenticatorTemplate
{
    static const String& Tag() {static const String tag="AuthenticatorTemplate"; return tag;}

    String host_;
    String issuer_;
    String authority_;
    String instanceDiscoveryEndpoint_;
    String authorizeEndpoint_;
    String tokenEndpoint_;
    String userRealmEndpoint_;

    static const String authorizeEndpointTemplate();
    static const String V1_ENDPOINT();
    static const String V2_ENDPOINT();
    static const String HOST();
    static const String TENANT();
    static const String VERSION();

public:
    enum EndpointVersion { V1, V2 };

    const String& host()                        const {return host_;}
    const String& issuer()                      const {return issuer_;}
    const String& authority()                   const {return authority_;}
    const String& instanceDiscoveryEndpoint()   const {return instanceDiscoveryEndpoint_;}
    const String& authorizeEndpoint()           const {return authorizeEndpoint_;}
    const String& tokenEndpoint()               const {return tokenEndpoint_;}
    const String& userRealmEndpoint()           const {return userRealmEndpoint_;}

    static ptr<AuthenticatorTemplate> createFromHost(const String& host);
    static ptr<AuthenticatorTemplate> createFromHost(const String& host, const EndpointVersion version);
    static EndpointVersion getEndpointVersion(const String& url);
    void verifyAnotherHostByInstanceDiscoveryAsync(const String& host, const String& tenant, CallStatePtr callState);

private:
    static ptr<AuthenticatorTemplate> create(const String& host, const EndpointVersion endpointVersion);

};

using AuthenticatorTemplatePtr = ptr<AuthenticatorTemplate>;

} // namespace rmsauth {

#endif // AUTHENTICATORTEMPLATE_H
