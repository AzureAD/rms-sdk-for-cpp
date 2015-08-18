/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <IRMSCryptoEnvironment.h>
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

  // setup crypto environment too
  auto cryptoEnv =   rmscrypto::api::RMSCryptoEnvironment();

  switch (opt) {
  case LoggerOption::Always:
    cryptoEnv->LogOption(
      rmscrypto::api::IRMSCryptoEnvironment::LoggerOption::Always);
    break;

  case LoggerOption::Never:
    cryptoEnv->LogOption(
      rmscrypto::api::IRMSCryptoEnvironment::LoggerOption::Never);
    break;
  }
}

modernapi::IRMSEnvironment::LoggerOption IRMSEnvironmentImpl::LogOption() {
  return static_cast<LoggerOption>(_optLog.load());
}

shared_ptr<modernapi::IRMSEnvironment>IRMSEnvironmentImpl::Environment() {
  return std::dynamic_pointer_cast<modernapi::IRMSEnvironment>(
    platform::settings::_instance);
}
} // namespace settings
} // namespace platform
} // namespace rmscore
