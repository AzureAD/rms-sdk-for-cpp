/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_ICONSENTCALLBACK_H_
#define _RMS_LIB_ICONSENTCALLBACK_H_

#include <memory>
#include "IConsent.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
using ConsentList = std::vector<std::shared_ptr<IConsent> >;

/*!
* @brief Callback to be provided by library user to notify app user of actions to
         be taken and request their consent.
*/
class IConsentCallback {
public:

/*!
* @brief Library user should implement this method to notify app user of actions
         to be taken and request their consent.
* @param consents List of Consent objects containing details about actions to
                  be taken.
* @return Same list of consents; method should set Result for each Consent to
          true or false.
*/
  virtual ConsentList Consents(ConsentList& consents) = 0;
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_ICONSENTCALLBACK_H_
