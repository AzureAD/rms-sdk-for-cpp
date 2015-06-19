/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "DocumentTrackingConsent.h"
#include "../ModernAPI/ConsentType.h"


namespace rmscore { namespace consent {

DocumentTrackingConsent::DocumentTrackingConsent(const std::string& email, const std::string& domain)
{
    this->user_     = email;
    this->domain_   = domain;
    this->type_     = modernapi::ConsentType::DocumentTrackingConsent;
}

} // namespace consent
} // namespace rmscore
