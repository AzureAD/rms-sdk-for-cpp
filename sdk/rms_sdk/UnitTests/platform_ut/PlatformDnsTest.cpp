/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */
/*
#include "PlatformDnsTest.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Http/IDnsServerResolver.h"

using namespace rmscore::platform;

PlatformDnsTest::PlatformDnsTest()
{}

void PlatformDnsTest::testDns(bool enabled)
{
  if (!enabled) return;

  auto dnsResolver= http::IDnsServerResolver::Create();
  auto response =dnsResolver->lookup("_rmsdisco._http._tcp.microsoft.com");

  QVERIFY2(response == "api.aadrm.com", response.c_str());

}
*/
