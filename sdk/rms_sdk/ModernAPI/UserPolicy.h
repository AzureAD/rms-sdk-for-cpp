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

/*!
@brief
*/
enum GetUserPolicyResultStatus {
	/*!
	@brief
	*/
  Success = 0, 
  /*!
  @brief
  */
  NoRights = 1, 
  /*!
  @brief
  */
  Expired = 2
};


class UserPolicy;

/*!
@brief The result of the UserPolicy::Acquire operation
*/
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
   @brief Represents the policy associated with protected content.
 */
class DLL_PUBLIC_RMS UserPolicy {
public:

	/*!
	@brief Acquire a policy.

	@param serializedPolicy The PL to be consumed.
	@param userId The email address of the user that is trying to consume the protected content.
	This email address will be used to determine which policy object to use from the offline cache.
	If a policy object for this user is not found in the offline cache then this API will go online 
	(if allowed) to get the policy object.
	If null is passed, the first policy object found in the offline cache will be used. And again if 
	there is no policy object found for this content in the offline cache, this API will go online to 
	get the policy object.
	This parameter is also used as a hint for userId for user authentication, i.e., it will be passed 
	to the IAuthenticationCallback.GetTokenAsync() in the AuthenticationParameters structure.
	@param authenticationCallback Authentication callback to be used for authentication process.
	@param consentCallback Consent callback for user consent process.
	@param options Offline flag
	@param cacheMask BRP072215 What?
	@return Pointer a GetUserPolicyResult structure.

	*/
  static std::shared_ptr<GetUserPolicyResult>Acquire(
    const std::vector<unsigned char>& serializedPolicy,
    const std::string               & userId,
    IAuthenticationCallback         & authenticationCallback,
    IConsentCallback                & consentCallback,
    PolicyAcquisitionOptions          options,
    ResponseCacheFlags                cacheMask);

  /*!
  @brief For publishing using templates.

  @param templateDescriptor The template from which the policy is being created.
  @param userId The email address of the user that is protecting the content.
  This email address will be used to discover the RMS service instance, either 
  ADRMS server or Azure RMS, that the user's organization is using.
  This parameter is also used as a hint for userId for user authentication, i.e., 
  it will be passed to the IAuthenticationCallback.GetTokenAsync in the 
  AuthenticationParameters structure.
  @param authenticationCallback Authentication callback to be used for the authentication process.
  @param options Options for creating protection policy object. 
  For more information, see UserPolicyCreationOptions.
  @param signedAppData Singed App data to be stored with this template.
  @return Pointer to a UserPolicy object.
  */
  static std::shared_ptr<UserPolicy>CreateFromTemplateDescriptor(
    const TemplateDescriptor& templateDescriptor,
    const std::string       & userId,
    IAuthenticationCallback & authenticationCallback,
    UserPolicyCreationOptions options,
    const AppDataHashMap    & signedAppData);

  /*!
  @brief Create a custom protection policy.

  @param policyDescriptor A PolicyDescriptor object which defines the policy.
  @param userId The email address of the user that is protecting the content. 
  This email address will be used to discover the RMS service instance, either 
  ADRMS server or Azure RMS, that the user's organization is using. 
  This parameter is also used as a hint for userId for user authentication, i.e., 
  it will be passed to the IAuthenticationCallback.GetTokenAsync() in the 
  AuthenticationParameters structure.
  @param authenticationCallback Authentication callback to be used for authentication.
  @param options Options for creating protection policy object. 
  For more information, see UserPolicyCreationOptions.
  @return Pointer to a UserPolicy object.
  */
  static std::shared_ptr<UserPolicy>Create(
    PolicyDescriptor        & policyDescriptor,
    const std::string       & userId,
    IAuthenticationCallback & authenticationCallback,
    UserPolicyCreationOptions options);

  /*!
  @brief Returns a value that indicates whether the current user has the specified right.

  @param right The right to check.
  @return True if the current user has the right; otherwise false.
  Note - When called on behalf of the content owner, returns True for any right that is specified.
  */
  bool                                              AccessCheck(
    const std::string& right) const;
  
  /*!
  @brief Gets the policy type; template based or custom.
  */
  UserPolicyType                                    Type();

  /*!
  @brief Gets the name of the policy.
  */
  std::string                                       Name();

  /*!
  @brief Gets the policy description.
  */
  std::string                                       Description();

  /*!
  @brief Gets the template used to publish the content.
  */
  std::shared_ptr<modernapi::TemplateDescriptor>    TemplateDescriptor();

  /*!
  @brief Gets the policy descriptor.
  */
  std::shared_ptr<modernapi::PolicyDescriptor>      PolicyDescriptor();

  /*!
  @brief Gets the email address of the content owner.
  */
  std::string                                       Owner();

  /*!
  @brief Gets the user associated with the protection policy.
  */
  std::string                                       IssuedTo();

  /*!
  @brief Gets the flag indicating if the current user the is the content owner.
  */
  bool                                              IsIssuedToOwner();

  /*!
  @brief Gets the unique content ID associated with the protected document.
  */
  std::string                                       ContentId();

  /*!
  @brief Gets the encrypted app specific data.
  */
  const AppDataHashMap                              EncryptedAppData();

  /*!
  @brief Gets the signed app specific data.
  */
  const AppDataHashMap                              SignedAppData();

  /*!
  @brief The date the content is valid until.
  */
  std::chrono::time_point<std::chrono::system_clock>ContentValidUntil();

  /*!
  @brief
  @return Gets property for allowance of offline access.
  */
  bool                                              AllowOfflineAccess();

  /*!
  @brief Gets the deprecated algorithms flag.
  */
  bool                                              DoesUseDeprecatedAlgorithms();

  /*!
  @brief Gets property for allowance of audited extraction.
  */
  bool                                              IsAuditedExtractAllowed();

  /*!
  @brief Gets the serialized policy.
  */
  const std::vector<unsigned char>                  SerializedPolicy();

  /*!
  @brief Gets a pointer to ProtectionPolicy. BRP072215 - meaning?
  @return
  */
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
