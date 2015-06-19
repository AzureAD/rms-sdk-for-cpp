/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_SERVICEDISCOVERYDETAILS_H_
#define _RMS_LIB_SERVICEDISCOVERYDETAILS_H_
#include <stdint.h>
#include <string>

namespace rmscore {
namespace restclients {
struct ServiceDiscoveryDetails
{
  std::string EndUserLicensesUrl;
  std::string TemplatesUrl;
  std::string PublishingLicensesUrl;
  std::string CloudDiagnosticsServerUrl;
  std::string PerformanceServerUrl;
  std::string Domain;
  uint32_t    Ttl;
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_SERVICEDISCOVERYDETAILS_H_
