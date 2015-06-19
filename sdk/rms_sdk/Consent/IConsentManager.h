/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_ICONSENTMANAGER_H_
#define _RMS_LIB_ICONSENTMANAGER_H_

#include <string>
#include <vector>
#include <memory>
#include "../ModernAPI/IConsent.h"

namespace rmscore {
namespace consent {
class IConsentManager
{
public:
	virtual bool ShouldGetConsent() = 0;
    virtual void PersistConsentResult(const modernapi::ConsentResult& result) = 0;

public:
    static std::shared_ptr<IConsentManager> Create(std::shared_ptr<modernapi::IConsent> consent, bool bAutoApproved);
};
} // namespace consent
} // namespace rmscore
#endif // _RMS_LIB_ICONSENTMANAGER_H_
