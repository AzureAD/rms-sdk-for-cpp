/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef ICONSENT
#define ICONSENT

#include <string>
#include <vector>
#include "ConsentType.h"
#include "ConsentResult.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
/**
 @brief Represents a user's consent/refusal to allow usage of a given URL.
*/
class IConsent {
public:
  /**
    @brief Result of a consent request.
  */
  virtual const ConsentResult& Result() const = 0;
  virtual void Result(const ConsentResult& value) = 0;

  /**
    @brief The type of URL to consented to.
  */
  virtual ConsentType Type() const = 0;

/**
  @brief List of URLs for which consent is granted/refused.
*/
  virtual const std::vector<std::string>Urls() const = 0;

/**
  @brief The user granting/refusing consent.
*/
  virtual const std::string User() const = 0;

/**
  @brief Just the domain.
*/
  virtual const std::string Domain() const = 0;
};
} // namespace modernapi
} // namespace rmscore

#endif // ICONSENT
