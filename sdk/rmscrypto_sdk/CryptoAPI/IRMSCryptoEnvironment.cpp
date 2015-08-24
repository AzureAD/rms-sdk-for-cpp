/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "IRMSCryptoEnvironment.h"
#include "../Platform/Settings/IRMSCryptoEnvironmentImpl.h"

using namespace std;
namespace rmscrypto {
namespace api {
shared_ptr<IRMSCryptoEnvironment>RMSCryptoEnvironment() {
  return platform::settings::IRMSCryptoEnvironmentImpl::Environment();
}
} // namespace api
} // namespace rmscrypto
