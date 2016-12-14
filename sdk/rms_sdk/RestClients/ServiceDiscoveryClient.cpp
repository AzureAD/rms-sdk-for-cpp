/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <locale>
#include <sstream>
#include <string>

#include "../Core/FeatureControl.h"
#include "../Json/IJsonSerializer.h"
#include "../Platform/Logger/Logger.h"
#include "../ModernAPI/RMSExceptions.h"

#include "RestHttpClient.h"
#include "RestServiceUrls.h"
#include "RestClientErrorHandling.h"
#include "ServiceDiscoveryClient.h"


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

ServiceDiscoveryListResponse ServiceDiscoveryClient::GetServiceDiscoveryDetails(const Domain & domain,
    const std::shared_ptr<std::string>& pServerPublicCertificate,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string& discoveryUrl,
    std::shared_ptr<std::atomic<bool>> cancelState)
{
    auto url = this->CreateGetRequest(discoveryUrl, domain);
    // Make sure stParams is filled, and get the original domain input used to generate the domain

    std::string publicCertificate(pServerPublicCertificate.get() == nullptr ? "" : *pServerPublicCertificate);
    std::string originalInput(domain.GetOriginalInput());

    auto authParams = AuthenticationHandler::AuthenticationHandlerParameters
    {
        publicCertificate,
        originalInput
    };

    auto result = RestHttpClient::Get(url, authParams, authenticationCallback, cancelState);

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

  // discovery service URL is received in several possible forms. due to dns entries being set differently.
  // For all of these the scheme was added in the previous step (http vs https check).
  // e.g. 1 - https://<domain>/my/v1/servicediscovery -> if is IsEvoEnabledChange it to v2.
  // e.g. 2 - https://<domain>/my/v2/servicediscovery -> if is IsEvoEnabledChange false it to v1.
  // e.g. 3 - https://<domain> ->
  // So fix it
  auto suffixV1 = RestServiceUrls::GetTenantV1() +
                  RestServiceUrls::GetServiceDiscoverySuffix();
  auto suffixV2 = RestServiceUrls::GetTenantV2() +
                  RestServiceUrls::GetServiceDiscoverySuffix();

  auto positionSuffixV1 = discoveryUrl.rfind(suffixV1);
  auto positionSuffixV2 = discoveryUrl.rfind(suffixV2);

  if (positionSuffixV1 != std::string::npos)
  {
    // e.g. 1 keep the end.
    if (rmscore::core::FeatureControl::IsEvoEnabled())
    {
        ss << discoveryUrl.substr(0, positionSuffixV1) << suffixV2;
    }
    else
    {
        ss << discoveryUrl;
    }
  }
  else if (positionSuffixV2 != std::string::npos)
  {
    // e.g. 2 keep the end.
    if (rmscore::core::FeatureControl::IsEvoEnabled())
    {
        ss << discoveryUrl;
    }
    else
    {
        ss << discoveryUrl.substr(0, positionSuffixV2) << suffixV1;
    }
  }
  else
  {
    // e.g. 3
    ss << discoveryUrl << RestServiceUrls::GetDefaultTenant() << RestServiceUrls::GetServiceDiscoverySuffix();
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
