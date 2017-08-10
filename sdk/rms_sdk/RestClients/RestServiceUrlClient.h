/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_RESTSERVICEURLCLIENT_H_
#define _RMS_LIB_RESTSERVICEURLCLIENT_H_

#include <algorithm>
#include <map>
#include <memory>

#include "IRestServiceUrlClient.h"
#include "LicenseParserResult.h"

namespace rmscore {
namespace restclients {

struct MyStringCompare
{
  bool operator()(const std::string& ilhs, const std::string& irhs) const
  {
    auto lhs = ilhs;
    auto rhs = irhs;
    std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::toupper);
    std::transform(rhs.begin(), rhs.end(), rhs.begin(), ::toupper);
    return lhs < rhs;
  }
};

class RestServiceUrlClient : public IRestServiceUrlClient
{
public:

    virtual ~RestServiceUrlClient();

    virtual std::string GetClientLicensorCertificatesUrl(
        const std::string                     & sEmail,
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        std::shared_ptr<std::atomic<bool> >     cancelState) override;

    virtual std::string GetEndUserLicensesUrl(const std::shared_ptr<LicenseParserResult>& licenseParserResult,
        const std::string& sEmail,
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        modernapi::IConsentCallbackImpl& consentCallback,
        std::shared_ptr<std::atomic<bool>> cancelState) override;

    virtual std::string GetTemplatesUrl(const std::string& sEmail,
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        std::shared_ptr<std::atomic<bool>> cancelState) override;

    virtual std::string GetPublishUrl(const std::string& sEmail,
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        std::shared_ptr<std::atomic<bool>> cancelState) override;

    virtual std::string GetCloudDiagnosticsServerUrl(const std::string& sEmail,
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        std::shared_ptr<std::atomic<bool>> cancelState) override;

    virtual std::string GetPerformanceServerUrl(const std::string& sEmail,
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        std::shared_ptr<std::atomic<bool>> cancelState) override;

    virtual std::shared_ptr<ServiceDiscoveryDetails> GetServiceDiscoveryDetails(
        const std::shared_ptr<LicenseParserResult>& licenseParserResults,
        const std::string& sEmail,
        modernapi::IAuthenticationCallbackImpl& authenticationCallback,
        modernapi::IConsentCallbackImpl* consentCallback,
        std::shared_ptr<std::atomic<bool>> cancelState) override;

private:

    static std::map<std::string, std::shared_ptr<ServiceDiscoveryDetails>, MyStringCompare>
        serviceDiscoveryDetailsCache;

    std::string GetTtlString(uint32_t ttl);

    static std::shared_ptr<ServiceDiscoveryDetails> FindCache(const std::string& sEmail);


    static void GetConsent(modernapi::IConsentCallbackImpl& consentCallback,
        const std::string& sEmail,
        std::shared_ptr<ServiceDiscoveryDetails>serviceDiscoveryDetails);

    static void GetConsent(modernapi::IConsentCallbackImpl& consentCallback,
        const std::string& sEmail,
        const std::string& domain,
        const std::string& url);
};

} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_RESTSERVICEURLCLIENT_H_
