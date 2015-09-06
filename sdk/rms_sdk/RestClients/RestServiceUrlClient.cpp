/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "RestServiceUrlClient.h"
#include "RestServiceUrls.h"
#include "RestClientCache.h"
#include "DnsClientResult.h"
#include "IDnsLookupClient.h"
#include "LicenseParser.h"
#include "ServiceDiscoveryClient.h"
#include "../Platform/Settings/ILocalSettings.h"
#include "../Platform/Logger/Logger.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Common/tools.h"
using namespace std;
using namespace rmscore::modernapi;
using namespace rmscore::common;
using namespace rmscore::platform::settings;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace restclients {
const uint32_t DEFAULT_DNS_TTL = 30 * 24 * 60 * 60; // 30 days in seconds.

map<string, shared_ptr<ServiceDiscoveryDetails>,
    MyStringCompare> RestServiceUrlClient::serviceDiscoveryDetailsCache =
  map<string, shared_ptr<ServiceDiscoveryDetails>, MyStringCompare>();

RestServiceUrlClient::~RestServiceUrlClient()
{}

shared_ptr<ServiceDiscoveryDetails>RestServiceUrlClient::FindCache(
  const string& sEmail)
{
  if (sEmail.empty() ||
      RestClientCache::IsCacheLookupDisableTestHookOn()) return nullptr;

  auto it = serviceDiscoveryDetailsCache.find(string(sEmail));

  if (it == serviceDiscoveryDetailsCache.end()) return nullptr;

  return it->second;
}

void RestServiceUrlClient::GetConsent(
  IConsentCallbackImpl                  & consentCallback,
  const string                          & sEmail,
  std::shared_ptr<ServiceDiscoveryDetails>serviceDiscoveryDetails)
{
  GetConsent(consentCallback,
             sEmail,
             serviceDiscoveryDetails->Domain,
             serviceDiscoveryDetails->EndUserLicensesUrl);
}

void RestServiceUrlClient::GetConsent(IConsentCallbackImpl& consentCallback,
                                      const string        & sEmail,
                                      const string        & domain,
                                      const string        & url)
{
  vector<string> urls;
  urls.push_back(url);
  consentCallback.Consents(sEmail, domain, urls);
}

string RestServiceUrlClient::GetEndUserLicensesUrl(
  const void                        *pbPublishLicense,
  const size_t                       cbPublishLicense,
  const string                     & sEmail,
  IAuthenticationCallbackImpl      & authenticationCallback,
  IConsentCallbackImpl             & consentCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  shared_ptr<ServiceDiscoveryDetails> serviceDiscoveryDetails =
    FindCache(sEmail);

  if (serviceDiscoveryDetails == nullptr)
  {
    serviceDiscoveryDetails = GetServiceDiscoveryDetails(pbPublishLicense,
                                                         cbPublishLicense,
                                                         sEmail,
                                                         authenticationCallback,
                                                         &consentCallback,
                                                         cancelState);
  }
  else
  {
    GetConsent(consentCallback, sEmail, serviceDiscoveryDetails);
  }

  return string(serviceDiscoveryDetails->EndUserLicensesUrl);
}

string RestServiceUrlClient::GetTemplatesUrl(
  const string                     & sEmail,
  IAuthenticationCallbackImpl      & authenticationCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  shared_ptr<ServiceDiscoveryDetails> serviceDiscoveryDetails =
    FindCache(sEmail);

  if (serviceDiscoveryDetails == nullptr)
  {
    serviceDiscoveryDetails = GetServiceDiscoveryDetails(
      nullptr,
      0,
      sEmail,
      authenticationCallback,
      nullptr,
      cancelState);
    serviceDiscoveryDetailsCache[string(sEmail)] = serviceDiscoveryDetails;
  }
  return string(serviceDiscoveryDetails->TemplatesUrl);
}

string RestServiceUrlClient::GetPublishUrl(
  const string                     & sEmail,
  IAuthenticationCallbackImpl      & authenticationCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  shared_ptr<ServiceDiscoveryDetails> serviceDiscoveryDetails =
    FindCache(sEmail);

  if (serviceDiscoveryDetails == nullptr)
  {
    serviceDiscoveryDetails = GetServiceDiscoveryDetails(
      nullptr,
      0,
      sEmail,
      authenticationCallback,
      nullptr,
      cancelState);
    serviceDiscoveryDetailsCache[string(sEmail)] = serviceDiscoveryDetails;
  }
  return string(serviceDiscoveryDetails->PublishingLicensesUrl);
}

string RestServiceUrlClient::GetCloudDiagnosticsServerUrl(
  const string                     & sEmail,
  IAuthenticationCallbackImpl      & authenticationCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  shared_ptr<ServiceDiscoveryDetails> serviceDiscoveryDetails =
    FindCache(sEmail);

  if (serviceDiscoveryDetails == nullptr)
  {
    serviceDiscoveryDetails = GetServiceDiscoveryDetails(
      nullptr,
      0,
      sEmail,
      authenticationCallback,
      nullptr,
      cancelState);
    serviceDiscoveryDetailsCache[string(sEmail)] = serviceDiscoveryDetails;
  }
  return string(serviceDiscoveryDetails->CloudDiagnosticsServerUrl);
}

string RestServiceUrlClient::GetPerformanceServerUrl(
  const string                     & sEmail,
  IAuthenticationCallbackImpl      & authenticationCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  shared_ptr<ServiceDiscoveryDetails> serviceDiscoveryDetails =
    FindCache(sEmail);

  if (serviceDiscoveryDetails == nullptr)
  {
    serviceDiscoveryDetails = GetServiceDiscoveryDetails(
      nullptr,
      0,
      sEmail,
      authenticationCallback,
      nullptr,
      cancelState);
    serviceDiscoveryDetailsCache[string(sEmail)] = serviceDiscoveryDetails;
  }
  return string(serviceDiscoveryDetails->PerformanceServerUrl);
}

shared_ptr<ServiceDiscoveryDetails>RestServiceUrlClient::
GetServiceDiscoveryDetails(
  const void                        *pbPublishLicense,
  const size_t                       cbPublishLicense,
  const string                     & sEmail,
  IAuthenticationCallbackImpl      & authenticationCallback,
  IConsentCallbackImpl              *consentCallback,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  // If service discovery is not enabled, return default
  // In publish if empty string is passed as userid, return default
  auto settings = ILocalSettings::Create();

  bool serviceDiscoveryEnabled = settings->GetBool("MSIPCThin",
                                                   "ServiceDiscoveryEnabled",
                                                   true);

  if (!serviceDiscoveryEnabled ||
      ((pbPublishLicense == nullptr) && (sEmail.empty())))
  {
    auto serviceDiscoveryDetails = make_shared<ServiceDiscoveryDetails>();
    serviceDiscoveryDetails->EndUserLicensesUrl =
      RestServiceUrls::GetEndUserLicensesUrl();
    serviceDiscoveryDetails->PublishingLicensesUrl =
      RestServiceUrls::GetPublishUrl();
    serviceDiscoveryDetails->TemplatesUrl =
      RestServiceUrls::GetTemplatesUrl();
    serviceDiscoveryDetails->CloudDiagnosticsServerUrl =
      RestServiceUrls::GetCloudDiagnosticsServerUrl();
    serviceDiscoveryDetails->PerformanceServerUrl =
      RestServiceUrls::GetPerformanceServerUrl();

    if (consentCallback)
    {
      GetConsent(*consentCallback, sEmail, serviceDiscoveryDetails);
    }

    return serviceDiscoveryDetails;
  }

  // In consumption we use the domains from the publish license, else in publish
  // flows we use the domain from the email. Hence, they cannot both be null.
  if ((pbPublishLicense == nullptr) && (sEmail.empty())) {
    throw exceptions::RMSInvalidArgumentException("Invalid operation values");
  }

  // Figure out the domain(s).
  vector<shared_ptr<Domain> > domains;

  if (pbPublishLicense != nullptr)
  {
    Logger::Hidden("Using publishLicense to create domain");
    domains = LicenseParser::ExtractDomainsFromPublishingLicense(pbPublishLicense,
                                                                 cbPublishLicense);
  }
  else
  {
    Logger::Hidden("Using email to create domain");
    shared_ptr<Domain> domain = Domain::CreateFromEmail(sEmail);
    domains.push_back(domain);
  }

  auto pCache = IRestClientCache::Create(IRestClientCache::CACHE_PLAINDATA);

  for (auto& domain : domains)
  {
    auto domainStr               = domain->GetDomainStringForDnsLookup();
    auto serviceDiscoveryDetails = pCache->LookupServiceDiscoveryDetails(
      domainStr.c_str());

    if (serviceDiscoveryDetails.get() != nullptr)
    {
      Logger::Hidden("Service discovery is cached");

      if (consentCallback)
      {
        GetConsent(*consentCallback, sEmail, serviceDiscoveryDetails);
      }

      return serviceDiscoveryDetails;
    }
  }

  shared_ptr<Domain> selectedDomain;
  shared_ptr<DnsClientResult> dnsClientResult;

  for (auto& domain : domains)
  {
    vector<string> vDnsClientResult = pCache->LookupDnsClientResult(
      domain->GetDomainStringForDnsLookup().c_str());

    if (!vDnsClientResult.empty())
    {
      Logger::Hidden("Found cached DNS client result");
      selectedDomain  = domain;
      dnsClientResult = DnsClientResult::Create(vDnsClientResult[0]);
      break;
    }
  }

  if (dnsClientResult.get() == nullptr)
  {
    Logger::Hidden("No cached DNS client result");

    // selectedDomain must also be null if dnsClientResult is null
    if (selectedDomain.get() != nullptr)
    {
      Logger::Hidden("selectedDomain must be nul");
    }

    // DNS lookup to find discovery service URL.
    shared_ptr<IDnsLookupClient> dnsLookupClient = IDnsLookupClient::Create();

    for (auto& domain : domains)
    {
      // DNS lookup to find discovery service URL.
      dnsClientResult = dnsLookupClient->LookupDiscoveryService(domain);

      if (dnsClientResult.get() != nullptr)
      {
        Logger::Hidden("DNS lookup successful dns client result.");
        auto domainsChecked = dnsClientResult->GetDomainsChecked();
        for_each(
          begin(domainsChecked),
          end(domainsChecked),
          [ = ](const string& domainChecked)
            {
              pCache->StoreDnsClientResult(domainChecked.c_str(),
                                           GetTtlString(dnsClientResult->GetTtl())
                                           .c_str(),
                                           dnsClientResult->GetDiscoveryUrl());
            });

        selectedDomain = domain;
        break;
      }
    }

    if (dnsClientResult.get() == nullptr)
    {
      Logger::Hidden("Failed DNS lookup, reverting to cloud discovery service");

      if (domains.empty()) {
        throw exceptions::RMSInvalidArgumentException("DNS is empty");
      }

      // select first domain
      selectedDomain = domains[0];
      dnsClientResult.reset(new DnsClientResult(vector<string>()));
    }
  }

  if (selectedDomain.get() == nullptr)
  {
    throw exceptions::RMSInvalidArgumentException("Domain is empty");
  }

  Logger::Hidden("selectedDomain = %s",
                 selectedDomain->GetOriginalInput().c_str());

  // get the discovery url
  string discoveryUrl = dnsClientResult->GetDiscoveryUrl();
  Logger::Hidden("discoveryUrl = %s", discoveryUrl.c_str());

  if (consentCallback)
  {
    auto domains = selectedDomain->GetDomainStringForDnsLookup();
    GetConsent(*consentCallback,
               sEmail,
               domains,
               discoveryUrl);
  }

  // Query Discovery service for services details.
  shared_ptr<IServiceDiscoveryClient> serviceDiscoveryClient =
    IServiceDiscoveryClient::Create();
  auto serviceDiscoveryResponse =
    serviceDiscoveryClient->GetServiceDiscoveryDetails(
      *selectedDomain, authenticationCallback, discoveryUrl, cancelState);

  auto serviceDiscoveryDetails = make_shared<ServiceDiscoveryDetails>();
  serviceDiscoveryDetails->Ttl = static_cast<uint32_t>(-1); // TODO : Currently
                                                            // not implemented
                                                            // server side

  for (auto endpoint = serviceDiscoveryResponse.serviceEndpoints.begin();
       endpoint != serviceDiscoveryResponse.serviceEndpoints.end(); ++endpoint)
  {
    if (_strcmpi((*endpoint).name.c_str(), "enduserlicenses") == 0)
    {
      serviceDiscoveryDetails->EndUserLicensesUrl = (*endpoint).uri;
    }
    else if (_strcmpi((*endpoint).name.c_str(), "publishinglicenses") == 0)
    {
      serviceDiscoveryDetails->PublishingLicensesUrl = (*endpoint).uri;
    }
    else if (_strcmpi((*endpoint).name.c_str(), "templates") == 0)
    {
      serviceDiscoveryDetails->TemplatesUrl = (*endpoint).uri;
    }
    else if (_strcmpi((*endpoint).name.c_str(), "clientdebuglogs") == 0)
    {
      serviceDiscoveryDetails->CloudDiagnosticsServerUrl = (*endpoint).uri;
    }
    else if (_strcmpi((*endpoint).name.c_str(), "clientperformancelogs") == 0)
    {
      serviceDiscoveryDetails->PerformanceServerUrl = (*endpoint).uri;
    }
  }

  if (serviceDiscoveryDetails->EndUserLicensesUrl.empty()) {
    throw exceptions::RMSInvalidArgumentException("EndUserLicensesUrl is empty");
  }

  if (serviceDiscoveryDetails->PublishingLicensesUrl.empty()) {
    throw exceptions::RMSInvalidArgumentException("PublishingLicensesUrl is empty");
  }

  if (serviceDiscoveryDetails->TemplatesUrl.empty()) {
    throw exceptions::RMSInvalidArgumentException("TemplatesUrl is empty");
  }

  if (serviceDiscoveryDetails->CloudDiagnosticsServerUrl.empty()) {
    throw exceptions::RMSInvalidArgumentException(
            "CloudDiagnosticsServerUrl is empty");
  }

  if (serviceDiscoveryDetails->PerformanceServerUrl.empty()) {
    throw exceptions::RMSInvalidArgumentException("PerformanceServerUrl is empty");
  }

  Logger::Hidden(
    "serviceDiscoveryDetails: EndUserLicensesUrl = '%s', PublishingLicensesUrl = '%s'," \
    " TemplatesUrl = '%s', CloudDiagnosticsServerUrl = '%s', PerformanceServerUrl = '%s'",
    serviceDiscoveryDetails->EndUserLicensesUrl.c_str(),
    serviceDiscoveryDetails->PublishingLicensesUrl.c_str(),
    serviceDiscoveryDetails->TemplatesUrl.c_str(),
    serviceDiscoveryDetails->CloudDiagnosticsServerUrl.c_str(),
    serviceDiscoveryDetails->PerformanceServerUrl.c_str());

  // Store answer only if DNS record was found
  // don't store when dns record was not found as the result could be user's
  // mistyping of email address
  // or the DNS record actually didn't exist.
  if (dnsClientResult->DnsRecordFound())
  {
    pCache->Store(
      selectedDomain->GetDomainStringForDnsLookup(), serviceDiscoveryDetails,
      GetTtlString(serviceDiscoveryDetails->Ttl));
  }

  serviceDiscoveryDetails->Domain =
    selectedDomain->GetDomainStringForDnsLookup();
  return serviceDiscoveryDetails;
}

string RestServiceUrlClient::GetTtlString(uint32_t ttl)
{
  if (static_cast<uint32_t>(-1) == ttl)
  {
    ttl = DEFAULT_DNS_TTL;
  }

  common::DateTime dtn = common::DateTime::currentDateTime();

  return common::timeToString(dtn.addSecs(ttl));
}

shared_ptr<IRestServiceUrlClient>IRestServiceUrlClient::Create()
{
  return shared_ptr<IRestServiceUrlClient>(new RestServiceUrlClient());
}
} // namespace restclients
} // namespace rmscore
