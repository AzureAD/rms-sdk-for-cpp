/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_USAGERESTRICTIONSCLIENT_H_
#define _RMS_LIB_USAGERESTRICTIONSCLIENT_H_

#include "IUsageRestrictionsClient.h"

namespace rmscore {
namespace restclients {
class UsageRestrictionsClient : public IUsageRestrictionsClient {
public:

  virtual std::shared_ptr<UsageRestrictionsResponse>GetUsageRestrictions(
    const UsageRestrictionsRequest        & request,
    modernapi::IAuthenticationCallbackImpl& authCallback,
    modernapi::IConsentCallbackImpl       & consentCallback,
    const std::string                     & email,
    const bool                              bOffline,
    std::shared_ptr<std::atomic<bool> >     cancelState,
    const modernapi::ResponseCacheFlags     cacheMask) override;

private:

  static bool TryGetFromCache(const UsageRestrictionsRequest            & request,
                              const std::string                         & email,
                              std::shared_ptr<UsageRestrictionsResponse>& response,
                              bool                                        decryptData);
  static void StoreToCache(const UsageRestrictionsRequest          & request,
                           const std::string                       & email,
                           std::shared_ptr<UsageRestrictionsResponse>response,
                           const common::ByteArray                 & strResponse,
                           bool                                      encryptData);

private:

  static const char s_usageRestrictionsCacheName[];
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_USAGERESTRICTIONSCLIENT_H_
