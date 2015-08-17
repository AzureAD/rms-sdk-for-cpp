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
shared_ptr<IRMSAuthEnvironment>RMSAuthEnvironment() {
  return IRMSAuthEnvironmentImpl::Environment();
}
} // namespace rmsauth
