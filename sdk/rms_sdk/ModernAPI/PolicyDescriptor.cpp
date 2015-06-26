/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <algorithm>

#include "../Core/ProtectionPolicy.h"
#include "../ModernAPI/RMSExceptions.h"
#include "PolicyDescriptor.h"

using namespace std;
using namespace rmscore::common;
using namespace rmscore::core;

namespace rmscore {
namespace modernapi {
template<class T>
static void ValidateInputRefObjectCollection(const TIterable<T>& collection)
{
  for (auto& item : collection) {
    if (item.empty()) {
      throw exceptions::RMSInvalidArgumentException("Invalid item");
    }
  }
}

void UserRights::ValidateUsers(UserList& users)
{
  ValidateInputRefObjectCollection<string>(users);
}

void UserRights::ValidateRights(RightList& rights)
{
  ValidateInputRefObjectCollection<string>(rights);
}

UserRoles::UserRoles(const UserList& users, const RoleList& roles)
  : users(users)
  , roles(roles)
{
  ValidateUsers(users);
  ValidateRoles(roles);
}

void UserRoles::ValidateUsers(const UserList& users)
{
  ValidateInputRefObjectCollection<string>(users);
}

void UserRoles::ValidateRoles(const RoleList& roles)
{
  ValidateInputRefObjectCollection<string>(roles);
}

PolicyDescriptor::PolicyDescriptor(const std::vector<UserRights>& userRightsList)
  : name_()
  , description_()
  , userRightsList_(userRightsList)
  , userRolesList_()
  , contentValidUntil_()
  , bAllowOfflineAccess_(true)
  , referrer_()
  , encryptedAppData_()
  , signedAppData_()
{
  ValidateUserRightsList(userRightsList);
}

PolicyDescriptor::PolicyDescriptor(const std::vector<UserRoles>& userRolesList)
  : name_()
  , description_()
  , userRightsList_()
  , userRolesList_(userRolesList)
  , contentValidUntil_()
  , bAllowOfflineAccess_(true)
  , referrer_()
  , encryptedAppData_()
  , signedAppData_()
{
  ValidateUserRolesList(userRolesList);
}

PolicyDescriptor::PolicyDescriptor(std::shared_ptr<ProtectionPolicy>policy)
  : name_()
  , description_()
  , userRightsList_()
  , contentValidUntil_()
  , bAllowOfflineAccess_(true)
  , referrer_()
  , encryptedAppData_()
  , signedAppData_()
{
  if (!policy->IsAdhoc()) {
    throw exceptions::RMSInvalidArgumentException("Invalid policy argument");
  }

  name_                = policy->GetName();
  description_         = policy->GetDescription();
  bAllowOfflineAccess_ = policy->AllowOfflineAccess();
  referrer_            = std::make_shared<std::string>(policy->GetReferrer());

  auto timeFrom = policy->GetValidityTimeFrom();
  auto dur      = policy->GetValidityTimeDuration();

  if ((std::chrono::system_clock::to_time_t(timeFrom) > 0) && (0ull != dur))
  {
    contentValidUntil_ = timeFrom + std::chrono::milliseconds(
      policy->GetValidityTimeDuration());
  }
  else
  {
    contentValidUntil_ = std::chrono::system_clock::from_time_t(0);
  }

  if (policy->HasUserRightsInformation())
  {
    vector<UserRights> userRightsList;

    for (auto userRightsImpl : policy->GetUserRightsList())
    {
      vector<string> users;

      for (auto userImpl : userRightsImpl.users)
      {
        users.push_back(userImpl);
      }

      vector<string> rights;

      for (auto rightImpl : userRightsImpl.rights)
      {
        rights.push_back(rightImpl);
      }

      userRightsList.push_back(UserRights(users, rights));
    }

    userRightsList_ = userRightsList;

    // In consumption flow, we might get roles as well.
    vector<UserRoles> userRolesList;

    for (auto userRolesImpl : policy->GetUserRolesList())
    {
      vector<string> users;

      for (auto userImpl : userRolesImpl.users)
      {
        users.push_back(userImpl);
      }

      vector<string> roles;

      for (auto& roleImpl : userRolesImpl.roles)
      {
        roles.push_back(roleImpl);
      }
      userRolesList.push_back(UserRoles(users, roles));
    }

    userRolesList_ = userRolesList;
  }
}

void PolicyDescriptor::ValidateUserRightsList(
  const std::vector<UserRights>& userRightsList)
{
  for (const auto& userRight : userRightsList) {
    ValidateInputRefObjectCollection<string>(userRight.Users());
    ValidateInputRefObjectCollection<string>(userRight.Rights());
  }
}

void PolicyDescriptor::ValidateUserRolesList(
  const std::vector<UserRoles>& userRolesList)
{
  for (const auto& userRole : userRolesList) {
    ValidateInputRefObjectCollection<string>(userRole.Users());
    ValidateInputRefObjectCollection<string>(userRole.Roles());
  }
}
} // namespace modernapi
} // namespace rmscore
