/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_IRMSENVIRONMENT_H
#define _RMS_LIB_IRMSENVIRONMENT_H

#include <memory>

#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
class IRMSEnvironment {
public:

  virtual ~IRMSEnvironment() {}

  enum class LoggerOption : int { Always, Never };
  virtual void                                 LogOption(LoggerOption opt) = 0;
  virtual LoggerOption                         LogOption()                 = 0;
};

DLL_PUBLIC_RMS std::shared_ptr<IRMSEnvironment>RMSEnvironment();
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_IRMSENVIRONMENT_H
