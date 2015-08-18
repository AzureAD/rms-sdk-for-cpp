/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "IRMSAuthEnvironmentImpl.h"

using namespace std;
namespace rmsauth {
shared_ptr<IRMSAuthEnvironmentImpl> _instance =
  make_shared<IRMSAuthEnvironmentImpl>();

IRMSAuthEnvironmentImpl::IRMSAuthEnvironmentImpl()
  : _optLog(static_cast<int>(LoggerOption::Always))
{}

void IRMSAuthEnvironmentImpl::LogOption(LoggerOption opt) {
  _optLog = static_cast<int>(opt);
}

IRMSAuthEnvironment::LoggerOption IRMSAuthEnvironmentImpl::LogOption() {
  return static_cast<LoggerOption>(_optLog.load());
}

shared_ptr<IRMSAuthEnvironment>IRMSAuthEnvironmentImpl::Environment() {
  return std::dynamic_pointer_cast<IRMSAuthEnvironment>(_instance);
}
} // namespace rmsauth
