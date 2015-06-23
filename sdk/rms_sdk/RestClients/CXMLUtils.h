/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_CXMLUTILS_H_
#define _RMS_LIB_CXMLUTILS_H_

#include <string>
#include <vector>

namespace rmscore {
namespace restclients {
class CXMLUtils {
public:

  static bool FindTag(const char        *szXML,
                      const std::string& wstrTagName,
                      const char *     & szStart,
                      const char *     & szEnd);

  static bool FindElement(const char        *szXML,
                          const std::string& strElementName,
                          const char *     & szStartOuter,
                          const char *     & szEndOuter,
                          const char *     & szStartInner,
                          const char *     & szEndInner);

  static std::string ExtractElementFirst(const char        *szXML,
                                         const std::string& wstrElementName,
                                         bool               includeTag = true);

  static void ExtractElementAll(const char               *szXML,
                                const std::string       & strElementName,
                                std::vector<std::string>& vOccurrences,
                                bool                      includeTag = true);

  static void WrapWithRoot(const char  *szXML,
                           size_t       xmlSize,
                           std::string& wstrWrapped);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_CXMLUTILS_H_
