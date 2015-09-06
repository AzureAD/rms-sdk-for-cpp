/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_RESTSERVICEURLCLIENT_H_
#define _RMS_LIB_RESTSERVICEURLCLIENT_H_

#include <memory>
#include <algorithm>
#include <map>
#include "IRestServiceUrlClient.h"

namespace rmscore {
namespace restclients {
// TODO: What this???
struct MyStringCompare
{
  bool operator()(const std::string& lhs, const std::string& rhs) const
  {
    auto a = lhs;
    auto b = rhs;
    std::transform(a.begin(), a.end(), a.begin(), toupper);
    std::transform(b.begin(), b.end(), b.begin(), toupper);

    return a < b;
  }
};

class RestServiceUrlClient : public IRestServiceUrlClient {
public:

  virtual ~RestServiceUrlClient();

  virtual std::string GetEndUserLicensesUrl(
    const void                             *pbPublishLicense,
    const size_t                            cbPublishLicense,
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    modernapi::IConsentCallbackImpl       & consentCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;
  virtual std::string GetTemplatesUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;
  virtual std::string GetPublishUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;
  virtual std::string GetCloudDiagnosticsServerUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;
  virtual std::string GetPerformanceServerUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
  override;
  virtual std::shared_ptr<ServiceDiscoveryDetails>GetServiceDiscoveryDetails(
    const void                             *pbPublishLicense,
    const size_t                            cbPublishLicense,
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    modernapi::IConsentCallbackImpl        *consentCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState) override;

private:

  std::string                                    GetTtlString(uint32_t ttl);

  static std::shared_ptr<ServiceDiscoveryDetails>FindCache(
    const std::string& sEmail);
  static std::map<std::string, std::shared_ptr<ServiceDiscoveryDetails>,
                  MyStringCompare> serviceDiscoveryDetailsCache;

  static void GetConsent(
    modernapi::IConsentCallbackImpl       & consentCallback,
    const std::string                     & sEmail,
    std::shared_ptr<ServiceDiscoveryDetails>serviceDiscoveryDetails);
  static void GetConsent(
    modernapi::IConsentCallbackImpl& consentCallback,
    const std::string              & sEmail,
    const std::string              & domain,
    const std::string              & url);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_RESTSERVICEURLCLIENT_H_
