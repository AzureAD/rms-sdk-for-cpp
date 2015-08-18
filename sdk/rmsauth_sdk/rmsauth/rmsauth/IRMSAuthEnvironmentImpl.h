/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMSAUTH_LIB_IRMSENVIRONMENT_IMPL_H
#define _RMSAUTH_LIB_IRMSENVIRONMENT_IMPL_H

#include <mutex>
#include <QAtomicInt>

#include "IRMSAuthEnvironment.h"

namespace rmsauth {
class IRMSAuthEnvironmentImpl : public IRMSAuthEnvironment {
public:

  IRMSAuthEnvironmentImpl();
  virtual ~IRMSAuthEnvironmentImpl() {}

  virtual void                                      LogOption(LoggerOption opt);
  virtual LoggerOption                              LogOption();

  static std::shared_ptr<IRMSAuthEnvironment>Environment();

private:

  QAtomicInt _optLog;
};

extern std::shared_ptr<IRMSAuthEnvironmentImpl> _instance;
} // namespace rmsauth

#endif // _RMSAUTH_LIB_IRMSENVIRONMENT_IMPL_H
