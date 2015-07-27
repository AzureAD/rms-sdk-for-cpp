/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "../ModernAPI/IConsent.h"
#include "../ModernAPI/ConsentType.h"
#include "IConsentManager.h"
#include "ServiceUrlConsentManager.h"
#include "DocumentTrackingConsentManager.h"

namespace rmscore { namespace consent {

std::shared_ptr<IConsentManager> IConsentManager::Create(std::shared_ptr<modernapi::IConsent> consent, bool bAutoApproved)
{
    if (consent->Type() == modernapi::ConsentType::ServiceUrlConsent)
    {
        return std::make_shared<ServiceUrlConsentManager>(consent, bAutoApproved);
    }
    else
    {
        return std::make_shared<DocumentTrackingConsentManager>(consent, bAutoApproved);
    }
}
} // namespace consent
} // namespace rmscore
