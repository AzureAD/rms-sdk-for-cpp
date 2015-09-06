/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "PlatformHttpClientTest.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Http/IHttpClient.h"
#include "../../Common/FrameworkSpecificTypes.h"

using namespace rmscore::platform;

PlatformHttpClientTest::PlatformHttpClientTest()
{}

void PlatformHttpClientTest::testHttpClient(bool enabled)
{
  if (!enabled) return;

  auto pclient = http::IHttpClient::Create();
  rmscore::common::ByteArray response;
  std::string request("any");

  auto url = "https://api.aadrm.com/my/v1/servicediscovery";

  http::StatusCode status = pclient->Get(
    url,
    response,
    nullptr);

  QVERIFY2(status == http::StatusCode::UNAUTHORIZED,
           "pclient->Get: Unexpected status code");

  pclient->AddHeader("content-type", "application/x-www-form-urlencoded");
  status = pclient->Post(
    url,
    rmscore::common::ByteArray(request.begin(), request.end()),
    "any",
    response,
    nullptr);

  QByteArray expected = QString(
    "{\"Message\":\"The authorization token is not well-formed.\"}").toUtf8();
  QByteArray actual((const char *)response.data(), (int)response.size());

  QCOMPARE(expected, actual);

  QVERIFY2(status == http::StatusCode::UNAUTHORIZED,
           "pclient->Post: Unexpected status code");
}
