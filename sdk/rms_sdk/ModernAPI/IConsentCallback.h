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
* @brief Interface for displaying consents. This callback is provided by the app
*        developer to know when and which consent notifications to display to user.
*/
class IConsentCallback {
public:

/*!
* @brief You, the app developer, should implement this method and return consents.
* @param consents List of Consents
* @return Access token
*/
  virtual ConsentList Consents(ConsentList& consents) = 0;
};
} // namespace modernapi
} // namespace rmscore

#endif // _RMS_LIB_ICONSENTCALLBACK_H_
