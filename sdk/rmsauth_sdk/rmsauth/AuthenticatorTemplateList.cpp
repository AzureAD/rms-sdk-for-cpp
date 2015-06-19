/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AuthenticatorTemplateList.h>

namespace rmsauth {

AuthenticatorTemplateList::AuthenticatorTemplateList()
{
    StringArray trustedHostList = { "login.windows.net", "login.chinacloudapi.cn", "login.cloudgovapi.us", "login.microsoftonline.com" };

// TODO.shch: implement PlatformSpecificHelper.GetEnvironmentVariable("customTrustedHost")
//        string customAuthorityHost = PlatformSpecificHelper.GetEnvironmentVariable("customTrustedHost");

    String customAuthorityHost;
    if (customAuthorityHost.empty())
    {
        for (String host : trustedHostList)
        {
            this->push_back(AuthenticatorTemplate::createFromHost(host));
        }
    }
    else
    {
        this->push_back(AuthenticatorTemplate::createFromHost(customAuthorityHost));
    }
}
AuthenticatorTemplatePtr AuthenticatorTemplateList::findMatchingItemAsync(bool validateAuthority, const String& host, const String& tenant, CallStatePtr callState)
{
    if (validateAuthority)
    {
        for(ArrayList<AuthenticatorTemplatePtr>::size_type i = 0; i < this->size(); ++i)
        {
            auto authenticatorTemplate = this->at(i);
            if(authenticatorTemplate->host() == host)
            {
                return authenticatorTemplate;
            }
        }

        this->at(0)->verifyAnotherHostByInstanceDiscoveryAsync(host, tenant, callState);
    }

    return AuthenticatorTemplate::createFromHost(host);
}

} // namespace rmsauth {
