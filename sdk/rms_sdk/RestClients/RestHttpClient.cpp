/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <numeric>
#include "RestHttpClient.h"
#include "AuthenticationHandler.h"
#include "../Common/tools.h"
#include "../Platform/Http/IHttpClient.h"
#include "../Platform/Settings/ILanguageSettings.h"
#include "../Platform/Logger/Logger.h"

using namespace std;
using namespace rmscore::common;
using namespace rmscore::modernapi;
using namespace rmscore::platform::http;
using namespace rmscore::platform::settings;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace restclients {
RestHttpClient::Result RestHttpClient::Get(
  const std::string                & sUrl,
  IAuthenticationCallbackImpl      & authenticationCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  // Performance latency should exclude the time it takes in Authentication and
  // consent operations
  auto accessToken = AuthenticationHandler::GetAccessTokenForUrl(sUrl,
                                                                 authenticationCallback,
                                                                 cancelState);
  auto parameters = HttpRequestParameters {
    HTTP_GET,            // type
    string(sUrl),        // url
    common::ByteArray(), // requestBody
    accessToken,         // accessToken
    cancelState
  };

  // call the DoHttpRequest() and abandon the call when the cancel event is
  // signalled (for Office scenarios)
  return RestHttpClient::DoHttpRequest(move(parameters));
}

RestHttpClient::Result RestHttpClient::Post(
  const string                     & sUrl,
  ByteArray                       && requestBody,
  IAuthenticationCallbackImpl      & authenticationCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  // Performance latency should exclude the time it takes in Authentication and
  // consent operations
  auto accessToken = AuthenticationHandler::GetAccessTokenForUrl(sUrl,
                                                                 authenticationCallback,
                                                                 cancelState);
  auto parameters = HttpRequestParameters {
    HTTP_POST,         // type
    string(sUrl),      // url
    move(requestBody), // requestBody
    accessToken,       // accessToken
    cancelState
  };

  // call the DoHttpRequest() and abandon the call when the cancel event is
  // signalled (for Office scenarios)
  return RestHttpClient::DoHttpRequest(move(parameters));
}

RestHttpClient::Result RestHttpClient::DoHttpRequest(
  const HttpRequestParameters& parameters)
{
  shared_ptr<IHttpClient> pHttpClient = IHttpClient::Create();

  // generate a request id (i.e., a new ScenarioId and a new CorrelationId)
  string requestId = GenerateRequestId();

  // Add headers
  pHttpClient->AddAcceptMediaTypeHeader("application/json");
  pHttpClient->AddHeader("content-type", "application/json");
  pHttpClient->AddAcceptLanguageHeader(ConstructLanguageHeader());
  pHttpClient->AddAuthorizationHeader(ConstructAuthTokenHeader(parameters.
                                                               accessToken));
  pHttpClient->AddHeader("x-ms-rms-request-id", requestId);

  // x-ms-rms-platform-id header consists of AppName, AppVersion, SDKVersion,
  // AppPublisherId, AppPublisherName, UniqueClientID
  if (platformIdHeaderCache.size() == 0)
  {
    platformIdHeaderCache = GetPlatformIdHeader();
  }

  pHttpClient->AddHeader("x-ms-rms-platform-id", platformIdHeaderCache);

  Result result;

  switch (parameters.type)
  {
  case HTTP_POST:

    Logger::Hidden(
      "RestHttpClient::DoHttpRequest doing http POST to %s, Request-ID: %s",
      parameters.requestUrl.c_str(),
      requestId.c_str());

    result.status = pHttpClient->Post(
      parameters.requestUrl,
      parameters.requestBody, std::string("application/json"),
      result.responseBody,
      parameters.cancelState);

    break;

  case HTTP_GET:

    Logger::Hidden(
      "RestHttpClient::DoHttpRequest doing http GET to %s, Request-ID: %s",
      parameters.requestUrl.c_str(),
      requestId.c_str());

    result.status = pHttpClient->Get(
      parameters.requestUrl, result.responseBody,
      parameters.cancelState);

    break;
  }

  Logger::Hidden("RestHttpClient::DoHttpRequest returned status code: %d",
                 (int)result.status);

  return result;
}

string RestHttpClient::ConstructAuthTokenHeader(const string& accessToken)
{
  // prefix with "Bearer "
  return "Bearer " + accessToken;
}

string RestHttpClient::ConstructLanguageHeader()
{
  shared_ptr<ILanguageSettings> pLanguageSettings = ILanguageSettings::Create();

  // get the ordered list of the languages supported by the app
  vector<string> appLanguages = pLanguageSettings->GetAppLanguages();

  // construct the header
  return accumulate(begin(appLanguages), end(appLanguages), string(),
                    [](const string& str1, const string& str2) -> string
      {
        // join with comma
        return str1.empty() ? str2 : str1 + ", " + str2;
      });
}

string RestHttpClient::GenerateRequestId()
{
  return GenerateAGuid() + ";" + GenerateAGuid();
}

string RestHttpClient::GetPlatformIdHeader()
{
  string platformId(
    "AppName=rmscore;AppVersion=1.0;DevicePlatform=WindowsStore;SDKVersion=4.1;");

  return platformId;
}

string RestHttpClient::platformIdHeaderCache = GetPlatformIdHeader();
} // namespace restclients
} // namespace rmscore
