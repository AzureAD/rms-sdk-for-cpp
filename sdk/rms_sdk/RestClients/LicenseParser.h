/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_LICENSEPARSER_H_
#define _RMS_LIB_LICENSEPARSER_H_

#include <memory>
#include <vector>

#include "Domain.h"
#include "LicenseParserResult.h"

using namespace std;

namespace rmscore {
namespace restclients {

class LicenseParser {
public:

  static const shared_ptr<LicenseParserResult> ParsePublishingLicense(const void *pbPublishLicense,
                                                                      size_t cbPublishLicense);

private:

  static bool IsValidUTF16LEPLStart(const void* pbPublishLicense,
                                    size_t cbPublishLicense);

  static const shared_ptr<LicenseParserResult> ParsePublishingLicenseInner(const void* pbPublishLicense,
                                                                           size_t cbPublishLicense);

  static void RemoveTrailingNewLine(string& str);
};

} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_LICENSEPARSER_H_
