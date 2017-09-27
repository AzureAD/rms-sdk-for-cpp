/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PlatformHttpClientTest.h"

#include "gmock/gmock.h"

#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Http/IHttpClient.h"

using namespace rmscore::platform;

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
  // the data is {"Message":"The authorization token is not well-formed."} from utf8 to hex
  rmscore::common::ByteArray expectedRes {
    0x7B, 0x22, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x22, 0x3A, 0x22, 0x54, 0x68, 0x65,
    0x20, 0x61, 0x75, 0x74, 0x68, 0x6F, 0x72, 0x69, 0x7A, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20,
    0x74, 0x6F, 0x6B, 0x65, 0x6E, 0x20, 0x69, 0x73, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x77, 0x65,
    0x6C, 0x6C, 0x2D, 0x66, 0x6F, 0x72, 0x6D, 0x65, 0x64, 0x2E, 0x22, 0x7D
  };

  EXPECT_THAT(response, testing::ContainerEq(expectedRes));

  EXPECT_EQ(status, http::StatusCode::UNAUTHORIZED) << "pclient->Post: Unexpected status code";
}
