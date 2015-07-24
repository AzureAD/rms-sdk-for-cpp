/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef USERRIGHTS
#define USERRIGHTS
#include <vector>
#include <string>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
typedef std::vector<std::string> UserList;
typedef std::vector<std::string> RightList;

/*!
 * @brief Supplies properties for users and their granted rights.
 */
class DLL_PUBLIC_RMS UserRights {
public:

/*!
 * @brief Creates a UserRights object.
 */
  UserRights(const UserList& users, const RightList& rights) : users(users),
    rights(rights) {}

/*!
 * @brief The users to whom the rights are granted.
 */
  const UserList& Users() const
  {
    return this->users;
  }

/*!
 * @brief The granted rights.
 */
  const RightList& Rights() const
  {
    return this->rights;
  }

private:

  UserList  users;
  RightList rights;

private:

  void ValidateUsers(UserList& users);
  void ValidateRights(RightList& rights);
};
} // namespace modernapi
} // namespace rmscore


#endif // USERRIGHTS
