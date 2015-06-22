/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_USERPOLICY_H_
#define _RMS_LIB_USERPOLICY_H_

#include <stdint.h>
#include <unordered_map>
#include <chrono>

#include "IAuthenticationCallback.h"
#include "IConsentCallback.h"
#include "ModernAPIExport.h"
#include "CacheControl.h"
#include "PolicyDescriptor.h"

namespace rmscore {
namespace core {
class ProtectionPolicy;
}

namespace modernapi {
class TemplateDescriptor;
class PolicyDescriptor;

enum GetUserPolicyResultStatus {
  Success = 0, NoRights = 1, Expired = 2
};

class UserPolicy;

struct DLL_PUBLIC_RMS GetUserPolicyResult {
  GetUserPolicyResult(GetUserPolicyResultStatus   status,
                      std::shared_ptr<std::string>referrer,
                      std::shared_ptr<UserPolicy> policy);

  GetUserPolicyResultStatus   Status;
  std::shared_ptr<std::string>Referrer;
  std::shared_ptr<UserPolicy> Policy;
};
enum PolicyAcquisitionOptions {
  POL_None = 0x0, POL_OfflineOnly = 0x1
};
enum UserPolicyCreationOptions {
  USER_None                   = 0x0,
  USER_AllowAuditedExtraction = 0x1,     // specifies whether the
                                         // content can be opened in a non-RMS
                                         // aware app or not
  USER_PreferDeprecatedAlgorithms = 0x2, // specifies whether the
                                         // deprecated algorithms (ECB) is
                                         // preferred or not
};
enum UserPolicyType {
  TemplateBased = 0, Custom = 1,
};

class DLL_PUBLIC_RMS UserPolicy {
public:

  static std::shared_ptr<GetUserPolicyResult>Acquire(
    const std::vector<unsigned char>& serializedPolicy,
    const std::string               & userId,
    IAuthenticationCallback         & authenticationCallback,
    IConsentCallback                & consentCallback,
    PolicyAcquisitionOptions          options,
    ResponseCacheFlags                cacheMask);

  static std::shared_ptr<UserPolicy>CreateFromTemplateDescriptor(
    const TemplateDescriptor& templateDescriptor,
    const std::string       & userId,
    IAuthenticationCallback & authenticationCallback,
    UserPolicyCreationOptions options,
    const AppDataHashMap    & signedAppData);

  static std::shared_ptr<UserPolicy>Create(
    PolicyDescriptor        & policyDescriptor,
    const std::string       & userId,
    IAuthenticationCallback & authenticationCallback,
    UserPolicyCreationOptions options);

  bool                                              AccessCheck(std::string& right)
  const;
  UserPolicyType                                    Type();

  std::string                                       Name();
  std::string                                       Description();
  std::shared_ptr<modernapi::TemplateDescriptor>    TemplateDescriptor();
  std::shared_ptr<modernapi::PolicyDescriptor>      PolicyDescriptor();
  std::string                                       Owner();
  std::string                                       IssuedTo();

  bool                                              IsIssuedToOwner();

  std::string                                       ContentId();
  const AppDataHashMap                              EncryptedAppData();
  const AppDataHashMap                              SignedAppData();
  std::chrono::time_point<std::chrono::system_clock>ContentValidUntil();

  bool                                              DoesUseDeprecatedAlgorithms();
  bool                                              IsAuditedExtractAllowed();

  const std::vector<unsigned char>                  SerializedPolicy();
  std::shared_ptr<core::ProtectionPolicy>           GetImpl();

private:

  UserPolicy(std::shared_ptr<core::ProtectionPolicy>pImpl);

  std::shared_ptr<core::ProtectionPolicy> m_pImpl;
  std::shared_ptr<modernapi::TemplateDescriptor> m_templateDescriptor;
  std::shared_ptr<modernapi::PolicyDescriptor>   m_policyDescriptor;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_USERPOLICY_H_
