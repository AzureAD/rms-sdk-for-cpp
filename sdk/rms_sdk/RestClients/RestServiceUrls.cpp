/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <string>
#include "RestServiceUrls.h"
#include "../Platform/Settings/ILocalSettings.h"

using namespace std;
using namespace rmscore::platform::settings;

namespace rmscore {
namespace restclients {
string RestServiceUrls::GetEndUserLicensesUrl()
{
    string sPath = "/enduserlicenses";

    return GetServiceRootUrl() + sPath;
}

string RestServiceUrls::GetTemplatesUrl()
{
    string sPath = "/templates";

    return GetServiceRootUrl() + sPath;
}

string RestServiceUrls::GetPublishUrl()
{
    string sPath = "/publishinglicenses";

    return GetServiceRootUrl() + sPath;
}

string RestServiceUrls::GetCloudDiagnosticsServerUrl()
{
    string sPath = "/clientlogs/debug";

    return GetServiceRootUrl() + sPath;
}

string RestServiceUrls::GetPerformanceServerUrl()
{
    string sPath = "/clientlogs/performance";

    return GetServiceRootUrl() + sPath;
}

string RestServiceUrls::GetServiceDiscoveryUrl()
{
    return GetServiceRootUrl() + GetServiceDiscoverySuffix();
}

string RestServiceUrls::GetDefaultTenant()
{
    return "/my/v1";
}

string RestServiceUrls::GetServiceDiscoverySuffix()
{
    return "/servicediscovery";
}

string RestServiceUrls::GetServiceRootUrl()
{
    string sProductionServiceUrl = "https://api.aadrm.com" + GetDefaultTenant();

    shared_ptr<ILocalSettings> pLocalSettings = ILocalSettings::Create();
    return pLocalSettings->GetString("MSIPCThin", "ServiceRootURLOverride", sProductionServiceUrl);
}
} // namespace restclients
} // namespace rmscore
