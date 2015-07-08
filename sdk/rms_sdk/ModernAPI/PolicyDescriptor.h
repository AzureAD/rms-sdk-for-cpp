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
 * @brief Specifies users and rights assigned for a file.
 */
class DLL_PUBLIC_RMS PolicyDescriptor {
public:

  PolicyDescriptor(const std::vector<UserRights>& userRightsList);

  PolicyDescriptor(const std::vector<UserRoles>& userRolesList);

  const std::string& Name()
  {
    return this->name_;
  }

  void Name(const std::string& value)
  {
    this->name_ = value;
  }

  const std::string& Description()
  {
    return this->description_;
  }

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

  const std::vector<UserRoles>& UserRolesList()
  {
    return this->userRolesList_;
  }

  const std::chrono::time_point<std::chrono::system_clock>& ContentValidUntil()
  {
    return this->contentValidUntil_;
  }

  void ContentValidUntil(
    const std::chrono::time_point<std::chrono::system_clock>& value)
  {
    this->contentValidUntil_ = value;
  }

  bool AllowOfflineAccess()
  {
    return this->bAllowOfflineAccess_;
  }

  void AllowOfflineAccess(bool value)
  {
    this->bAllowOfflineAccess_ = value;
  }

  std::shared_ptr<std::string>Referrer() const
  {
    return this->referrer_;
  }

  void Referrer(std::shared_ptr<std::string>uri)
  {
    this->referrer_ = uri;
  }

  const AppDataHashMap& EncryptedAppData()
  {
    return this->encryptedAppData_;
  }

  void EncryptedAppData(const AppDataHashMap& value)
  {
    this->encryptedAppData_ = value;
  }

  const AppDataHashMap& SignedAppData()
  {
    return this->signedAppData_;
  }

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
 * @brief Constants for PolicyDescriptor.OfflineCacheLifetimeInDays property.
 */
enum OfflineCacheLifetimeConstants {
  NoCache           = 0,
  CacheNeverExpires = -1
};
} // m_namespace modernapi
} // m_namespace rmscore

#endif // _RMS_LIB_POLICYDESCRIPTOR_H_
