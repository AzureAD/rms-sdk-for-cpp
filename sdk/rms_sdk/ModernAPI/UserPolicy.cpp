/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <QDebug>
#include "../Common/tools.h"
#include "../Core/ProtectionPolicy.h"
#include "../ModernAPI/RMSExceptions.h"

#include "UserPolicy.h"
#include "TemplateDescriptor.h"
#include "PolicyDescriptor.h"
#include "AuthenticationCallbackImpl.h"
#include "rights.h"
#include "ConsentCallbackImpl.h"

using namespace rmscore::core;
using namespace rmscrypto::api;
using namespace rmscore::modernapi;
using namespace std;

namespace rmscore {
namespace modernapi {
GetUserPolicyResult::GetUserPolicyResult(GetUserPolicyResultStatus  status,
                                         std::shared_ptr<string>    referrer,
                                         std::shared_ptr<UserPolicy>policy) :
  Status(status),
  Referrer(referrer), Policy(policy) {}

UserPolicy::UserPolicy(shared_ptr<core::ProtectionPolicy>pImpl) : m_pImpl(pImpl) {
  if (pImpl->IsAdhoc()) m_policyDescriptor =
      make_shared<modernapi::PolicyDescriptor>(pImpl);
  else m_templateDescriptor = make_shared<modernapi::TemplateDescriptor>(pImpl);
}

shared_ptr<GetUserPolicyResult>UserPolicy::Acquire(
  const std::vector<unsigned char>& serializedPolicy,
  const string                    & userId,
  IAuthenticationCallback         & authenticationCallback,
  IConsentCallback                & consentCallback,
  PolicyAcquisitionOptions          options,
  ResponseCacheFlags                cacheMask)
{
  qDebug() << "+UserPolicy::AcquireAsync";

  AuthenticationCallbackImpl authenticationCallbackImpl(authenticationCallback,
                                                        userId);
  ConsentCallbackImpl consentCallbackImpl(consentCallback, userId, false);
  common::Event ev;

  shared_ptr<ProtectionPolicy> pImpl = ProtectionPolicy::Acquire(
    serializedPolicy.data(),
    serializedPolicy.size(),
    authenticationCallbackImpl,
    consentCallbackImpl,
    userId,
    (PolicyAcquisitionOptions::POL_OfflineOnly ==
     (options & PolicyAcquisitionOptions::POL_OfflineOnly)),
    ev, cacheMask);

  auto referrer = make_shared<std::string>(pImpl->GetReferrer());

  shared_ptr<GetUserPolicyResult> result;

  switch (pImpl->GetAccessStatus()) {
  case AccessStatus::ACCESS_STATUS_ACCESS_GRANTED:

    result = make_shared<GetUserPolicyResult>(
      GetUserPolicyResultStatus::Success,
      referrer,
      shared_ptr<UserPolicy>(new UserPolicy(pImpl)));
    break;

  case AccessStatus::ACCESS_STATUS_ACCESS_DENIED:

    result = make_shared<GetUserPolicyResult>(GetUserPolicyResultStatus::NoRights,
                                              referrer,
                                              nullptr);
    break;

  case AccessStatus::ACCESS_STATUS_ACCESS_EXPIRED:

    result = make_shared<GetUserPolicyResult>(GetUserPolicyResultStatus::Expired,
                                              referrer,
                                              nullptr);
    break;

  default:
    throw exceptions::RMSInvalidArgumentException("Invalid Access Status");
  } // switch

  qDebug() << "-UserPolicy::AcquireAsync";
  return result;
} // UserPolicy::Acquire

std::shared_ptr<UserPolicy>UserPolicy::CreateFromTemplateDescriptor(
  const modernapi::TemplateDescriptor& templateDescriptor,
  const string                       & userId,
  IAuthenticationCallback            & authenticationCallback,
  UserPolicyCreationOptions            options,
  const AppDataHashMap               & signedAppData)
{
  qDebug() << "+UserPolicy::CreateFromTemplateDescriptorAsync";

  AuthenticationCallbackImpl authenticationCallbackImpl(authenticationCallback,
                                                        userId);

  modernapi::AppDataHashMap signedApplicationData;

  if (!signedAppData.empty()) {
    for (auto& p : signedAppData) {
      auto name  = p.first;
      auto value = p.second;

      signedApplicationData[name] = value;
    }
  }
  auto templ = templateDescriptor.TemplateId();

  auto pImpl = ProtectionPolicy::Create(
    UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms == (options &
                                                                   UserPolicyCreationOptions
                                                                   ::
                                                                   USER_PreferDeprecatedAlgorithms),
    UserPolicyCreationOptions::USER_AllowAuditedExtraction == (options &
                                                               UserPolicyCreationOptions
                                                               ::
                                                               USER_AllowAuditedExtraction),
    templ,
    authenticationCallbackImpl,
    userId,
    signedApplicationData);
  auto result = std::shared_ptr<UserPolicy>(new UserPolicy(pImpl));

  qDebug() << "-UserPolicy::CreateFromTemplateDescriptorAsync";
  return result;
} // UserPolicy::CreateFromTemplateDescriptor

std::shared_ptr<UserPolicy>UserPolicy::Create(
  modernapi::PolicyDescriptor& policyDescriptor,
  const string               & userId,
  IAuthenticationCallback    & authenticationCallback,
  UserPolicyCreationOptions    options)
{
  qDebug() << "+UserPolicy::CreateAsync";

  auto authenticationCallbackImpl = AuthenticationCallbackImpl {
    authenticationCallback, userId
  };
  PolicyDescriptorImpl policyDescriptorImpl;

  if (!policyDescriptor.Name().empty()) policyDescriptorImpl.name =
      policyDescriptor.Name();

  if (!policyDescriptor.Description().empty()) policyDescriptorImpl.description =
      policyDescriptor.Description();
  policyDescriptorImpl.bAllowOfflineAccess =
    policyDescriptor.AllowOfflineAccess();
  policyDescriptorImpl.ftContentValidUntil = policyDescriptor.ContentValidUntil();

  auto ref = policyDescriptor.Referrer();

  if ((ref != nullptr) &&
      !ref->empty()) policyDescriptorImpl.referrer = *ref.get();

  // Either rights list or roles list need to be not nullptr

  auto userRightsList = policyDescriptor.UserRightsList();

  if (!userRightsList.empty()) {
    for_each(begin(userRightsList), end(userRightsList), [&policyDescriptorImpl](
               UserRights& userRights) {
          UserRightsImpl userRightsImpl;
          auto users = userRights.Users();

          transform(begin(users), end(users), back_inserter(userRightsImpl.users),
                    [](
                      string& user) {
            return user;
          });

          auto rights = userRights.Rights();

          transform(begin(rights), end(rights),
                    back_inserter(userRightsImpl.rights), [](
                      string& right) {
            return right;
          });

          policyDescriptorImpl.userRightsList.emplace_back(move(userRightsImpl));
        });
  } else { // if (policyDescriptor.UserRolesList != nullptr)
    auto roles = policyDescriptor.UserRolesList();

    for_each(begin(roles), end(roles),
             [&policyDescriptorImpl](UserRoles& userRoles) {
          UserRolesImpl userRolesImpl;
          auto users = userRoles.Users();

          transform(begin(users), end(users), back_inserter(userRolesImpl.users),
                    [](
                      string& user) {
            return user;
          });

          auto roles = userRoles.Roles();

          transform(begin(roles), end(roles), back_inserter(userRolesImpl.roles),
                    [](
                      string& role) {
            return role;
          });

          policyDescriptorImpl.userRolesList.emplace_back(move(userRolesImpl));
        });
  }

  if (!policyDescriptor.EncryptedAppData().empty()) {
    auto encryptedAppData = policyDescriptor.EncryptedAppData();

    for (auto& p : encryptedAppData) {
      auto name  = p.first;
      auto value = p.second;

      policyDescriptorImpl.encryptedApplicationData[name] =  value;
    }
  }

  if (!policyDescriptor.SignedAppData().empty()) {
    auto signedAppData = policyDescriptor.SignedAppData();

    for (auto& p : signedAppData) {
      auto name  = p.first;
      auto value = p.second;

      policyDescriptorImpl.signedApplicationData[name] = value;
    }
  }

  auto pImpl = ProtectionPolicy::Create(
    UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms == (options &
                                                                   UserPolicyCreationOptions
                                                                   ::
                                                                   USER_PreferDeprecatedAlgorithms),
    UserPolicyCreationOptions::USER_AllowAuditedExtraction == (options &
                                                               UserPolicyCreationOptions
                                                               ::
                                                               USER_AllowAuditedExtraction),
    policyDescriptorImpl,
    authenticationCallbackImpl,
    userId);
  auto result = std::shared_ptr<UserPolicy>(new UserPolicy(pImpl));

  if (result->m_policyDescriptor.get() != nullptr) {
    result->m_policyDescriptor->ContentValidUntil(
      policyDescriptor.ContentValidUntil());
  }

  qDebug() << "-UserPolicy::CreateAsync";
  return result;
} // UserPolicy::Create

bool UserPolicy::AccessCheck(const string& right) const {
  return m_pImpl->AccessCheck(right);
}

UserPolicyType UserPolicy::Type() {
  if (m_pImpl->IsAdhoc()) return UserPolicyType::Custom;
  else return UserPolicyType::TemplateBased;
}

string UserPolicy::Name() {
  return m_pImpl->GetName();
}

string UserPolicy::Description() {
  return m_pImpl->GetDescription();
}

shared_ptr<TemplateDescriptor>UserPolicy::TemplateDescriptor() {
  return m_templateDescriptor;
}

shared_ptr<PolicyDescriptor>UserPolicy::PolicyDescriptor() {
  return m_policyDescriptor;
}

string UserPolicy::Owner() {
  return m_pImpl->GetOwner();
}

string UserPolicy::IssuedTo() {
  return m_pImpl->GetIssuedTo();
}

bool UserPolicy::IsIssuedToOwner() {
  return m_pImpl->IsIssuedToOwner();
}

string UserPolicy::ContentId() {
  return m_pImpl->GetContentId();
}

const AppDataHashMap UserPolicy::EncryptedAppData() {
  auto enctryptedAppDataImpl = m_pImpl->GetEncryptedApplicationData();

  AppDataHashMap encryptedAppData;

  for_each(begin(enctryptedAppDataImpl), end(enctryptedAppDataImpl),
           [&](const pair<string,
                          string>
               & appDataEntry) {
        encryptedAppData[appDataEntry.first] = appDataEntry.second;
      });

  return encryptedAppData;
}

const AppDataHashMap UserPolicy::SignedAppData() {
  auto signedAppDataImpl = m_pImpl->GetSignedApplicationData();

  AppDataHashMap signedAppData;

  for_each(begin(signedAppDataImpl), end(signedAppDataImpl),
           [&](const pair<string,
                          string>
               & appDataEntry) {
        signedAppData[appDataEntry.first] = appDataEntry.second;
      });

  return signedAppData;
}

std::chrono::time_point<std::chrono::system_clock>UserPolicy::ContentValidUntil()
{
  return m_pImpl->GetValidityTimeUntil();
}

bool UserPolicy::AllowOfflineAccess() {
  return m_pImpl->AllowOfflineAccess();
}

bool UserPolicy::DoesUseDeprecatedAlgorithms() {
  return CipherMode::CIPHER_MODE_ECB == m_pImpl->GetCipherMode();
}

bool UserPolicy::IsAuditedExtractAllowed() {
  auto ae = CommonRights::AuditedExtract();

  return m_pImpl->AccessCheck(ae);
}

const std::vector<unsigned char>UserPolicy::SerializedPolicy() {
  const auto& pl = m_pImpl->GetPublishLicense();

  return pl;
}

shared_ptr<ProtectionPolicy>UserPolicy::GetImpl() {
  return m_pImpl;
}
} // namespace modernapi
} // namespace rmscore
