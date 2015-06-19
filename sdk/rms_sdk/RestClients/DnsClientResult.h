/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_DNSCLIENTRESULT_H_
#define _RMS_LIB_DNSCLIENTRESULT_H_

#include <string>
#include <memory>
#include <vector>

namespace rmscore {
namespace restclients {
class DnsClientResult {
public:

  static std::shared_ptr<DnsClientResult>Create(const std::string& discoveryUrl);

public:

  DnsClientResult(const std::vector<std::string>& domainsChecked);

  std::string             GetDiscoveryUrl();
  std::vector<std::string>GetDomainsChecked();
  uint32_t                GetTtl();
  bool                    DnsRecordFound();

private:

  bool m_DnsRecordFound;
  std::vector<std::string> m_Servers;
  std::vector<std::string> m_DomainsChecked;
  std::vector<uint32_t>    m_Ttls;
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_DNSCLIENTRESULT_H_
