/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <string>
#include <sstream>
#include <vector>

#include "Domain.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Http/IUri.h"

using namespace std;
using namespace rmscore::platform::http;

namespace rmscore {
namespace restclients {
Domain::Domain() :
  m_DomainType(DomainType::None),
  m_Email(),
  m_Url(),
  m_RelevantDomain()
{}

shared_ptr<Domain>Domain::CreateFromEmail(const string& sEmail)
{
  shared_ptr<Domain> domain(new Domain());
  domain->m_Email      = string(sEmail);
  domain->m_DomainType = DomainType::Email;

  auto index = domain->m_Email.find("@");

  if (index == string::npos) {
    throw exceptions::RMSInvalidArgumentException("Bad email value");
  }

  domain->m_RelevantDomain = domain->m_Email.substr(index + 1);
  return domain;
}

shared_ptr<Domain>Domain::CreateFromUrl(const string& sUrl)
{
  shared_ptr<Domain> domain(new Domain());
  domain->m_Url        = string(sUrl);
  domain->m_DomainType = DomainType::Url;

  auto uri = IUri::Create(sUrl);
  domain->m_RelevantDomain = uri->GetHost();

  return domain;
}

string Domain::GetOriginalInput() const
{
  switch (m_DomainType)
  {
  case DomainType::Email:
    return m_Email;

  case DomainType::Url:
    return m_Url;

  default:
    return "";
  }
}

string Domain::GetDomainStringForDnsLookup() const
{
  return m_RelevantDomain;
}

DomainType Domain::GetType() const
{
  return m_DomainType;
}

vector<string>Domain::SplitDomains(const string domain)
{
  vector<string> domains;
  stringstream   ss(domain);
  string item;

  while (getline(ss, item, '.'))
  {
    domains.push_back(item);
  }

  return domains;
}
} // namespace restclients
} // namespace rmscore
