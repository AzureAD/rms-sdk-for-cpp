/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <sstream>
#include "DnsLookupClient.h"
#include "Domain.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Http/IDnsServerResolver.h"
#include "../Platform/Logger/Logger.h"

using namespace std;
using namespace rmscore::platform::http;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace restclients {
const string RMS_QUERY_PREFIX = "_rmsdisco._http._tcp.";
DnsLookupClient::~DnsLookupClient()
{}

shared_ptr<DnsClientResult>DnsLookupClient::LookupDiscoveryService(
  shared_ptr<Domain>domain)
{
  if (domain.get() == nullptr) {
    throw exceptions::RMSInvalidArgumentException("Invalid domain");
  }

  auto pResolver = IDnsServerResolver::Create();

  string domainString            = domain->GetDomainStringForDnsLookup();
  vector<string> possibleDomains = GetPossibleDomains(domainString);

  for (auto possibleDomainIt = begin(possibleDomains);
       possibleDomainIt != end(possibleDomains); ++possibleDomainIt)
  {
    Logger::Hidden("possibleDomain: %s", possibleDomainIt->c_str());
    string dnsRequest(RMS_QUERY_PREFIX + *possibleDomainIt);
    auto   dnsResponse = pResolver->lookup(dnsRequest);

    if (dnsResponse.empty())
    {
      Logger::Hidden("Failed DNS lookup with domain: %s",
                     possibleDomainIt->c_str());
      continue;
    }

    Logger::Hidden("Successfully queried results with domain: %s",
                   possibleDomainIt->c_str());
    return DnsClientResult::Create(dnsResponse);
  }

  return DnsClientResult::Create(string("api.aadrm.com"));
}

vector<string>DnsLookupClient::GetPossibleDomains(const std::string& domain)
{
  const int MINIMUM_NUMBER_OF_ELEMENTS_IN_DOMAIN = 1;

  vector<string> possibleDomains;
  vector<string> members = Domain::SplitDomains(domain);
  int numberElements     = static_cast<int>(members.size()) -
                           MINIMUM_NUMBER_OF_ELEMENTS_IN_DOMAIN;

  for (int i = 0, lengthFromStart = 0; i < numberElements; ++i)
  {
    possibleDomains.push_back(domain.substr(lengthFromStart));
    lengthFromStart += static_cast<int>(members[i].length()) + 1;
  }

  return possibleDomains;
}

shared_ptr<IDnsLookupClient>IDnsLookupClient::Create()
{
  return shared_ptr<IDnsLookupClient>(new DnsLookupClient());
}
} // namespace restclients
} // namespace rmscore
