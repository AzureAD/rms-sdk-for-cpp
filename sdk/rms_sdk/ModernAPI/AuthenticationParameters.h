/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_AUTHENTICATIONCALLBACK_H_
#define _RMS_LIB_AUTHENTICATIONCALLBACK_H_

#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/**
 * @brief Coordinates for OAuth authentication with Azure AD or AD FS.
 */
class DLL_PUBLIC_RMS AuthenticationParameters {
public:

  std::string Authority()
  {
    return m_authority;
  }

  std::string Resource()
  {
    return m_resource;
  }

  std::string Scope()
  {
    return m_scope;
  }

  std::string UserId()
  {
    return m_userId;
  }

  AuthenticationParameters(const std::string& authority,
                           const std::string& resource,
                           const std::string& scope,
                           const std::string& userId)
    : m_authority(authority)
    , m_resource(resource)
    , m_scope(scope)
    , m_userId(userId)
  {}

private:

  std::string m_authority;
  std::string m_resource;
  std::string m_scope;
  std::string m_userId;
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_AUTHENTICATIONCALLBACK_H_
