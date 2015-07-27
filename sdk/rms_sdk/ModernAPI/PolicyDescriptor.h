/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_POLICYDESCRIPTOR_H_
#define _RMS_LIB_POLICYDESCRIPTOR_H_

#include <chrono>
#include <unordered_map>
#include <memory>
#include <string>

#include "ModernAPIExport.h"

#include "UserRoles.h"
#include "UserRights.h"

namespace rmscore {
namespace core {
class ProtectionPolicy;
}

namespace modernapi {

/// @cond internal
namespace detail {
struct HashConstString
{
  long operator()(const std::string& str) const {
    return static_cast<long>(std::hash<std::string>()(str));
  }
};

template<typename T>
using HashMapString = std::unordered_map<std::string, T, HashConstString>;
}
/// @endcond

using AppDataHashMap = detail::HashMapString<std::string>;


/**
 * @brief Specifies users and rights assigned for a file. This is information used for custom protection.
 */
class DLL_PUBLIC_RMS PolicyDescriptor {
public:

 /**
  * @brief Constructor for PolicyDescriptor which initializes the object with a list of user rights.
  * @param userRightsList A collection of user rights.
  */
  PolicyDescriptor(const std::vector<UserRights>& userRightsList);

  /**
  * @brief Constructor for PolicyDescriptor which initializes the object with a list of user roles.
  * @param userRolesList A collection of user roles.
  */
  PolicyDescriptor(const std::vector<UserRoles>& userRolesList);

  /**
  * @brief Gets the name of the PolicyDescriptor.
  */
  const std::string& Name()
  {
    return this->name_;
  }

  /**
  * @brief Sets the name of the PolicyDescriptor.
  * @param value PolicyDescriptor name to set.
  */
  void Name(const std::string& value)
  {
    this->name_ = value;
  }

  /**
  * @brief Gets the description of the PolicyDescriptor.
  */
  const std::string& Description()
  {
    return this->description_;
  }

  /**
  * @brief Sets the description of the PolicyDescriptor.
  * @param value PolicyDescriptor description to set.
  */
  void Description(const std::string& value)
  {
    this->description_ = value;
  }

  /**
   * @brief Gets the user's rights list.
   *
   * The value of the UserRightsList property will be null if the current
   * user doesn't have access to the user rights information (i.e., is not the
   * owner and does not have the VIEWRIGHTSDATA right).
   *
   * @return List of users and their rights for this file.
   */
  const std::vector<UserRights>& UserRightsList() const
  {
    return this->userRightsList_;
  }

  /**
  * @brief Gets the user's roles list.
  */
  const std::vector<UserRoles>& UserRolesList()
  {
    return this->userRolesList_;
  }

  /**
  * @brief Gets the date that the content is valid until.
  */
  const std::chrono::time_point<std::chrono::system_clock>& ContentValidUntil()
  {
    return this->contentValidUntil_;
  }

  /**
  * @brief Sets the date that the content is valid until.
  * @param value Date the content is valid until.
  */
  void ContentValidUntil(
    const std::chrono::time_point<std::chrono::system_clock>& value)
  {
    this->contentValidUntil_ = value;
  }

  /**
  * @brief Gets the setting of the offline access allowance control; True or False.
  */
  bool AllowOfflineAccess()
  {
    return this->bAllowOfflineAccess_;
  }

  /**
  * @brief Sets the True or False setting of the offline access allowance control.
  * @param value True, offline access is allowed or False, it is not.
  */
  void AllowOfflineAccess(bool value)
  {
    this->bAllowOfflineAccess_ = value;
  }

  /**
  * @brief Gets the referral URI of the PolicyDescriptor.
  */
  std::shared_ptr<std::string>Referrer() const
  {
    return this->referrer_;
  }

  /**
  * @brief Sets the referral URI of the PolicyDescriptor.
  * @param uri The referral URI to set.
  */
  void Referrer(std::shared_ptr<std::string>uri)
  {
    this->referrer_ = uri;
  }

  /**
  * @brief Gets the encrypted app data.
  */
  const AppDataHashMap& EncryptedAppData()
  {
    return this->encryptedAppData_;
  }

  /**
  * @brief Sets the encrypted app data.
  * @param value The encrypted app data to set.
  */
  void EncryptedAppData(const AppDataHashMap& value)
  {
    this->encryptedAppData_ = value;
  }

  /**
  * @brief Gets the signed app data.
  */
  const AppDataHashMap& SignedAppData()
  {
    return this->signedAppData_;
  }

  /**
  * @brief Sets the signed app data.
  * @param value The encrypted app data to set.
  */
  void SignedAppData(const AppDataHashMap& value)
  {
    this->signedAppData_ = value;
  }

public:

  PolicyDescriptor(std::shared_ptr<core::ProtectionPolicy>policy);

private:

  std::string name_;
  std::string description_;

  std::vector<UserRights> userRightsList_;
  std::vector<UserRoles>  userRolesList_;

  std::chrono::time_point<std::chrono::system_clock> contentValidUntil_;
  bool bAllowOfflineAccess_;

  std::shared_ptr<std::string> referrer_;
  AppDataHashMap encryptedAppData_;
  AppDataHashMap signedAppData_;

private:

  static void ValidateUserRightsList(
    const std::vector<UserRights>& userRightsList);
  static void ValidateUserRolesList(
    const std::vector<UserRoles>& userRolesList);
};

/**
 * @brief Constants for PolicyDescriptor.AllowOfflineAccess property.
 */
enum OfflineCacheLifetimeConstants {

	/**
	* @brief Indicates whether the content should not be accessed offline.
	*/
  NoCache           = 0,

  /**
  * @brief The offline cache for the content shouldn't expire.
  */
  CacheNeverExpires = -1
};
} // m_namespace modernapi
} // m_namespace rmscore

#endif // _RMS_LIB_POLICYDESCRIPTOR_H_
