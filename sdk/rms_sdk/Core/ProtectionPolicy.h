/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_PROTECTIONPOLICY_H_
#define _RMS_LIB_PROTECTIONPOLICY_H_

#include <chrono>
#include <CryptoAPI.h>
#include "../Common/CommonTypes.h"
#include "../Common/FrameworkSpecificTypes.h"
#include "../ModernAPI/IConsentCallbackImpl.h"
#include "../ModernAPI/IAuthenticationCallbackImpl.h"
#include "../ModernAPI/ProtectedFileStream.h"
#include "../ModernAPI/PolicyDescriptor.h"
#include "../RestClients/RestObjects.h"

namespace rmscore {
namespace core {
enum AccessStatus {
  ACCESS_STATUS_ACCESS_GRANTED,
  ACCESS_STATUS_ACCESS_DENIED,
  ACCESS_STATUS_ACCESS_EXPIRED,
};
struct UserRightsImpl {
  common::StringArray users;
  common::StringArray rights;
};
struct UserRolesImpl {
  common::StringArray users;
  common::StringArray roles;
};
struct PolicyDescriptorImpl {
  std::string                                       name;
  std::string                                       description;
  std::vector<UserRightsImpl>                       userRightsList;
  std::vector<UserRolesImpl>                        userRolesList;
  std::chrono::time_point<std::chrono::system_clock>ftContentValidUntil;
  bool                                              bAllowOfflineAccess;
  std::string                                       referrer;
  modernapi::AppDataHashMap                         signedApplicationData;
  modernapi::AppDataHashMap                         encryptedApplicationData;
};

class EmnptyConsentCallbackImpl : public modernapi::IConsentCallbackImpl {
public:

  virtual void Consents(const std::string& /*email*/,
                        const std::string& /*domain*/,
                        const
                        common::StringArray& /*urls*/) {}
};

class ProtectionPolicy {
public:

  AccessStatus GetAccessStatus() const {
    return m_accessStatus;
  }

  bool              AccessCheck(const std::string& right) const;

  const std::string GetId() const {
    return m_id;
  }

  const std::string GetName() const {
    return m_name;
  }

  const std::string GetDescription() const {
    return m_description;
  }

  const std::string GetOwner() const {
    return m_owner;
  }

  const std::string GetReferrer() const {
    return m_referrer;
  }

  const std::string GetIssuedTo() const {
    return m_issuedTo;
  }

  const std::string GetContentId() const {
    return m_contentId;
  }

  const std::string GetRequester() const {
    return m_requester;
  }

  void SetRequester(const std::string& requester) {
    m_requester = requester;
  }

  bool                                              IsIssuedToOwner() const;

  std::chrono::time_point<std::chrono::system_clock>GetValidityTimeFrom() const {
    return m_ftValidityTimeFrom;
  }

  std::chrono::time_point<std::chrono::system_clock>GetValidityTimeUntil() const {
    return m_ftValidityTimeUntil;
  }

  uint64_t GetValidityTimeDuration() const;

  bool     AllowOfflineAccess() const {
    return m_bAllowOfflineAccess;
  }

  bool IsAdhoc() const {
    return !m_bFromTemplate;
  }

  bool HasUserRightsInformation() const {
    return m_bHasUserRightsInformation;
  }

  const std::vector<UserRightsImpl>GetUserRightsList() const {
    return m_userRightsList;
  }

  const std::vector<UserRolesImpl>GetUserRolesList() const {
    return m_userRolesList;
  }

  const modernapi::AppDataHashMap GetSignedApplicationData() const {
    return m_signedApplicationData;
  }

  const modernapi::AppDataHashMap GetEncryptedApplicationData() const {
    return m_encryptedApplicationData;
  }

  rmscrypto::api::CipherMode GetCipherMode() const {
    return m_cipherMode;
  }

  std::shared_ptr<rmscrypto::api::ICryptoProvider>GetCryptoProvider() const {
    return m_pCryptoProvider;
  }

  const common::ByteArray GetPublishLicense() const {
    return m_publishLicense;
  }

public:

  static std::shared_ptr<ProtectionPolicy>Acquire(
    const uint8_t                          *pbPublishLicense,
    const size_t                            cbPublishLicense,
    modernapi::IAuthenticationCallbackImpl& authCallback,
    const std::string                     & email,
    const bool                              bOffline,
    std::shared_ptr<std::atomic<bool> >     cancelState,
    modernapi::ResponseCacheFlags           cacheMask);

