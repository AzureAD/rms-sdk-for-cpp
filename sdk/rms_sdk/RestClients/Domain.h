/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_DOMAIN_H_
#define _RMS_LIB_DOMAIN_H_

#include <string>
#include <memory>
#include <vector>

namespace rmscore {
namespace restclients {
enum class DomainType
{
  None,
  Email,
  Url,
};

class Domain {
public:

  static std::shared_ptr<Domain> CreateFromEmail(const std::string &sEmail);
  static std::shared_ptr<Domain> CreateFromUrl(const std::string& sUrl);

  static std::vector<std::string>SplitDomains(const std::string domain);

public:

  std::string GetOriginalInput() const;
  std::string GetDomainStringForDnsLookup() const;
  DomainType  GetType() const;

private:

  Domain();

private:

  DomainType  m_DomainType;
  std::string m_Email;
  std::string m_Url;
  std::string m_RelevantDomain;
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_DOMAIN_H_
