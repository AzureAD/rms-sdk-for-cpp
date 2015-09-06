/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_IRESTSERVICEURLCLIENT_H_
#define _RMS_LIB_IRESTSERVICEURLCLIENT_H_

#include <memory>
#include <string>
#include <atomic>
#include "../ModernAPI/IAuthenticationCallbackImpl.h"
#include "../ModernAPI/IConsentCallbackImpl.h"
#include "ServiceDiscoveryDetails.h"

namespace rmscore {
namespace restclients {
class IRestServiceUrlClient {
public:

  virtual ~IRestServiceUrlClient() {}

  virtual std::string GetEndUserLicensesUrl(
    const void                             *pbPublishLicense,
    const size_t                            cbPublishLicense,
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    modernapi::IConsentCallbackImpl       & consentCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
    = 0;
  virtual std::string GetTemplatesUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
    = 0;
  virtual std::string GetPublishUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
    = 0;
  virtual std::string GetCloudDiagnosticsServerUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
    = 0;
  virtual std::string GetPerformanceServerUrl(
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState)
    = 0;
  virtual std::shared_ptr<ServiceDiscoveryDetails>GetServiceDiscoveryDetails(
    const void                             *pbPublishLicense,
    const size_t                            cbPublishLicense,
    const std::string                     & sEmail,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    modernapi::IConsentCallbackImpl        *consentCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState
    ) = 0;

public:

  static std::shared_ptr<IRestServiceUrlClient>Create();
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_IRESTSERVICEURLCLIENT_H_
