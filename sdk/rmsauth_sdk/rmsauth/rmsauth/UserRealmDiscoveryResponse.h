/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef USERREALMDISCOVERYRESPONSE
#define USERREALMDISCOVERYRESPONSE

#include <types.h>
#include <CallState.h>

namespace rmsauth {

class UserRealmDiscoveryResponse final
{
    static const String& Tag() {static const String tag="UserRealmDiscoveryResponse"; return tag;}

    String version_;
    String accountType_;
    String federationProtocol_;
    String federationMetadataUrl_;
    String federationActiveAuthUrl_;

public:
    const String& version() const { return version_; }
    const String& accountType() const { return accountType_; }
    const String& federationProtocol() const { return federationProtocol_; }
    const String& federationMetadataUrl() const { return federationMetadataUrl_; }
    const String& federationActiveAuthUrl() const { return federationActiveAuthUrl_; }

    void version(const String& val) { version_ = val; }
    void accountType(const String& val) { accountType_ = val; }
    void federationProtocol(const String& val) { federationProtocol_ = val; }
    void federationMetadataUrl(const String& val) { federationMetadataUrl_ = val; }
    void federationActiveAuthUrl(const String& val) { federationActiveAuthUrl_ = val; }

    static UserRealmDiscoveryResponse createByDiscoveryAsync(const String& userRealmUri, const String& userName, CallStatePtr callState);

    struct JsonNames {
       const String version = "ver";
       const String accountType = "account_type";
       const String federationProtocol = "federation_protocol";
       const String federationMetadataUrl = "federation_metadata_url";
       const String federationActiveAuthUrl = "federation_active_auth_url";
    };

    static const JsonNames& jsonNames()
    {
        static const JsonNames jsonNames{};
        return jsonNames;
    }
};

} // namespace rmsauth {

#endif // USERREALMDISCOVERYRESPONSE

