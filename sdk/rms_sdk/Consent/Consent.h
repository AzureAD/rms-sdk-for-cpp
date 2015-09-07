/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef CONSENT
#define CONSENT

#include <vector>
#include <string>

#include "../ModernAPI/IConsent.h"
#include "../Common/CommonTypes.h"

namespace rmscore {
namespace consent {
/// <summary>
/// Consent for accessing end user license url
/// </summary>
class Consent : public modernapi::IConsent {
public:

  virtual const  modernapi::ConsentResult& Result() const override
  {
    return this->result_;
  }

  virtual void Result(const modernapi::ConsentResult& value) {
    this->result_ = value;
  }

  //    virtual void ConsentResult(const modernapi::ConsentResult& value)
  // override
  //    {
  //        this->result_ = value;
  //    }

  virtual modernapi::ConsentType Type() const override
  {
    return this->type_;
  }

  virtual const std::vector<std::string>Urls() const override
  {
    return this->urls_;
  }

  virtual const std::string User() const override
  {
    return this->user_;
  }

  virtual const std::string Domain() const override
  {
    return this->domain_;
  }

protected:

  modernapi::ConsentResult result_;
  modernapi::ConsentType   type_;
  std::vector<std::string> urls_;
  std::string user_;
  std::string domain_;
};
} // namespace consent
} // namespace rmscore
#endif // CONSENT
