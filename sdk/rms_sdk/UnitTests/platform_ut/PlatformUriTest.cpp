/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "PlatformUriTest.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Http/IUri.h"
#include "../../Common/FrameworkSpecificTypes.h"

using namespace rmscore::platform;

PlatformUriTest::PlatformUriTest()
{}

void PlatformUriTest::testUri(bool enabled)
{
  if (!enabled) return;

  auto url = http::IUri::Create("https://api.aadrm.com/my/v1/servicediscovery");

  auto urlString= url->ToString();
  QVERIFY2(urlString == "https://api.aadrm.com/my/v1/servicediscovery",
           "url->ToString: Uri to string doesn't match uri on creation.");

  auto hostString= url->GetHost();
  auto portString= url->GetPort();
  auto schemeString= url->GetScheme();

  qDebug(hostString);
  qDebug(portString);
  qDebug(schemeString);

}
