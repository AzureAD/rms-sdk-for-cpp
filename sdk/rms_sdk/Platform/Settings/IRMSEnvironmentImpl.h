/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_IRMSENVIRONMENT_IMPL_H
#define _RMS_LIB_IRMSENVIRONMENT_IMPL_H

#include <mutex>
#include <QAtomicInt>

#include "../../ModernAPI/IRMSEnvironment.h"

namespace rmscore {
namespace platform {
namespace settings {
class IRMSEnvironmentImpl : public modernapi::IRMSEnvironment {
public:

  IRMSEnvironmentImpl();
  virtual ~IRMSEnvironmentImpl() {}

  virtual void                                      LogOption(LoggerOption opt);
  virtual LoggerOption                              LogOption();

  static std::shared_ptr<modernapi::IRMSEnvironment>Environment();

private:

  QAtomicInt _optLog;
};

extern std::shared_ptr<IRMSEnvironmentImpl> _instance;
} // namespace settings
} // namespace platform
} // namespace rmscore

#endif // _RMS_LIB_IRMSENVIRONMENT_IMPL_H
