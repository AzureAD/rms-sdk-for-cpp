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
/*!
 * @brief Structure for authentication information.
 */
struct AuthenticationChallenge
{
  /*!
   * @brief Authority with which the request is made.
   */
  std::string authority;

  /*!
  * @brief Resource being requested.
  */
  std::string resource;

  /*!
  * @brief Scope of the authentication request.
  */
  std::string scope;
};

/*!
* @brief Authentication callback implementation.
*/
class IAuthenticationCallbackImpl {
public:
	/*!
	* @brief Gets the state of the need for auth challenge.
	*/
  virtual bool        NeedsChallenge() const = 0;

  /*!
  * @brief Method for acquiring the access token.
  * @param challenge Pointer to auth information.
  * @return Returns the access token.
  */
  virtual std::string GetAccessToken(const AuthenticationChallenge& challenge) = 0;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_IAUTHENTICATIONCALLBACKIMPL_H_
