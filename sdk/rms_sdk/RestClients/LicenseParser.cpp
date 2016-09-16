/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "../ModernAPI/RMSExceptions.h"
#include "../Common/FrameworkSpecificTypes.h"
#include "../Common/CommonTypes.h"
#include "../Common/Constants.h"
#include "../Platform/Json/IJsonParser.h"
#include "../Platform/Json/IJsonObject.h"
#include "../Platform/Xml/IDomDocument.h"
#include "../Platform/Xml/IDomElement.h"
#include "../Common/tools.h"
#include "LicenseParser.h"
#include "CXMLUtils.h"

using namespace std;

namespace rmscore 
{
namespace restclients 
{
const uint8_t BOM_UTF8[] = {0xef, 0xbb, 0xbf};

const vector<shared_ptr<Domain> >LicenseParser::
ExtractDomainsFromPublishingLicense(const void *pbPublishLicense,
                                    size_t      cbPublishLicense)
{
    return ExtractDomainsFromPublishingLicenseInner(pbPublishLicense,
                                                  cbPublishLicense);
}

const vector<shared_ptr<Domain> >LicenseParser::
ExtractDomainsFromPublishingLicenseInner(const void *pbPublishLicense,
                                         size_t      cbPublishLicense)
{
    string publishLicense;
    if ((cbPublishLicense > sizeof(BOM_UTF8)) &&
        (memcmp(pbPublishLicense, BOM_UTF8, sizeof(BOM_UTF8)) == 0))
    {
        string utf8NoBOM(reinterpret_cast<const uint8_t *>(pbPublishLicense) + sizeof(BOM_UTF8),
                        reinterpret_cast<const uint8_t *>(pbPublishLicense) +
                        cbPublishLicense);
        publishLicense = utf8NoBOM;
    }
    else if (cbPublishLicense % 2 == 0)
    {
        // Assume UTF16LE (Unicode)
        auto strUnicode = QString::fromUtf16(reinterpret_cast<const uint16_t*>(pbPublishLicense), 
                                                              static_cast<int>(cbPublishLicense) / sizeof(uint16_t));
        auto utf8ByteArray = strUnicode.toUtf8();
        string utfString(utf8ByteArray.constData(), utf8ByteArray.length());
        publishLicense = utfString;
    }
    else 
    {
        throw exceptions::RMSNetworkException("Invalid publishing license encoding",
                                              exceptions::RMSNetworkException::InvalidPL);
    }
    if (common::isJson(publishLicense))
    {
        auto parser = platform::json::IJsonParser::Create();
        auto PL = parser->Parse(publishLicense);

        auto s = PL->GetNamedString(common::JsonConstants::PAYLOAD);
        auto pld = parser->Parse(s);
        auto lic = pld->GetNamedObject(common::JsonConstants::LICENSE);
        auto cre = lic->GetNamedObject(common::JsonConstants::CREATOR);
        auto cpld = parser->Parse(cre->GetNamedString(common::JsonConstants::PAYLOAD));
        auto pIssuer = cpld->GetNamedObject(common::JsonConstants::ISSUER);

        vector<shared_ptr<Domain> > domains;
        string eurl = pIssuer->GetNamedString(common::JsonConstants::EXTRANET_URL);
        string iurl = pIssuer->GetNamedString(common::JsonConstants::INTRANET_URL);

        domains.push_back(Domain::CreateFromUrl(eurl));
        if (_stricmp(eurl.data(), iurl.data()) != 0)
            domains.push_back(Domain::CreateFromUrl(iurl));

        if (domains.empty())
            throw exceptions::RMSNetworkException("Invalid domains publishing license",
                                              exceptions::RMSNetworkException::InvalidPL);
        return domains;
    }
    else
    {
        size_t finalSize = publishLicense.size();
        string publishLicenseWithRoot;
        CXMLUtils::WrapWithRoot(publishLicense.c_str(), finalSize, publishLicenseWithRoot);

        string extranetXpath =
            "/Root/XrML/BODY[@type='Microsoft Rights Label']/DISTRIBUTIONPOINT/OBJECT[@type='Extranet-License-Acquisition-URL']/ADDRESS[@type='URL']/text()";
        string intranetXpath =
            "/Root/XrML/BODY[@type='Microsoft Rights Label']/DISTRIBUTIONPOINT/OBJECT[@type='License-Acquisition-URL']/ADDRESS[@type='URL']/text()";

        auto document = IDomDocument::create();
        std::string errMsg;
        int errLine   = 0;
        int errColumn = 0;

        auto ok = document->setContent(publishLicenseWithRoot,
                                     errMsg,
                                     errLine,
                                     errColumn);

        if (!ok)
        {
            throw exceptions::RMSNetworkException("Invalid publishing license",
                                              exceptions::RMSNetworkException::InvalidPL);
        }

        auto extranetDomainNode = document->SelectSingleNode(extranetXpath);
        auto intranetDomainNode = document->SelectSingleNode(intranetXpath);

        string extranetDomain = (nullptr != extranetDomainNode.get())
                              ? extranetDomainNode->text()
                              : string();
        string intranetDomain = (nullptr != intranetDomainNode.get())
                              ? intranetDomainNode->text()
                              : string();

        vector<shared_ptr<Domain> > domains;

        if (!extranetDomain.empty())
        {
            domains.push_back(Domain::CreateFromUrl(extranetDomain));
        }

        if (!intranetDomain.empty())
        {
            // don't add the intranet domain if it's the same as extranet
            if (extranetDomain.empty() ||
               (0 != _stricmp(intranetDomain.data(), extranetDomain.data())))
            {
                domains.push_back(Domain::CreateFromUrl(intranetDomain));
            }
        }
        if (domains.empty())
        {
            throw exceptions::RMSNetworkException("Invalid domains publishing license",
                                              exceptions::RMSNetworkException::InvalidPL);
        }
        return domains;
    }
}
} // namespace restclients
} // namespace rmscore
