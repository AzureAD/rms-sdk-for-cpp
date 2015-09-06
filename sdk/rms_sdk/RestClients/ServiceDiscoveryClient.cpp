/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "ServiceDiscoveryClient.h"
#include "RestHttpClient.h"
#include "RestServiceUrls.h"
#include "RestClientErrorHandling.h"
#include "../Json/IJsonSerializer.h"
#include "../Platform/Logger/Logger.h"
#include <string>
#include <locale>
#include <sstream>
#include "../ModernAPI/RMSExceptions.h"

using namespace rmscore::platform;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace restclients {
static const std::string EMAIL_URL_PARAM  = "?email=";
static const std::string DOMAIN_URL_PARAM = "?service=";

std::shared_ptr<IServiceDiscoveryClient>IServiceDiscoveryClient::Create()
{
  return std::make_shared<ServiceDiscoveryClient>();
}

ServiceDiscoveryListResponse ServiceDiscoveryClient::GetServiceDiscoveryDetails(
  const Domain                          & domain,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                     & discoveryUrl,
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
  auto url = this->CreateGetRequest(discoveryUrl, domain);

  auto result = RestHttpClient::Get(
    url,
    authenticationCallback,
    cancelState);

  if (result.status != http::StatusCode::OK)
  {
    HandleRestClientError(result.status, result.responseBody);
  }

  auto pJsonSerializer = json::IJsonSerializer::Create();

  return pJsonSerializer->DeserializeServiceDiscoveryResponse(result.responseBody);
}

static std::locale s_loc;
static bool CharEqual(char first, char second)
{
  return std::tolower(first, s_loc) == std::tolower(second, s_loc);
}

std::string ServiceDiscoveryClient::CreateGetRequest(
  const std::string& discoveryUrl,
  const Domain     & domain)
{
  const std::string HTTPS_PROTOCOL = "https://";
  std::stringstream ss;

  // Received DiscoveryService that begins with HTTP(s)://.
  if (discoveryUrl.find("//") != std::string::npos)
  {
    bool beginsWithHttps = std::equal(HTTPS_PROTOCOL.begin(),
                                      HTTPS_PROTOCOL.end(),
                                      discoveryUrl.begin(), CharEqual);

    if (!beginsWithHttps)
    {
      Logger::Hidden("The following discovery service is unsecured: %s",
                     discoveryUrl.c_str());
      throw exceptions::RMSInvalidArgumentException("invalid servise name");
    }
  }
  else
  {
    ss << HTTPS_PROTOCOL;
  }

  ss << discoveryUrl;

  // discovery service URL is received in two forms
  // e.g. 1 - https://<domain>
  // e.g. 2 - https://<domain>/my/v1/servicediscovery
  // So fix it
  auto suffix = RestServiceUrls::GetDefaultTenant() +
                RestServiceUrls::GetServiceDiscoverySuffix();
  bool endsWithSuffix = equal(suffix.rbegin(), suffix.rend(),
                              discoveryUrl.rbegin(), CharEqual);

  if (!endsWithSuffix)
  {
    ss << suffix;
  }

  switch (domain.GetType())
  {
  case DomainType::Email:
    ss << EMAIL_URL_PARAM;
    break;

  case DomainType::Url:
    ss << DOMAIN_URL_PARAM;
    break;

  default:
    throw exceptions::RMSInvalidArgumentException("invalid domain type");
  }

  ss << domain.GetOriginalInput();

  return ss.str();
}
}
} // namespace rmscore { namespace restclients {
