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
shared_ptr<IRMSEnvironment>IRMSEnvironment::Environment() {
  return std::dynamic_pointer_cast<IRMSEnvironment>(platform::settings::_instance);
}
} // namespace modernapi
} // namespace rmscore
