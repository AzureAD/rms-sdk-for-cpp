/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "../Common/FrameworkSpecificTypes.h"
#include "../Common/CommonTypes.h"
#include "../Core/FeatureControl.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Xml/IDomDocument.h"
#include "../Platform/Xml/IDomElement.h"
#include "../Platform/Logger/Logger.h"
#include <QCoreApplication>

#include "CXMLUtils.h"
#include "LicenseParser.h"

using namespace std;
using namespace rmscore::platform::logger;

namespace rmscore 
{
namespace restclients 
{

namespace {
    const string EXTRANET_XPATH =
        "/Root/XrML/BODY[@type='Microsoft Rights Label']/DISTRIBUTIONPOINT/OBJECT[@type='Extranet-License-Acquisition-URL']/ADDRESS[@type='URL']/text()";
    const string INTRANET_XPATH =
        "/Root/XrML/BODY[@type='Microsoft Rights Label']/DISTRIBUTIONPOINT/OBJECT[@type='License-Acquisition-URL']/ADDRESS[@type='URL']/text()";
    const string SLC_XPATH =
        "/Root/XrML/BODY[@type='Microsoft Rights Label']/ISSUEDPRINCIPALS/PRINCIPAL/PUBLICKEY/PARAMETER[@name='modulus']/VALUE/text()";
}

const uint8_t BOM_UTF8[] = {0xef, 0xbb, 0xbf};

const shared_ptr<LicenseParserResult> LicenseParser::ParsePublishingLicense(const void *pbPublishLicense,
                                                                size_t cbPublishLicense)
{
  if(!QCoreApplication::instance()) {
    int argc = 1;
    char name[] = "LicenseParser::ParsePublishingLicense";
    char* argv = &name[0];
    QCoreApplication a(argc, &argv);
    return ParsePublishingLicenseInner(pbPublishLicense, cbPublishLicense);
  }
  return ParsePublishingLicenseInner(pbPublishLicense, cbPublishLicense);
}

const shared_ptr<LicenseParserResult> LicenseParser::ParsePublishingLicenseInner(const void *pbPublishLicense,
                                                                     size_t cbPublishLicense)
{
    string publishLicense;
    if ((cbPublishLicense > sizeof(BOM_UTF8)) && (memcmp(pbPublishLicense, BOM_UTF8, sizeof(BOM_UTF8)) == 0))
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
    //Logger::Hidden("Publishing License in LicenseParser: %s", publishLicense.c_str());
    size_t finalSize = publishLicense.size();

    string publishLicenseWithRoot;
    CXMLUtils::WrapWithRoot(publishLicense.c_str(), finalSize, publishLicenseWithRoot);

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

    auto extranetDomainNode = document->SelectSingleNode(EXTRANET_XPATH);
    auto intranetDomainNode = document->SelectSingleNode(INTRANET_XPATH);

    auto extranetDomain = (nullptr != extranetDomainNode.get()) ? extranetDomainNode->text() : string();
    RemoveTrailingNewLine(extranetDomain);
    auto intranetDomain = (nullptr != intranetDomainNode.get()) ? intranetDomainNode->text() : string();
    RemoveTrailingNewLine(intranetDomain);
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

    shared_ptr<LicenseParserResult> result;
    if (rmscore::core::FeatureControl::IsEvoEnabled())
    {
        auto slcNode = document->SelectSingleNode(SLC_XPATH);
        if (nullptr == slcNode.get())
        {
            throw exceptions::RMSNetworkException("Server public certificate",
                                              exceptions::RMSNetworkException::InvalidPL);
        }
        auto publicCertificate = slcNode->text();
        RemoveTrailingNewLine(publicCertificate);

        result = make_shared<LicenseParserResult>(LicenseParserResult(domains,
                                                                      make_shared<string>(publicCertificate)));
    }
    else
    {
        result = make_shared<LicenseParserResult>(domains);
    }
    return result;
}

void LicenseParser::RemoveTrailingNewLine(string& str)
{
    if (str.empty())
    {
        return;
    }
    if (str.length() > 1)
    {
        // Support linux style newline.
        if ((str[str.length() - 2] == '\r') && (str[str.length() - 1] == '\n'))
        {
            str.erase(str.length() - 2);
            return;
        }
    }
    if (str[str.length() - 1] == '\n')
    {
        str.erase(str.length() - 1);
    }

}

} // namespace restclients
} // namespace rmscore
