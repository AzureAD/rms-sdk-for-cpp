/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <algorithm>
#include <sstream>
#include "ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../RestClients/IUsageRestrictionsClient.h"
#include "../RestClients/IPublishClient.h"
#include "../Common/tools.h"

using namespace rmscore::modernapi;
using namespace rmscore::restclients;
using namespace rmscore::platform::logger;
using namespace std;

namespace rmscore {
namespace core {
static AccessStatus MapAccessStatus(const string& accessStatus) {
  if (0 == _stricmp("AccessGranted", accessStatus.c_str())) {
    return ACCESS_STATUS_ACCESS_GRANTED;
  } else if (0 == _stricmp("AccessDenied", accessStatus.c_str())) {
    return ACCESS_STATUS_ACCESS_DENIED;
  } else if (0 == _stricmp("ContentExpired", accessStatus.c_str())) {
    return ACCESS_STATUS_ACCESS_EXPIRED;
  } else {
    ostringstream str;

    str << "Got an invalid AccessStatus (" << accessStatus.c_str() <<
      ") from the server.";
    throw exceptions::RMSNetworkException(
            str.str(), exceptions::RMSNetworkException::ServerError);
  }
}

static rmscrypto::api::CipherMode MapCipherMode(const string& cipherMode) {
  if (0 ==
      _stricmp("MICROSOFT.CBC4K",
               cipherMode.c_str())) return rmscrypto::api::CIPHER_MODE_CBC4K;

  if (0 == _stricmp("MICROSOFT.CBC512.NOPADDING", cipherMode.c_str())) {
    return rmscrypto::api::CIPHER_MODE_CBC512NOPADDING;
  } else if (0 == _stricmp("MICROSOFT.ECB", cipherMode.c_str())) {
    return rmscrypto::api::CIPHER_MODE_ECB;
  } else {
    ostringstream str;

    str << "Got an invalid CipherMode (" << cipherMode.c_str() <<
      ") from the server.";
    throw exceptions::RMSNetworkException(
            str.str(), exceptions::RMSNetworkException::ServerError);
  }
}

shared_ptr<ProtectionPolicy>ProtectionPolicy::Acquire(
  const uint8_t                          *pbPublishLicense,
  const size_t                            cbPublishLicense,
  modernapi::IAuthenticationCallbackImpl& authCallback,
  const string                          & email,
  const bool                              bOffline,
  std::shared_ptr<std::atomic<bool> >     cancelState,
  modernapi::ResponseCacheFlags           cacheMask)
{
  EmnptyConsentCallbackImpl consentCallback;

  return ProtectionPolicy::Acquire(pbPublishLicense,
                                   cbPublishLicense,
                                   authCallback,
                                   consentCallback,
                                   email,
                                   bOffline,
                                   cancelState,
                                   cacheMask);
}

shared_ptr<ProtectionPolicy>ProtectionPolicy::Acquire(
  const uint8_t                          *pbPublishLicense,
  const size_t                            cbPublishLicense,
  modernapi::IAuthenticationCallbackImpl& authCallback,
  modernapi::IConsentCallbackImpl       & consentCallback,
  const string                          & email,
  const bool                              bOffline,
  std::shared_ptr<std::atomic<bool> >     cancelState,
  modernapi::ResponseCacheFlags           cacheMask)
{
  if (pbPublishLicense == nullptr) throw exceptions::RMSNullPointerException(
            "pbPublishLicense is null pointer");


  Logger::Hidden(" +ProtectionPolicy::Acquire");

  shared_ptr<ProtectionPolicy> pProtectionPolicy;
  try {
    pProtectionPolicy = GetCachedProtectionPolicy(pbPublishLicense,
                                                  cbPublishLicense,
                                                  email);
  } catch (exceptions::RMSException) {
    shared_ptr<IUsageRestrictionsClient> pClient =
      IUsageRestrictionsClient::Create();

    UsageRestrictionsRequest request =
    {
      pbPublishLicense, (uint32_t)cbPublishLicense
    };

    std::shared_ptr<UsageRestrictionsResponse> response =
      pClient->GetUsageRestrictions(request,
                                    authCallback,
                                    consentCallback,
                                    email,
                                    bOffline,
                                    cancelState,
                                    cacheMask);

    // log the response
    Logger::Hidden("ProtectionPolicy::Acquire got a usage restrictions response");
    Logger::Hidden("AccessStatus: %s",      response->accessStatus.c_str());
    Logger::Hidden("Id: %s",                response->id.c_str());
    Logger::Hidden("Name: %s",              response->name.c_str());
    Logger::Hidden("Referrer: %s",          response->referrer.c_str());
    Logger::Hidden("Owner: %s",             response->owner.c_str());
    Logger::Hidden("CipherMode: %s",        response->key.cipherMode.c_str());
    Logger::Hidden("AllowOfflineAccess: %s",
                   (response->bAllowOfflineAccess ? "true" : "false"));
    Logger::Hidden("licenseValidUntil: %s", response->licenseValidUntil.c_str());
    Logger::Hidden("contentId: %s",         response->contentId.c_str());
    Logger::Hidden("fromTemplate: %s",
                   response->bFromTemplate ? "TRUE" : "FALSE");

    // create and initialize a new protection policy object from the received
    // response
    pProtectionPolicy = shared_ptr<ProtectionPolicy>(new ProtectionPolicy());
    pProtectionPolicy->Initialize(pbPublishLicense, cbPublishLicense, response);
    pProtectionPolicy->SetRequester(email);

    // add the newly acquired protection policy to cache
    if (cacheMask & modernapi::RESPONSE_CACHE_INMEMORY) {
      AddProtectionPolicyToCache(pProtectionPolicy);
    }
  }
  Logger::Hidden(" -ProtectionPolicy::Acquire");

  return pProtectionPolicy;
} // ProtectionPolicy::Acquire

std::shared_ptr<ProtectionPolicy>ProtectionPolicy::Create(
  const bool                              bPreferDeprecatedAlgorithms,
  const bool                              bAllowAuditedExtraction,
  const string                          & templateId,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const string                          & email,
  const AppDataHashMap                  & signedAppData,
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
  Logger::Hidden(" +ProtectionPolicy::Create(using template)");

  auto pPublishClient = IPublishClient::Create();

  auto request = PublishUsingTemplateRequest {
    bPreferDeprecatedAlgorithms,
    bAllowAuditedExtraction,
    templateId,
    signedAppData
  };

  auto response = pPublishClient->PublishUsingTemplate(request,
                                                       authenticationCallback,
                                                       email,
                                                       cancelState);

  // log the response
  Logger::Hidden("ProtectionPolicy ::Create got a publish response");
  Logger::Hidden("Id: %s",         response.id.c_str());
  Logger::Hidden("Name: %s",       response.name.c_str());
  Logger::Hidden("Referrer: '%s'", response.referrer.c_str());
  Logger::Hidden("Owner: %s",      response.owner.c_str());
  Logger::Hidden("CipherMode: %s", response.key.cipherMode.c_str());
  Logger::Hidden("ContentId: %s",  response.contentId.c_str());

  // create and initialize a new protection policy object from the received
  // response

  auto pProtectionPolicy = make_shared<ProtectionPolicy>();

  pProtectionPolicy->Initialize(response, bAllowAuditedExtraction, true,
                                response.signedApplicationData);

  Logger::Hidden("-ProtectionPolicy::Create");
  return pProtectionPolicy;
} // ProtectionPolicy::Create

shared_ptr<ProtectionPolicy>ProtectionPolicy::Create(
  const bool                              bPreferDeprecatedAlgorithms,
  const bool                              bAllowAuditedExtraction,
  PolicyDescriptorImpl                  & descriptor,
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const string                          & email,
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
  if (descriptor.userRightsList.empty() && descriptor.userRolesList.empty()) {
    throw exceptions::RMSInvalidArgumentException(
            "Got an invalid response from the server : args are empty.");
  }
  Logger::Hidden(" +ProtectionPolicy::Create(custom)");

  auto pPublishClient = IPublishClient::Create();

  // create a request
  auto request = PublishCustomRequest(
    bPreferDeprecatedAlgorithms,
    bAllowAuditedExtraction
    );

  request.name        = descriptor.name;
  request.description = descriptor.description;

  common::Locale loc;

  request.language = loc.name().replace('_', "-").toStdString();

  request.encryptedApplicationData = descriptor.encryptedApplicationData;
  request.signedApplicationData    = descriptor.signedApplicationData;

  // initialize rights of the request
  // Either rights or roles will be present in the policy descriptor
  if (descriptor.userRightsList.size() != 0) {
    for_each(begin(descriptor.userRightsList), end(descriptor.userRightsList),
             [ =, &request](const UserRightsImpl& userRightsImpl) {
          if (userRightsImpl.users.empty() || userRightsImpl.rights.empty()) {
            throw exceptions::RMSInvalidArgumentException(
              "Got an invalid response from the server : args are empty.");
          }
          auto userRightsRequest = UserRightsRequest { userRightsImpl.users, userRightsImpl.rights };
          request.userRightsList.emplace_back(userRightsRequest);
        });
  } else {
    for_each(begin(descriptor.userRolesList), end(descriptor.userRolesList),
             [ =, &request](const UserRolesImpl& userRolesImpl) {
          if (userRolesImpl.users.empty() || userRolesImpl.roles.empty()) {
            throw exceptions::RMSInvalidArgumentException(
              "Got an invalid response from the server : args are empty.");
          }
          auto userRolesRequest = UserRolesRequest { userRolesImpl.users, userRolesImpl.roles };
          request.userRolesList.emplace_back(userRolesRequest);
        });
  }

  request.bAllowOfflineAccess = descriptor.bAllowOfflineAccess;
  request.ftLicenseValidUntil = descriptor.ftContentValidUntil;

  if (descriptor.referrer.size() >
      0) request.wsReferralInfo = descriptor.referrer;

  auto response = pPublishClient->PublishCustom(request,
                                                authenticationCallback,
                                                email,
                                                cancelState);

  // log the response
  Logger::Hidden(
    "ProtectionPolicy ::Create got a publish response with Id ='%s',Name = '%s'',Referrer = '%s'', Owner = '%s', CipherMode : '%s',ContentId: '%s'",
    response.id.c_str(),
    response.name.c_str(),
    response.referrer.c_str(),
    response.owner.c_str(),
    response.key.cipherMode.c_str(),
    response.contentId.c_str());

  auto pProtectionPolicy = shared_ptr<ProtectionPolicy>(new ProtectionPolicy());
  pProtectionPolicy->Initialize(response,
                                bAllowAuditedExtraction,
                                descriptor.bAllowOfflineAccess,
                                request.signedApplicationData,
                                request.encryptedApplicationData);
  pProtectionPolicy->SetRequester(email);

  Logger::Hidden("-ProtectionPolicy::Create");
  AddProtectionPolicyToCache(pProtectionPolicy);

  return pProtectionPolicy;
} // ProtectionPolicy::Create

ProtectionPolicy::ProtectionPolicy() :
  m_accessStatus(ACCESS_STATUS_ACCESS_DENIED),
  m_bAllowOfflineAccess(false), m_bFromTemplate(false)
{
  m_requester           = "";
  m_ftValidityTimeFrom  = std::chrono::system_clock::from_time_t(0);
  m_ftValidityTimeUntil = std::chrono::system_clock::from_time_t(0);
}

void ProtectionPolicy::Initialize(
  const uint8_t                            *pbPublishLicense,
  size_t                                    cbPublishLicense,
  std::shared_ptr<UsageRestrictionsResponse>response)
{
  // initializing protection policy from the consumption response
  m_accessStatus        = MapAccessStatus(response->accessStatus);
  m_id                  = response->id;
  m_name                = response->name;
  m_description         = response->description;
  m_owner               = response->owner;
  m_referrer            = response->referrer;
  m_rights              = response->rights;
  m_issuedTo            = response->issuedTo;
  m_contentId           = response->contentId;
  m_bAllowOfflineAccess = response->bAllowOfflineAccess;
  m_bFromTemplate       = response->bFromTemplate;

  InitializeValidityTime(response->ftContentValidUntil);
  InitializeIntervalTime(response->ftLicenseValidUntil);

  // Check whether roles are present
  if (response->roles.size() != 0) m_roles = response->roles;

  // custom policy for the owner
  if (!response->customPolicy.bIsNull) {
    for_each(begin(response->customPolicy.userRightsList), end(
               response->customPolicy.userRightsList),
             [this](UserRightsResponse& userRights) {
          UserRightsImpl userRightsImpl;
          userRightsImpl.users = userRights.users;
          userRightsImpl.rights = userRights.rights;
          m_userRightsList.emplace_back(userRightsImpl);
        });

    m_bHasUserRightsInformation = true;

    for_each(begin(response->customPolicy.userRolesList), end(
               response->customPolicy.userRolesList),
             [this](UserRolesResponse& userRoles) {
          UserRolesImpl userRolesImpl;
          userRolesImpl.users = userRoles.users;
          userRolesImpl.roles = userRoles.roles;
          m_userRolesList.emplace_back(userRolesImpl);
        });
  } else {
    m_userRightsList            = vector<UserRightsImpl>();
    m_userRolesList             = vector<UserRolesImpl>();
    m_bHasUserRightsInformation = false;
  }
  m_signedApplicationData    = response->signedApplicationData;
  m_encryptedApplicationData = response->encryptedApplicationData;

  // if access is granted verify the key and create a crypto provider
  if (ACCESS_STATUS_ACCESS_GRANTED ==
      m_accessStatus) InitializeKey(response->key);

  // initialize the publishing license
  m_publishLicense.resize(cbPublishLicense);

#ifdef Q_OS_WIN32
  memcpy_s(&m_publishLicense[0],
           m_publishLicense.size(), pbPublishLicense, cbPublishLicense);
#else // ifdef Q_OS_WIN32
  memcpy(&m_publishLicense[0], pbPublishLicense, cbPublishLicense);
#endif // ifdef Q_OS_WIN32
}     // ProtectionPolicy::Initialize

void ProtectionPolicy::Initialize(
  PublishResponse     & response,
  bool                  bAllowAuditedExtraction,
  bool                  bAllowOfflineAccess,
  const AppDataHashMap& signedData,
  const AppDataHashMap& encryptedData)
{
  // initializing protection policy from the publishing response (here the user
  // is the owner)

  m_accessStatus = ACCESS_STATUS_ACCESS_GRANTED;
  m_id           = response.id;
  m_name         = response.name;
  m_description  = response.description;
  m_owner        = response.owner;
  m_referrer     = response.referrer;
  m_rights       = vector<string>{
    string {
      "OWNER"
    }
  };
  m_issuedTo  = response.owner;
  m_contentId = response.contentId;

  m_ftValidityTimeFrom  = std::chrono::system_clock::from_time_t(0);
  m_ftValidityTimeUntil = std::chrono::system_clock::from_time_t(0);

  m_bAllowOfflineAccess = bAllowOfflineAccess;

  m_bAllowAuditedExtraction = bAllowAuditedExtraction;

  // if access is granted verify the key and create a crypto provider
  if (ACCESS_STATUS_ACCESS_GRANTED ==
      m_accessStatus) InitializeKey(response.key);

  // initialize the publishing license
  m_publishLicense = response.serializedLicense;

  // set the Application Data
  m_signedApplicationData    = signedData;
  m_encryptedApplicationData = encryptedData;
} // ProtectionPolicy::Initialize

void ProtectionPolicy::InitializeKey(restclients::KeyDetailsResponse& response) {
  if (response.value.empty()) throw exceptions::RMSInvalidArgumentException(
            "Got an invalid response from the server : access is granted but the key is empty.");



  try {
    std::vector<unsigned char> key(common::ConvertBase64ToBytes(response.value));
    m_cipherMode      = MapCipherMode(response.cipherMode);
    m_pCryptoProvider = rmscrypto::api::CreateCryptoProvider(m_cipherMode, key);
  } catch (exceptions::RMSException) {
    throw exceptions::RMSNetworkException(
            "Got an invalid base64 as a key value from the server.",
            exceptions::RMSNetworkException::ServerError);
  }
}

void ProtectionPolicy::InitializeValidityTime(
  const std::chrono::time_point<std::chrono::system_clock>& ftContentValidUntil)
{
  if (std::chrono::system_clock::to_time_t(ftContentValidUntil) > 0) {
    // The REST service doesn't return us the form time of the validity time
    // range, so setting it to now
    m_ftValidityTimeFrom  = std::chrono::system_clock::now();
    m_ftValidityTimeUntil = ftContentValidUntil;
  } else {
    m_ftValidityTimeFrom  = std::chrono::system_clock::from_time_t(0);
    m_ftValidityTimeUntil = std::chrono::system_clock::from_time_t(0);
  }
}

int64_t daysTo(const std::chrono::time_point<std::chrono::system_clock>& l,
               const std::chrono::time_point<std::chrono::system_clock>& r) {
  return std::chrono::duration_cast<std::chrono::hours>(l - r).count() / 24;
}

int64_t msecsTo(const std::chrono::time_point<std::chrono::system_clock>& l,
                const std::chrono::time_point<std::chrono::system_clock>& r) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(l - r).count();
}

void ProtectionPolicy::InitializeIntervalTime(
  const std::chrono::time_point<std::chrono::system_clock>& ftLicenseValidUntil) {
  if (std::chrono::system_clock::to_time_t(ftLicenseValidUntil) > 0) {
    // if the licenseValidUntil and contentValidUntil are the same then there is
    // no interval time set
    if (daysTo(m_ftValidityTimeUntil, ftLicenseValidUntil) != 0) {
      auto dt               = std::chrono::system_clock::now();
      int64_t iIntervalTime = daysTo(ftLicenseValidUntil, dt);

      if (iIntervalTime <= 0) m_bAllowOfflineAccess = false;
    }
  }
} // ProtectionPolicy::InitializeIntervalTime

bool ProtectionPolicy::AccessCheck(const string& right) const {
  auto i =
    find_if(begin(m_rights), end(m_rights),
            [right](const string& grantedRight) {
        return 0 ==
        _stricmp("OWNER", grantedRight.c_str()) || 0 == _stricmp(right.c_str(),
                                                                 grantedRight
                                                                 .c_str());
      });

  return end(m_rights) != i;
}

uint64_t ProtectionPolicy::GetValidityTimeDuration() const {
  return msecsTo(m_ftValidityTimeFrom, m_ftValidityTimeUntil);
}

bool ProtectionPolicy::IsIssuedToOwner() const {
  return 0 == _stricmp(m_owner.c_str(), m_issuedTo.c_str());
}

std::shared_ptr<ProtectionPolicy>ProtectionPolicy::GetCachedProtectionPolicy(
  const uint8_t *pbPublishLicense,
  const size_t   cbPublishLicense,
  const string   requester)
{
  common::MutexLocker lock(&s_cachedProtectionPoliciesMutex);

  if (pbPublishLicense == nullptr) {
    throw exceptions::RMSNullPointerException("NULL pointer exception");
  }


  if (s_pCachedProtectionPolicies == nullptr) {
    throw exceptions::RMSNotFoundException("No cached policy found");
  }

  // find in the list of cached protection policies
  auto i =
    find_if(
      s_pCachedProtectionPolicies->begin(), s_pCachedProtectionPolicies->end(),
      [pbPublishLicense, cbPublishLicense, requester](
        const shared_ptr<ProtectionPolicy>& pProtectionPolicy) {
        // return if the PL matches
        const std::vector<unsigned char>& pl = pProtectionPolicy->GetPublishLicense();

        bool cacheFound = (pl.size()) == cbPublishLicense &&
                          0 == memcmp(pbPublishLicense,
                                      &pl[0],
                                      pl.size());

        if (cacheFound && !requester.empty()) {
          string expectedRequester(requester);
          transform(expectedRequester.begin(), expectedRequester.end(),
                    expectedRequester.begin(), ::tolower);
          string cachedRequester(pProtectionPolicy->GetRequester());
          transform(cachedRequester.begin(),   cachedRequester.end(),
                    cachedRequester.begin(), ::tolower);

          cacheFound = (expectedRequester == cachedRequester);
        }
        return static_cast<uint32_t>(cacheFound);
      });

  if (i == s_pCachedProtectionPolicies->end()) {
    throw exceptions::RMSNotFoundException("No cached policy found");
  }

  shared_ptr<ProtectionPolicy> pCachedPolicy = *i;

  // push to front as the most recently used if it's not already
  if (s_pCachedProtectionPolicies->begin() != i) {
    s_pCachedProtectionPolicies->erase(i);
    s_pCachedProtectionPolicies->push_front(pCachedPolicy);
    i = s_pCachedProtectionPolicies->begin();
  }
  return *i;
} // ProtectionPolicy::GetCachedProtectionPolicy

void ProtectionPolicy::AddProtectionPolicyToCache(
  shared_ptr<ProtectionPolicy>pProtectionPolicy)
{
  common::MutexLocker lock(&s_cachedProtectionPoliciesMutex);

  if (nullptr ==
      s_pCachedProtectionPolicies) s_pCachedProtectionPolicies =
      new CachedProtectionPolicies();
  s_pCachedProtectionPolicies->push_front(pProtectionPolicy);

  const uint32_t dwMaxCacheEntries = 32;

  if (s_pCachedProtectionPolicies->size() > dwMaxCacheEntries) {
    // if we've reached the max cache size, ditch the least recently used
    // protection policy
    s_pCachedProtectionPolicies->pop_back();
  }
}

// NOTE: We don't delete the cache deliberately. We leak the cache on dll unload
// as it is not safe to call all the destructors on dll unload.
ProtectionPolicy::CachedProtectionPolicies *ProtectionPolicy::
s_pCachedProtectionPolicies = nullptr;
common::Mutex ProtectionPolicy::s_cachedProtectionPoliciesMutex;
} // namespace core
} // namespace rmscore
