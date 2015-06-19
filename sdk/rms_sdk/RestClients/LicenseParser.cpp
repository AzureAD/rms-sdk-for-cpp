/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "../ModernAPI/RMSExceptions.h"
#include "../Common/FrameworkSpecificTypes.h"
#include "../Common/CommonTypes.h"
#include "../Platform/Xml/IDomDocument.h"
#include "../Platform/Xml/IDomElement.h"
#include "LicenseParser.h"
#include "CXMLUtils.h"

using namespace std;

namespace rmscore {
namespace restclients {
const vector<shared_ptr<Domain> >LicenseParser::
ExtractDomainsFromPublishingLicense(const void *pbPublishLicense,
                                    size_t      cbPublishLicense)
{
  return ExtractDomainsFromPublishingLicenseInner(pbPublishLicense,
                                                  cbPublishLicense);
}

const vector<shared_ptr<Domain> >LicenseParser::
ExtractDomainsFromPublishingLicenseInner(const void *pbPublishLicense,
                                         size_t      cbPublishLicense)
{
  string publishLicense(reinterpret_cast<const uint8_t *>(pbPublishLicense),
                        reinterpret_cast<const uint8_t *>(pbPublishLicense) +
                        cbPublishLicense);
  size_t finalSize = publishLicense.size();

  string publishLicenseWithRoot;
  CXMLUtils::WrapWithRoot(
    publishLicense.c_str(), finalSize, publishLicenseWithRoot);

  string extranetXpath =
    "/Root/XrML/BODY[@type='Microsoft Rights Label']/DISTRIBUTIONPOINT/OBJECT[@type='Extranet-License-Acquisition-URL']/ADDRESS[@type='URL']/text()";
  string intranetXpath =
    "/Root/XrML/BODY[@type='Microsoft Rights Label']/DISTRIBUTIONPOINT/OBJECT[@type='License-Acquisition-URL']/ADDRESS[@type='URL']/text()";

  auto document = IDomDocument::create();
  std::string errMsg;
  int errLine   = 0;
  int errColumn = 0;

  auto ok = document->setContent(publishLicenseWithRoot,
                                 errMsg,
                                 errLine,
                                 errColumn);

  if (!ok) {
    throw exceptions::RMSNetworkException("Invalid publishing license",
                                          exceptions::RMSNetworkException::InvalidPL);
  }

  auto extranetDomainNode = document->SelectSingleNode(extranetXpath);
  auto intranetDomainNode = document->SelectSingleNode(intranetXpath);

  string extranetDomain = (nullptr != extranetDomainNode.get())
                          ? extranetDomainNode->text()
                          : string();

  string intranetDomain = (nullptr != intranetDomainNode.get())
                          ? intranetDomainNode->text()
                          : string();

  vector<shared_ptr<Domain> > domains;

  if (!extranetDomain.empty())
  {
    domains.push_back(Domain::CreateFromUrl(extranetDomain));
  }

  if (!intranetDomain.empty())
  {
    // don't add the intranet domain if it's the same as extranet
    if (extranetDomain.empty() ||
        (0 != _stricmp(intranetDomain.data(), extranetDomain.data())))
    {
      domains.push_back(Domain::CreateFromUrl(intranetDomain));
    }
  }

  if (domains.empty()) {
    throw exceptions::RMSNetworkException("Invalid domains publishing license",
                                          exceptions::RMSNetworkException::InvalidPL);
  }

  return domains;
}
} // namespace restclients
} // namespace rmscore
