/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <algorithm>
#include <vector>
#include "ServiceUrlConsentManager.h"
#include "ConsentDBHelper.h"
#include "../Common/CommonTypes.h"
#include "../ModernAPI/IConsent.h"

using namespace std;
using namespace rmscore::modernapi;

namespace rmscore {
namespace consent {
ServiceUrlConsentManager::ServiceUrlConsentManager(
  std::shared_ptr<modernapi::IConsent>consent, bool isAutoApproved)
  : m_consent(consent), m_approved(isAutoApproved)
{
  if (!m_consent->Urls().empty())
  {
    for (auto& url : m_consent->Urls())
    {
      if (_stricmp(m_cloudDomain.c_str(), url.c_str()) != 0)
      {
        this->m_urls.push_back(url);
      }
    }
  }
}

bool ServiceUrlConsentManager::ShouldGetConsent()
{
  if (m_urls.empty()) {
    return false;
  }

  if (ConsentDBHelper::GetInstance().Initialize())
  {
    return !(m_approved ||
             ConsentDBHelper::GetInstance().IsApprovedServiceDomainPresent(
               m_consent->User(), m_urls.at(0)));
  }

  return false;
}

void ServiceUrlConsentManager::PersistConsentResult(const ConsentResult& result)
{
  if (result.ShowAgain() || (m_urls.size() == 0)) return;

  if (ConsentDBHelper::GetInstance().Initialize())
  {
    ConsentDBHelper::GetInstance().AddApprovedServiceDomain(
      m_consent->User(), m_urls.at(0));
  }
}
} // namespace consent
} // namespace rmscore
