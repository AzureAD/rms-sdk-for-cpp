/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_IUSAGERESTRICTIONSCLIENT_H_
#define _RMS_LIB_IUSAGERESTRICTIONSCLIENT_H_
#include "../Common/FrameworkSpecificTypes.h"
#include "../ModernAPI/ProtectedFileStream.h"
#include "RestObjects.h"

namespace rmscore {
namespace modernapi {
class IAuthenticationCallbackImpl;
class IConsentCallbackImpl;
}
namespace restclients {
class IUsageRestrictionsClient {
public:

  virtual std::shared_ptr<UsageRestrictionsResponse>GetUsageRestrictions(
    const UsageRestrictionsRequest        & request,
    modernapi::IAuthenticationCallbackImpl& authCallback,
    modernapi::IConsentCallbackImpl       & consentCallback,
    const std::string                     & email,
    const bool                              bOffline,
    std::shared_ptr<std::atomic<bool> >     cancelState,
    const modernapi::ResponseCacheFlags     cacheMask) = 0;

public:

  static std::shared_ptr<IUsageRestrictionsClient>Create();
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_IUSAGERESTRICTIONSCLIENT_H_
