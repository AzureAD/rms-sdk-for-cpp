/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_AUTHENTICATIONCALLBACKIMPL_H_
#define _RMS_LIB_AUTHENTICATIONCALLBACKIMPL_H_

#include "IAuthenticationCallbackImpl.h"
#include "IAuthenticationCallback.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/**
 * @brief Implementation class for authentication callback.
 */
class DLL_PUBLIC_RMS AuthenticationCallbackImpl : public IAuthenticationCallbackImpl {
public:

  /**
   * @brief This class wraps an IAuthenticationCallback implementation
   *        to provide OAuth coordinates.
   * @param callback Authentication callback implementation.
   * @param userId User ID of the authentication requestor, to be passed to
   *        the IAuthenticationCallback.
   */
  AuthenticationCallbackImpl(IAuthenticationCallback& callback,
                             const std::string      & userId)
    : m_callback(callback)
    , m_userId(userId)
  {}

  /**
  @brief False if OAuth coordinates (authority, resource, scope) are already
         known, True if these should be retrieved from challenge header in
         401 Unauthorized response from API.
  */
  virtual bool NeedsChallenge() const override {
    return true;
  }

  /**
  * @brief Prepares AuthenticationParameters from AuthenticationChallenge
  *        response, and passes these parameters to IAuthenticationCallback.
  * @param challenge Package of coordinates received from API 401 Unauthorized
  *        response.
  */
  virtual std::string GetAccessToken(const AuthenticationChallenge& challenge)
  override
  {
    auto parameters = std::make_shared<AuthenticationParameters>(
      challenge.authority,
      challenge.resource,
      challenge.scope,
      m_userId);

    return m_callback.GetToken(parameters);
  }

private:

  IAuthenticationCallback& m_callback;
  std::string m_userId;
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_AUTHENTICATIONCALLBACKIMPL_H_
