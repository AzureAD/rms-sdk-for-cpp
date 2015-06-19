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
// TODO: figure out why it's an interface
class IConsent {
public:

  /// <summary>
  /// Gets or sets the consent result
  /// </summary>
  virtual const ConsentResult& Result() const = 0;

  // virtual void ConsentResult(const ConsentResult& value) = 0;

  /// <summary>
  /// Gets the type of consent
  /// </summary>
  virtual ConsentType Type() const = 0;

  /// <summary>
  /// Gets the list of urls that need to consented.
  /// </summary>
  virtual const std::vector<std::string>Urls() const = 0;

  /// <summary>
  /// Gets the user id
  /// </summary>
  virtual const std::string User() const = 0;

  /// <summary>
  /// Gets the domain information
  /// </summary>
  virtual const std::string Domain() const = 0;
};
} // namespace modernapi
} // namespace rmscore

#endif // ICONSENT
