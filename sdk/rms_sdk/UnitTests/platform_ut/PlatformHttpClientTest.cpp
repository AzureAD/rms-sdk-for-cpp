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

using namespace rmscore::platform;

// PlatformHttpClientTest::PlatformHttpClientTest()
// {}

TEST_P (PlatformHttpClientTest, testHttpClient) {
  bool enabled = GetParam();
  if (!enabled) return;

  auto pclient = http::IHttpClient::Create();
  rmscore::common::ByteArray response;
  std::string request("any");

  auto url = "https://api.aadrm.com/my/v1/servicediscovery";

  http::StatusCode status = pclient->Get(
    url,
    response,
    nullptr);

  EXPECT_EQ(status, http::StatusCode::UNAUTHORIZED) << "pclient->Get: Unexpected status code";

  pclient->AddHeader("content-type", "application/x-www-form-urlencoded");
  status = pclient->Post(
    url,
    rmscore::common::ByteArray(request.begin(), request.end()),
    "any",
    response,
    nullptr);
  unsigned char data[] = {
    0x7B, 0x5C, 0x22, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x655C223A5C2254686520617574686F72697A6174696F6E20746F6B656E206973206E6F742077656C6C2D666F726D65642E5C227D22
  }

  QByteArray expected = QString(
    "{\"Message\":\"The authorization token is not well-formed.\"}").toUtf8();
  QByteArray actual((const char *)response.data(), (int)response.size());

  EXPECT_EQ(expected, actual);

  EXPECT_EQ(status, http::StatusCode::UNAUTHORIZED) << "pclient->Post: Unexpected status code";
}
