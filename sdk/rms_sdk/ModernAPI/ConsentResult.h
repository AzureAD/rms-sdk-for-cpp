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
class ConsentResult {
public:

  ConsentResult(bool               accepted = false,
                bool               showAgain = true,
                const std::string& userId = "undef")
    : accepted(accepted)
    , showAgain(showAgain)
    , userId(userId)
  {}

  bool Accepted() const
  {
    return this->accepted;
  }

  bool ShowAgain() const
  {
    return this->showAgain;
  }

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
