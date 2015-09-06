/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_ISERVICEDISCOVERYCLIENT_H_
#define _RMS_LIB_ISERVICEDISCOVERYCLIENT_H_

#include "Domain.h"
#include "ServiceDiscoveryDetails.h"
#include "RestObjects.h"
#include "../ModernAPI/IAuthenticationCallbackImpl.h"

#include <string>
#include <memory>
#include <atomic>

namespace rmscore {
namespace restclients {
class IServiceDiscoveryClient {
public:

  virtual ServiceDiscoveryListResponse GetServiceDiscoveryDetails(
    const Domain                          & domain,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & discoveryUrl,
    std::shared_ptr<std::atomic<bool> >     cancelState) = 0;

public:

  static std::shared_ptr<IServiceDiscoveryClient>Create();
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_ISERVICEDISCOVERYCLIENT_H_
