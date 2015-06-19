/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef SERVICEURLCONSENT
#define SERVICEURLCONSENT

#include "Consent.h"

namespace rmscore { namespace consent {

/// <summary>
/// Consent for accessing end user license url
/// </summary>
class ServiceUrlConsent : public Consent
{
public:
    ServiceUrlConsent(const std::string& email, const common::StringArray& urls);

};

} // namespace consent
} // namespace rmscore

#endif // SERVICEURLCONSENT

