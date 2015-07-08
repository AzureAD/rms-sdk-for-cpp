/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IAUTHENTICATIONCALLBACKIMPL_H_
#define _RMS_LIB_IAUTHENTICATIONCALLBACKIMPL_H_

#include <string>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
struct AuthenticationChallenge
{
  std::string authority;
  std::string resource;
  std::string scope;
};

class IAuthenticationCallbackImpl {
public:
  virtual bool        NeedsChallenge() const = 0;
  virtual std::string GetAccessToken(const AuthenticationChallenge& challenge) = 0;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_IAUTHENTICATIONCALLBACKIMPL_H_
