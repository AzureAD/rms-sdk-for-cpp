/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_SERVICEURLCONSENTMANAGER_H_
#define _RMS_LIB_SERVICEURLCONSENTMANAGER_H_

#include <string>
#include <vector>
#include "IConsentManager.h"
#include "../ModernAPI/IConsent.h"
#include "../ModernAPI/ConsentResult.h"
#include "../ModernAPI/ConsentType.h"

namespace rmscore {
namespace consent {
class ServiceUrlConsentManager : public IConsentManager {
public:

  ServiceUrlConsentManager(std::shared_ptr<modernapi::IConsent>consent,
                           bool                                isAutoApproved);
  virtual bool ShouldGetConsent() override;
  virtual void PersistConsentResult(const modernapi::ConsentResult& result)
  override;

private:

  std::shared_ptr<modernapi::IConsent> m_consent;
  std::vector<std::string> m_urls;
  bool m_approved;
  const std::string m_cloudDomain = "api.aadrm.com";
};
} // namespace consent
} // namespace rmscore
#endif // _RMS_LIB_SERVICEURLCONSENTMANAGER_H_
