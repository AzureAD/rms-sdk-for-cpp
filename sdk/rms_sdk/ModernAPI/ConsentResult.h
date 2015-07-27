/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CONSENTRESULT
#define CONSENTRESULT

#include <string>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/*
* @brief Management of user consent result.
*/
class ConsentResult {
public:
	/*
	* @brief Creates and initializes an instance of a ConsentResult object.
	*/
  ConsentResult(bool               accepted = false,
                bool               showAgain = true,
                const std::string& userId = "undef")
    : accepted(accepted)
    , showAgain(showAgain)
    , userId(userId)
  {}

  /*
  * @brief Gets the Accepted flag indicating the user has consented or not. 
  */
  bool Accepted() const
  {
    return this->accepted;
  }

  /*
  * @brief Gets the ShowAgain flag indicating whether the user consent prompt should be shown again or not.
  */
  bool ShowAgain() const
  {
    return this->showAgain;
  }

  /*
  * @brief Gets the email ID of the user giving consent.
  */
  const std::string& UserId() const
  {
    return this->userId;
  }

private:

  bool accepted;
  bool showAgain;
  std::string userId;
};
} // namespace modernapi
} // namespace rmscore


#endif // CONSENTRESULT
