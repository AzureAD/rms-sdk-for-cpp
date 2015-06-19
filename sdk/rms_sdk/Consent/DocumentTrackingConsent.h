/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef DOCUMENTTRACKINGCONSENT
#define DOCUMENTTRACKINGCONSENT

#include "Consent.h"

namespace rmscore {
namespace consent {
/// <summary>
/// Consent for tracking document
/// </summary>
class DocumentTrackingConsent : public Consent
{
public:
    DocumentTrackingConsent(const std::string &email, const std::string& domain);
};

} // namespace consent
} // namespace rmscore
#endif // DOCUMENTTRACKINGCONSENT

