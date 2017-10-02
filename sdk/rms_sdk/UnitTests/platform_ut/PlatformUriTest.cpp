/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "PlatformUriTest.h"
#include "../../Platform/Http/IUri.h"
#include "../../Platform/Logger/Logger.h"

using namespace rmscore::platform;

PlatformUriTest::PlatformUriTest()
{}

void PlatformUriTest::testUri(bool enabled) {
  if (!enabled)
    return;
  auto url = http::IUri::Create("https://api.aadrm.com:443/");
  auto urlString = url->ToString();
  QVERIFY2(urlString == "https://api.aadrm.com:443/",
           "url->ToString: Uri to string doesn't match uri on creation.");
  auto hostString = url->GetHost();
  QVERIFY2(hostString == "api.aadrm.com",
           "url->GetHost: Uri to host doesn't match host on creation.");
  int port= url->GetPort();
  QVERIFY2(port==443,
           "url->GetPort: Uri to port doesn't match port on creation.");
  auto schemeString= url->GetScheme();
  QVERIFY2(schemeString == "https",
           "url->GetScheme: Uri to scheme doesn't match scheme on creation.");
}

