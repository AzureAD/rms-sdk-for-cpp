/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <RmsauthIdHelper.h>
#include <Constants.h>

namespace rmsauth {

const Headers RmsauthIdHelper::getProductHeaders()
{
    Headers headers;
    headers.insert(std::make_pair(Constants::rmsauthIdParameter().Product, Constants::rmsauthIdParameter().ProductVal));
    headers.insert(std::make_pair(Constants::rmsauthIdParameter().Version, Constants::rmsauthIdParameter().VersionVal));
    return move(headers);
}

const Headers RmsauthIdHelper::getPlatformHeaders()
{
    Headers headers;
    headers.insert(std::make_pair(Constants::rmsauthIdParameter().CpuPlatform, RmsauthIdHelper::getProcessorArchitecture()));
    headers.insert(std::make_pair(Constants::rmsauthIdParameter().OS, RmsauthIdHelper::getOSVersion()));
    return move(headers);
}

} // namespace rmsauth {
