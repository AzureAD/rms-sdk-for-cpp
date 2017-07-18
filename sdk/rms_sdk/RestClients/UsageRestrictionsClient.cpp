/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <memory>

#include "../Json/jsonserializer.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Http/IHttpClient.h"
#include "../Platform/Logger/Logger.h"
#include "../RestClients/IRestClientCache.h"
#include "../RestClients/RestServiceUrlClient.h"
#include "../RestClients/RestHttpClient.h"

#include "AuthenticationHandler.h"
#include "LicenseParser.h"
#include "LicenseParserResult.h"
#include "RestClientErrorHandling.h"
#include "UsageRestrictionsClient.h"

using namespace std;
using namespace rmscore::modernapi;
using namespace rmscore::platform::http;
using namespace rmscore::restclients;
using namespace rmscore::json;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace restclients {
std::shared_ptr<UsageRestrictionsResponse>UsageRestrictionsClient::
GetUsageRestrictions(const UsageRestrictionsRequest        & request,
                     modernapi::IAuthenticationCallbackImpl& authCallback,
                     modernapi::IConsentCallbackImpl       & consentCallback,
                     const string                          & email,
                     const bool                              bOffline,
                     std::shared_ptr<std::atomic<bool> >     cancelState,
                     const ResponseCacheFlags                cacheMask)
{
  std::shared_ptr<UsageRestrictionsResponse> response =
    make_shared<UsageRestrictionsResponse>();

  bool cryptData = (cacheMask& modernapi::RESPONSE_CACHE_CRYPTED) ==
                   modernapi::RESPONSE_CACHE_CRYPTED;
  bool useCache = (cacheMask& modernapi::RESPONSE_CACHE_ONDISK)  ==
                  modernapi::RESPONSE_CACHE_ONDISK;

  if (useCache && TryGetFromCache(request, email, response, cryptData))
  {
    return response;
  }

  // if we reached this point and offline flag is on, we have to throw
  if (bOffline) {
    throw exceptions::RMSNetworkException(
            "Couldn't get the response from cache, so need UI.",
            exceptions::RMSNetworkException::NeedsOnline);
  }
  auto pJsonSerializer   = json::IJsonSerializer::Create();
  auto serializedRequest = pJsonSerializer->SerializeUsageRestrictionsRequest(
    request);

  auto pRestServiceUrlClient = IRestServiceUrlClient::Create();
  auto licenseParserResult = LicenseParser::ParsePublishingLicense(request.pbPublishLicense,
    request.cbPublishLicense);


  auto endUserLicenseUrl = pRestServiceUrlClient->GetEndUserLicensesUrl(
    licenseParserResult,
    email,
    authCallback,
    consentCallback,
    cancelState);

  //"&claims=%7B%27access_token%27%3A%7B%27polids%27%3A%7B%27essential%27%3Atrue%2C%27values%27%3A%5B%2745a7a203-ff7f-497a-9f22-64d1c4a93b8e%27%5D%7D%7D%7D";
  auto httpRequestResult = RestHttpClient::Post(
    endUserLicenseUrl,
    move(serializedRequest),
    authCallback,
    cancelState);

  //if we receive 401 here, that probably means the CA flow should be triggered. In this case, we will keep taking the challenge from the 401
  //and re-raising the auth callback until the user cancels or produces a token of sufficient authority 
  while (httpRequestResult.status != StatusCode::UNAUTHORIZED)
  {
	  AuthenticationChallenge challenge = AuthenticationHandler::ParseChallengeHeader(httpRequestResult.header, endUserLicenseUrl);

	  auto token = authCallback.GetAccessToken(static_cast<const AuthenticationChallenge&>(challenge));

	  httpRequestResult = RestHttpClient::Post(
		  endUserLicenseUrl,
		  move(serializedRequest),
		  authCallback,
		  cancelState,
		  token);
  }

  if (StatusCode::OK != httpRequestResult.status)
  {
    HandleRestClientError(httpRequestResult.status,
                          httpRequestResult.responseBody);
  }

  try
  {
    *response = pJsonSerializer->DeserializeUsageRestrictionsResponse(
      httpRequestResult.responseBody);
  }
  catch (exceptions::RMSException)
  {
    throw exceptions::RMSNetworkException(
            "UsageRestrictionsClient: Got an invalid json from the REST service.",
            exceptions::RMSNetworkException::ServerError);
  }
  // we could deserialize the response so it should be valid and we can store it
  if (useCache) {
    StoreToCache(request,
                 email,
                 response,
                 httpRequestResult.responseBody,
                 cryptData);
  }

  return response;
}

int64_t daysTo(const std::chrono::time_point<std::chrono::system_clock>& l,
               const std::chrono::time_point<std::chrono::system_clock>& r) {
  return std::chrono::duration_cast<std::chrono::hours>(l - r).count() / 24;
}

void UsageRestrictionsClient::StoreToCache(
  const UsageRestrictionsRequest          & request,
  const std::string                       & email,
  std::shared_ptr<UsageRestrictionsResponse>response,
  const common::ByteArray                 & strResponse,
  bool                                      encryptData)
{
  if (0 != _stricmp("AccessGranted", response->accessStatus.c_str()))
  {
    // we don't cache no rights responses
    return;
  }

  if (!response->bAllowOfflineAccess) {
    // we don't cache not allowed for caching content
    return;
  }

  auto timeNow = std::chrono::system_clock::now();

  if ((std::chrono::system_clock::to_time_t(response->ftLicenseValidUntil) > 0) &&
      (daysTo(response->ftLicenseValidUntil, timeNow) <= 0))
  {
    // interval time is 0, i.e., LicenseValidUntil <= now, so we shouldn't cache
    // this response
    return;
  }

  shared_ptr<IRestClientCache> pCache = IRestClientCache::Create(
    encryptData ? IRestClientCache::CACHE_ENCRYPTED
    : IRestClientCache::CACHE_PLAINDATA);

  try
  {
    pCache->Store(s_usageRestrictionsCacheName, email,
                  request.pbPublishLicense, request.cbPublishLicense,
                  response->licenseValidUntil,
                  strResponse, true);
  }
  catch (exceptions::RMSException)
  {
    Logger::Hidden(
      "UsageRestrictionsClient::TryGetFromCache: Exception while parsing the cached response.");
  }
}

bool UsageRestrictionsClient::TryGetFromCache(
  const UsageRestrictionsRequest            & request,
  const std::string                         & email,
  std::shared_ptr<UsageRestrictionsResponse>& response,
  bool                                        decryptData)
{
  shared_ptr<IRestClientCache> pCache = IRestClientCache::Create(
    decryptData ? IRestClientCache::CACHE_ENCRYPTED
    : IRestClientCache::CACHE_PLAINDATA);
  shared_ptr<IJsonSerializer> pJsonSerializer = IJsonSerializer::Create();

  try
  {
    vector<string> vstrResponses = pCache->Lookup(
      s_usageRestrictionsCacheName,
      email,
      request.pbPublishLicense,
      request.cbPublishLicense,
      true);

    for (auto& iResponse : vstrResponses)
    {
      // try to deserialize the cached response
      try
      {
        common::ByteArray tmpArr(iResponse.begin(), iResponse.end());
        response = make_shared<UsageRestrictionsResponse>(
          pJsonSerializer->DeserializeUsageRestrictionsResponse(tmpArr));

        if (response->accessStatus.empty()) continue;

        // check if it's expired
        auto timeNow = std::chrono::system_clock::now();

        if ((std::chrono::system_clock::to_time_t(response->ftLicenseValidUntil) >
             0) && (timeNow > response->ftLicenseValidUntil))
        {
          // expired, so continue with the next response
          continue;
        }

        if ((std::chrono::system_clock::to_time_t(response->ftContentValidUntil) >
             0) && (timeNow > response->ftContentValidUntil))
        {
          // expired, so continue with the next response
          continue;
        }

        return true;
      }
      catch (exceptions::RMSException)
      {
        Logger::Hidden(
          "UsageRestrictionsClient::TryGetFromCache: Exception while parsing the cached response.");

        // exception is not fatal, we just continue with the next response
      }
    }

    return false;
  }
  catch (exceptions::RMSException)
  {
    Logger::Hidden(
      "UsageRestrictionsClient::TryGetFromCache: Exception while looking up the cache.");

    // exception is not fatal, we just return that we didn't find anything
    return false;
  }
}

const char UsageRestrictionsClient::s_usageRestrictionsCacheName[] = "UR";

shared_ptr<IUsageRestrictionsClient>IUsageRestrictionsClient::Create()
{
  return make_shared<UsageRestrictionsClient>();
}
} // namespace restclients
} // namespace rmscore
