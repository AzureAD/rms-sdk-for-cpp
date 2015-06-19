/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IAUTHENTICATIONCALLBACK_H_
#define _RMS_LIB_IAUTHENTICATIONCALLBACK_H_

#include <memory>
#include <string>
#include "AuthenticationParameters.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/**
 * @brief IAuthenticationCallback interface to provide strategy for getting usable access token.
 *
 * Apps should implement this interface, specifically the GetToken method, to return an
 * access token for use by the API client.
 */
class IAuthenticationCallback {
public:

    /**
   * @brief Override to return an access token clients can attach to outbound API calls.
   * @param authenticationParamaters {@link AuthenticationParameters} Coordinates for OAuth calls.
   * @return A usable access token.
   */
  virtual std::string GetToken(
    std::shared_ptr<AuthenticationParameters>& authenticationParameters) = 0;
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_IAUTHENTICATIONCALLBACK_H_
