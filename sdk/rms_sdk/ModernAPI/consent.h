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
enum ConsentType {
  DocumentTrackingConsent = 0, ServiceUrlConsent = 1
};

class DLL_PUBLIC_RMS ConsentResult {
public:

  ConsentResult(bool               accepted,
                bool               showAgain,
                const std::string& userId);

  bool Accepted() const {
    return this->accepted;
  }

  bool ShowAgain() const {
    return this->showAgain;
  }

  const std::string& UserId() const {
    return this->userId;
  }

private:

  bool accepted;
  bool showAgain;
  const std::string userId;
};

// TODO: figure out why it's an interface
class DLL_PUBLIC_RMS IConsent {
public:

  ConsentResult *Result;
  ConsentType Type;
  common::UrlArray Urls;
  std::string User;
  std::string Domain;

  IConsent() : Result(nullptr) {}
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CONSENT_H_
