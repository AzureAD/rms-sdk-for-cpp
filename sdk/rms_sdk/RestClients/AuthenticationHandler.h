/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_AUTHENTICATIONHANDLER_H_
#define _RMS_LIB_AUTHENTICATIONHANDLER_H_

#include "../ModernAPI/IAuthenticationCallbackImpl.h"
#include <string>

namespace rmscore {
namespace restclients {
class AuthenticationHandler {
public:

  static std::string GetAccessTokenForUrl(
    const std::string                       & sUrl,
    modernapi::IAuthenticationCallbackImpl  & callback);

private:

  static modernapi::AuthenticationChallenge GetChallengeForUrl(
    const std::string& sUrl);
  static modernapi::AuthenticationChallenge ParseChallengeHeader(
    const std::string& header,
    const std::string& url);
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_AUTHENTICATIONHANDLER_H_
