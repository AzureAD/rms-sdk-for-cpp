/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_LICENSEPARSER_H_
#define _RMS_LIB_LICENSEPARSER_H_

#include "Domain.h"
#include <vector>
#include <memory>
namespace rmscore {
namespace restclients {
class LicenseParser {
public:

  static const std::vector<std::shared_ptr<Domain> >
  ExtractDomainsFromPublishingLicense(const void *pbPublishLicense,
                                      size_t      cbPublishLicense);

private:

  static bool IsValidUTF16LEPLStart(
    const void *pbPublishLicense,
    size_t      cbPublishLicense);
  static const std::vector<std::shared_ptr<Domain> >
  ExtractDomainsFromPublishingLicenseInner(const void *pbPublishLicense,
                                           size_t      cbPublishLicense);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_LICENSEPARSER_H_
