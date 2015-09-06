/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_SERVICEDISCOVERYCLIENT_H_
#define _RMS_LIB_SERVICEDISCOVERYCLIENT_H_

#include "IServiceDiscoveryClient.h"
#include "ServiceDiscoveryDetails.h"
#include "Domain.h"
#include "../ModernAPI/IAuthenticationCallbackImpl.h"

#include <memory>

namespace rmscore {
namespace restclients {
class ServiceDiscoveryClient : public IServiceDiscoveryClient {
public:

  virtual ServiceDiscoveryListResponse GetServiceDiscoveryDetails(
    const Domain                          & domain,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & discoveryUrl,
    std::shared_ptr<std::atomic<bool> >     cancelState) override;

private:

  std::string CreateGetRequest(const std::string& discoveryUrl,
                               const Domain     & domain);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_SERVICEDISCOVERYCLIENT_H_
