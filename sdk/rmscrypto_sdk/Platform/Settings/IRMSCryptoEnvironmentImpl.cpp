/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "IRMSCryptoEnvironmentImpl.h"

using namespace std;
namespace rmscrypto {
namespace platform {
namespace settings {
shared_ptr<IRMSCryptoEnvironmentImpl> _instance = make_shared<IRMSCryptoEnvironmentImpl>();

IRMSCryptoEnvironmentImpl::IRMSCryptoEnvironmentImpl()
  : _optLog(static_cast<int>(LoggerOption::Always))
{}

void IRMSCryptoEnvironmentImpl::LogOption(LoggerOption opt) {
  _optLog = static_cast<int>(opt);
}

api::IRMSCryptoEnvironment::LoggerOption IRMSCryptoEnvironmentImpl::LogOption() {
  return static_cast<LoggerOption>(_optLog.load());
}

shared_ptr<api::IRMSCryptoEnvironment>IRMSCryptoEnvironmentImpl::Environment() {
  return std::dynamic_pointer_cast<api::IRMSCryptoEnvironment>(
    platform::settings::_instance);
}
} // namespace settings
} // namespace platform
} // namespace rmscrypto
