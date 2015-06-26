/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_DNSLOOKUPCLIENT_H_
#define _RMS_LIB_DNSLOOKUPCLIENT_H_

#include <memory>
#include "../Common/FrameworkSpecificTypes.h"
#include "../Common/CommonTypes.h"
#include "IDnsLookupClient.h"

namespace rmscore {
namespace restclients {
class DnsLookupClient : public IDnsLookupClient {
public:

  virtual ~DnsLookupClient();

  virtual std::shared_ptr<DnsClientResult>LookupDiscoveryService(
    std::shared_ptr<Domain>domain) override;

private:
  void SendPacket(
    common::DataStream &sendStream,
    const common::ByteArray &requestMessage);

  common::StringArray GetPossibleDomains(const std::string& domain);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_DNSLOOKUPCLIENT_H_
