/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "IRMSEnvironment.h"
#include "../Platform/Settings/IRMSEnvironmentImpl.h"

using namespace std;
namespace rmscore {
namespace modernapi {
shared_ptr<IRMSEnvironment>RMSEnvironment() {
  return platform::settings::IRMSEnvironmentImpl::Environment();
}
} // namespace modernapi
} // namespace rmscore
