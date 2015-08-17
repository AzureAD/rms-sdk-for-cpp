/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMSAUTH_LIB_IRMSENVIRONMENT_H
#define _RMSAUTH_LIB_IRMSENVIRONMENT_H

#include <memory>
#include "rmsauthExport.h"

namespace rmsauth {
class IRMSAuthEnvironment {
public:

  virtual ~IRMSAuthEnvironment() {}

  enum class LoggerOption : int { Always, Never };
  virtual void                                 LogOption(LoggerOption opt) = 0;
  virtual LoggerOption                         LogOption()                 = 0;
};

RMSAUTH_EXPORT std::shared_ptr<IRMSAuthEnvironment>RMSAuthEnvironment();
} // namespace rmsauth

#endif // _RMSAUTH_LIB_IRMSENVIRONMENT_H
