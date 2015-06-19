/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IDNSLOOKUPCLIENT_H_
#define _RMS_LIB_IDNSLOOKUPCLIENT_H_

#include "Domain.h"
#include "DnsClientResult.h"

#include <memory>

namespace rmscore {
namespace restclients {
class IDnsLookupClient {
public:

  virtual ~IDnsLookupClient() {}

  virtual std::shared_ptr<DnsClientResult>LookupDiscoveryService(
    std::shared_ptr<Domain>domain) = 0;

public:

  static std::shared_ptr<IDnsLookupClient>Create();
};
} // namespace restclients
} // namespace rmscore

#endif // _RMS_LIB_IDNSLOOKUPCLIENT_H_
