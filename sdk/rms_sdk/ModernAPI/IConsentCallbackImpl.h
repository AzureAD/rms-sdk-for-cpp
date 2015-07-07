/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_ICONSENTCALLBACKIMPL_H_
#define _RMS_LIB_ICONSENTCALLBACKIMPL_H_
#include <vector>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
class IConsentCallbackImpl {
public:

  virtual void Consents(const std::string             & email,
                        const std::string             & domain,
                        const std::vector<std::string>& urls) = 0;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_ICONSENTCALLBACKIMPL_H_
