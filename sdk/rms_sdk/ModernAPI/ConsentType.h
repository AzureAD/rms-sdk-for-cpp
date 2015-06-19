/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef CONSENTTYPE
#define CONSENTTYPE
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
enum class ConsentType : char
{
  // consent type for document tracking
  DocumentTrackingConsent = 0,

  // consent type for contacting a service url
  ServiceUrlConsent = 1
};
} // namespace modernapi
} // namespace rmscore


#endif // CONSENTTYPE
