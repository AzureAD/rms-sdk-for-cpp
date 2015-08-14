/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "IRMSEnvironmentImpl.h"

using namespace std;
namespace rmscore {
namespace platform {
namespace settings {
shared_ptr<IRMSEnvironmentImpl> _instance = make_shared<IRMSEnvironmentImpl>();

IRMSEnvironmentImpl::IRMSEnvironmentImpl()
  : _optLog(static_cast<int>(LoggerOption::Always))
{}

void IRMSEnvironmentImpl::LogOption(LoggerOption opt) {
  _optLog = static_cast<int>(opt);
}

modernapi::IRMSEnvironment::LoggerOption IRMSEnvironmentImpl::LogOption() {
  return static_cast<LoggerOption>(static_cast<int>(_optLog));
}
} // namespace settings
} // namespace platform
} // namespace rmscore
