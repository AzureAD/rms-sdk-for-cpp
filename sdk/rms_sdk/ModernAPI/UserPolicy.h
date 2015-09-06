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
#include <chrono>

#include "IAuthenticationCallback.h"
#include "IConsentCallback.h"
#include "ModernAPIExport.h"
#include "CacheControl.h"
#include "TemplateDescriptor.h"
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

/*!
   @brief Specifies the expected mode for an operation. For example, can library
      use UI or expect available network.
 */
enum PolicyAcquisitionOptions {
  /*!
     @brief Allow UI and network operations.

     The framework will try to perform the operation silently and offline, but
        will show a UI and connect to a network if necessary.
   */
  POL_None = 0x0,

  /*!
     @brief Do not allow UI and network operations.

     The framework will try to perform the operation without connecting to a
        network. If it needs to connect to a network, the operation will fail.
        For example, an app can choose not to open a document on the device when
        it is not connected to a WiFi network unless it can be opened offline.
   */
  POL_OfflineOnly = 0x1
};

/*!
   @brief Flags related to policy.
 */
enum UserPolicyCreationOptions {
  USER_None = 0x0,

  /*!
     @brief Content can be opened in a non-RMS-aware app.
   */
  USER_AllowAuditedExtraction = 0x1,

  /*!
     @brief Deprecated cryptographic algorithms (ECB) are okay. For backwards
        compatibility.
   */
  USER_PreferDeprecatedAlgorithms = 0x2,
};

/*!
   @brief Source of policy.
 */
enum UserPolicyType {
  /*!
     @brief Policy was created from a template.
   */
  TemplateBased = 0,

  /*!
     @brief Policy was created adhoc.
   */
  Custom = 1,
};

/*!
   @brief
 */
class DLL_PUBLIC_RMS UserPolicy {
public:

  static std::shared_ptr<GetUserPolicyResult>Acquire(
    const std::vector<unsigned char> & serializedPolicy,
    const std::string                & userId,
    IAuthenticationCallback          & authenticationCallback,
    IConsentCallback                  *consentCallback,
    PolicyAcquisitionOptions           options,
    ResponseCacheFlags                 cacheMask,
    std::shared_ptr<std::atomic<bool> >cancelState);

  static std::shared_ptr<UserPolicy>CreateFromTemplateDescriptor(
    const TemplateDescriptor         & templateDescriptor,
    const std::string                & userId,
    IAuthenticationCallback          & authenticationCallback,
    UserPolicyCreationOptions          options,
    const AppDataHashMap             & signedAppData,
    std::shared_ptr<std::atomic<bool> >cancelState);

  static std::shared_ptr<UserPolicy>Create(
    PolicyDescriptor                 & policyDescriptor,
    const std::string                & userId,
    IAuthenticationCallback          & authenticationCallback,
    UserPolicyCreationOptions          options,
    std::shared_ptr<std::atomic<bool> >cancelState);

  bool                                              AccessCheck(
    const std::string& right) const;

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
