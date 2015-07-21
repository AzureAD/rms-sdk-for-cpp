/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_CONSENT_H_
#define _RMS_LIB_CONSENT_H_

#include <string>
#include <vector>
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/*
 * @brief Enumeration for types of consent.
 */
enum ConsentType {
	/*
	* @brief User consent for document tracking.
	*/
  DocumentTrackingConsent = 0, 
  
  /*
  * @brief User consent for contacting a service URL.
  */
  ServiceUrlConsent = 1
};

/*
* @brief Used for managing the user's consent information.
*
*        In operation, users can accept the consent or reject it.
*        Users can also choose to show the consent again or not.
*        You can save a user’s decision about whether to see the consent
*        prompt again by using the ShowAgain property.
*/
class DLL_PUBLIC_RMS ConsentResult {
public:

	/*
	* @brief Creates and initializes an instance of a ConsentResult object. 
	* @param accepted True or False; the user accepted
	* @param showAgain True or false; the consent prompt should be shown to the user again.
	* @userId The Id of the user; usually represented as an email address.
	*/
  ConsentResult(bool               accepted,
                bool               showAgain,
                const std::string& userId);

  /*
  * @brief Gets the Accepted flag indicating the user has consented or not.
  */
  bool Accepted() const {
    return this->accepted;
  }

  /*
  * @brief Gets the ShowAgain flag indicating whether the user consent prompt should be shown again or not.
  */
  bool ShowAgain() const {
    return this->showAgain;
  }

  /*
  * @brief Gets the email ID of the user giving consent.
  */
  const std::string& UserId() const {
    return this->userId;
  }

private:

  bool accepted;
  bool showAgain;
  const std::string userId;
};

// TODO: figure out why it's an interface
/*
* @brief Class for managing consent results.
*/
class DLL_PUBLIC_RMS IConsent {
public:

  /*
   * @brief Pointer to a ConsentResult object.
   */
  ConsentResult *Result;
  /*
  * @brief Type of consent.
  */
  ConsentType Type;

  /*
  * @brief List of urls for consent.
  */
  common::UrlArray Urls;

  /*
  * @brief The consenting user.
  */
  std::string User;

  /*
  * @brief Domain information.
  */
  std::string Domain;

  /*
  * @brief 
  */
  // BRP072315 - What is this for?
  IConsent() : Result(nullptr) {}
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CONSENT_H_
