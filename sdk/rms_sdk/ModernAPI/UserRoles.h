/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef USERROLES
#define USERROLES
#include <vector>
#include <string>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
typedef std::vector<std::string>UserList;
typedef std::vector<std::string>RoleList;

/// <summary>
/// User rights
/// </summary>
class DLL_PUBLIC_RMS UserRoles {
public:

  /// <summary>
  /// .ctor
  /// </summary>
  UserRoles(const UserList& users,
            const RoleList& roles);

  /// <summary>
  /// The users to whom the rights are granted
  /// </summary>
  const UserList& Users() const
  {
    return this->users;
  }

  /// <summary>
  /// The roles of the users
  /// </summary>
  const RoleList& Roles() const
  {
    return this->roles;
  }

private:

  UserList users;
  RoleList roles;

private:

  void ValidateUsers(const UserList& users);
  void ValidateRoles(const RoleList& roles);
};
} // namespace modernapi
} // namespace rmscore


#endif // USERROLES
