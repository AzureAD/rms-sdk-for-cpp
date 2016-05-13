/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "LicenseParserTest.h"
#include "LicenseParserTestConstants.h"
#include "../../RestClients/LicenseParser.h"
#include "../../Common/CommonTypes.h"
#include <QFile>
#include <sstream>

using namespace std;
using namespace rmscore::common;
using namespace rmscore::restclients;

void LicenseParserTest::test_UTF16LE_License()
{
    shared_ptr<void> spData(reinterpret_cast<void*>(new uint8_t[PL_0101right_ECB_xml_len]));
    memcpy(spData.get(), PL_0101right_ECB_xml, PL_0101right_ECB_xml_len);
    auto vect_domains = LicenseParser::ExtractDomainsFromPublishingLicense(spData.get(), PL_0101right_ECB_xml_len); 
}

void LicenseParserTest::test_UTF8_License()
{
    shared_ptr<void> spData(reinterpret_cast<void*>(new uint8_t[PL_0101right_CBC_xml_len]));
    memcpy(spData.get(), PL_0101right_CBC_xml, PL_0101right_CBC_xml_len);
    auto vect_domains = LicenseParser::ExtractDomainsFromPublishingLicense(spData.get(), PL_0101right_CBC_xml_len);
}
