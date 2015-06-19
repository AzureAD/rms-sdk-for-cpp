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
class DLL_PUBLIC_RMS AuthenticationCallbackImpl : public IAuthenticationCallbackImpl {
public:

  AuthenticationCallbackImpl(IAuthenticationCallback& callback,
                             const std::string      & userId)
    : m_callback(callback)
    , m_userId(userId)
  {}

  virtual bool NeedsChallenge() const override {
    return true;
  }

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
