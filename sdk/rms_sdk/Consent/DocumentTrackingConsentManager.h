/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_DOCUMENTTRACKINGCONSENTMANAGER_H_
#define _RMS_LIB_DOCUMENTTRACKINGCONSENTMANAGER_H_


#include "IConsentManager.h"

namespace rmscore { namespace consent {

class DocumentTrackingConsentManager : public IConsentManager
{
public:

    DocumentTrackingConsentManager(std::shared_ptr<modernapi::IConsent> consent, bool isAutoApproved);
    virtual bool ShouldGetConsent() override;
    virtual void PersistConsentResult(const modernapi::ConsentResult& result) override;

private:
    std::shared_ptr<modernapi::IConsent> m_consent;
    bool m_approved;
};

} // namespace consent
} // namespace rmscore

#endif // _RMS_LIB_DOCUMENTTRACKINGCONSENTMANAGER_H_
