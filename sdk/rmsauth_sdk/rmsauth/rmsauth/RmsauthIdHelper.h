/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMSAUTHIDHELPER
#define RMSAUTHIDHELPER

#include "types.h"

namespace rmsauth {

class RmsauthIdHelper
{
public:
    static const Headers getProductHeaders();
    static const Headers getPlatformHeaders();

private:
    static const String getProcessorArchitecture();
    static const String getOSVersion();
};

} // namespace rmsauth {

#endif // RMSAUTHIDHELPER