  static std::shared_ptr<ProtectionPolicy>Acquire(
    const uint8_t                          *pbPublishLicense,
    const size_t                            cbPublishLicense,
    modernapi::IAuthenticationCallbackImpl& authCallback,
    modernapi::IConsentCallbackImpl       & consentCallback,
    const std::string                     & email,
    const bool                              bOffline,
    std::shared_ptr<std::atomic<bool> >     cancelState,
    modernapi::ResponseCacheFlags           cacheMask);

  static std::shared_ptr<ProtectionPolicy>Create(
    const bool                              bPreferDeprecatedAlgorithms,
    const bool                              bAllowAuditedExtraction,
    const std::string                     & templateId,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & email,
    const modernapi::AppDataHashMap       & signedAppData,
    std::shared_ptr<std::atomic<bool> >     cancelState);

  static std::shared_ptr<ProtectionPolicy>Create(
    const bool                              bPreferDeprecatedAlgorithms,
    const bool                              bAllowAuditedExtraction,
    PolicyDescriptorImpl                  & descriptor,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    const std::string                     & email,
    std::shared_ptr<std::atomic<bool> >     cancelState);

  static std::shared_ptr<ProtectionPolicy>GetCachedProtectionPolicy(
    const uint8_t    *pbPublishLicense,
    const size_t      cbPublishLicense,
    const std::string requester = std::string());

  ProtectionPolicy();

  void Initialize(
    const uint8_t                                         *pbPublishLicense,
    size_t                                                 cbPublishLicense,
    std::shared_ptr<restclients::UsageRestrictionsResponse>response);

  void Initialize(restclients::PublishResponse   & response,
                  bool                             bAllowAuditedExtraction,
                  bool                             bAllowOfflineAccess,
                  const modernapi::AppDataHashMap& signedData =
                    modernapi::AppDataHashMap(),
                  const modernapi::AppDataHashMap& encryptedData =
                    modernapi::AppDataHashMap());

  void        InitializeValidityTime(
    const std::chrono::time_point<std::chrono::system_clock>& ftContentValidUntil);
  void        InitializeIntervalTime(
    const std::chrono::time_point<std::chrono::system_clock>& ftLicenseValidUntil);

  void        InitializeKey(restclients::KeyDetailsResponse& response);

  static void AddProtectionPolicyToCache(
    std::shared_ptr<ProtectionPolicy>pProtectionPolicy);

private:

  // undefined copy constructor
  ProtectionPolicy(const ProtectionPolicy&);

  // undefined assignment operator
  ProtectionPolicy& operator=(const ProtectionPolicy&);

private:

  AccessStatus m_accessStatus;
  std::string  m_id;
  std::string  m_name;
  std::string  m_description;
  std::string  m_owner;
  std::string  m_referrer;
  std::string  m_issuedTo;
  std::string  m_contentId;
  std::string  m_requester;
  std::chrono::time_point<std::chrono::system_clock> m_ftValidityTimeFrom;
  std::chrono::time_point<std::chrono::system_clock> m_ftValidityTimeUntil;
  bool m_bAllowOfflineAccess;
  bool m_bFromTemplate;
  std::vector<std::string> m_rights;
  std::vector<std::string> m_roles;
  common::ByteArray m_publishLicense;
  rmscrypto::api::CipherMode m_cipherMode;
  std::shared_ptr<rmscrypto::api::ICryptoProvider> m_pCryptoProvider;
  bool m_bAllowAuditedExtraction;
  bool m_bHasUserRightsInformation;
  std::vector<UserRightsImpl> m_userRightsList;
  std::vector<UserRolesImpl>  m_userRolesList;
  modernapi::AppDataHashMap   m_signedApplicationData;
  modernapi::AppDataHashMap   m_encryptedApplicationData;

private:

  typedef std::list<std::shared_ptr<ProtectionPolicy> >CachedProtectionPolicies;

  static CachedProtectionPolicies *s_pCachedProtectionPolicies;
  static common::Mutex s_cachedProtectionPoliciesMutex;
};
} // namespace core
} // namespace rmscore
#endif // _RMS_LIB_PROTECTIONPOLICY_H_
