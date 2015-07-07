/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <memory>
#include "../ModernAPI/RMSExceptions.h"
#include "DnsClientResult.h"
#include "RestServiceUrls.h"

using namespace std;

namespace rmscore {
namespace restclients {
DnsClientResult::DnsClientResult(const vector<string>& domainsChecked) :
  m_DnsRecordFound(false),
  m_Servers(),
  m_DomainsChecked(domainsChecked),
  m_Ttls()
{
  m_Servers.push_back(RestServiceUrls::GetServiceDiscoveryUrl());
  m_Ttls.push_back(static_cast<uint32_t>(-1));
}

string DnsClientResult::GetDiscoveryUrl()
{
  if (m_Servers.empty()) {
    throw exceptions::RMSInvalidArgumentException("Invalid server");
  }
  return m_Servers[0];
}

vector<string>DnsClientResult::GetDomainsChecked()
{
  return m_DomainsChecked;
}

uint32_t DnsClientResult::GetTtl()
{
  if (m_Ttls.empty()) {
    throw exceptions::RMSInvalidArgumentException("Invalid tls");
  }
  return m_Ttls[0];
}

bool DnsClientResult::DnsRecordFound()
{
  return m_DnsRecordFound;
}

shared_ptr<DnsClientResult>DnsClientResult::Create(const string& discoveryUrl)
{
  shared_ptr<DnsClientResult> dnsClientResult = make_shared<DnsClientResult>(
    vector<string>());
  dnsClientResult->m_Servers.clear();
  dnsClientResult->m_Servers.push_back(discoveryUrl);
  return dnsClientResult;
}
} // namespace restclients
} // namespace rmscore
