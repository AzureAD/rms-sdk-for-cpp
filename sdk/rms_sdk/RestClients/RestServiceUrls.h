/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_RESTCLIENTURLS_H_
#define _RMS_LIB_RESTCLIENTURLS_H_

#include <string>
namespace rmscore {
namespace restclients {
class RestServiceUrls {
public:

  static std::string GetServiceDiscoveryUrl();
  static std::string GetServiceDiscoverySuffix();
  static std::string GetDefaultTenant();
  static std::string GetEndUserLicensesUrl();
  static std::string GetTemplatesUrl();
  static std::string GetPublishUrl();
  static std::string GetCloudDiagnosticsServerUrl();
  static std::string GetPerformanceServerUrl();

private:

  static std::string GetServiceRootUrl();
};
} // namespace restclients
} // namespace rmscore

#endif // _RMS_LIB_RESTCLIENTURLS_H_
