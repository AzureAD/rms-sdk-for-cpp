/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <memory>
#include <vector>
#include "IConsent.h"
#include "ConsentCallbackImpl.h"
#include "IConsentCallback.h"
#include "../Consent/DocumentTrackingConsent.h"
#include "../Consent/ServiceUrlConsent.h"
#include "../Consent/DocumentTrackingConsentManager.h"
#include "../Consent/ServiceUrlConsentManager.h"
#include "../ModernAPI/RMSExceptions.h"

using namespace std;
using namespace rmscore::consent;
using namespace rmscore::modernapi;

namespace rmscore {
namespace modernapi {
ConsentCallbackImpl::ConsentCallbackImpl(IConsentCallback  *callback,
                                         const std::string& userId,
                                         bool               isPublishing)
  : m_callback(callback)
  , m_userId(make_shared<string>(userId))
  , m_isPublishing(isPublishing)
{}

void ConsentCallbackImpl::Consents(const string             & email,
                                   const string             & domain,
                                   const std::vector<string>& urls)
{
  vector<IConsent> ret;

  // Create doctracking consents
  // Create service url consents
  // call the callback with all consents
  // Get the return value and create a vector with updated consents
  // return that vector

  // Create Serviceurl consent
  auto serviceConsent = std::make_shared<consent::ServiceUrlConsent>(email, urls);

  // Create doctracking consent
  auto docConsent = std::make_shared<consent::DocumentTrackingConsent>(email,
                                                                       domain);

  ConsentList consentCollection;


  auto serviceManager  = IConsentManager::Create(serviceConsent, m_isPublishing);
  auto documentManager = IConsentManager::Create(docConsent, m_isPublishing);

  // In case of Azure, there wont be any ServiceURL consent. We will add the
  // document tracking consent only.
  // In case of On-Prem, we will show serviceurl consent. This has information
  // about doc tracking as well.
  // So no need to add doc tracking if we have already added serviceurl consent.

  if (serviceManager->ShouldGetConsent())
  {
    consentCollection.push_back(serviceConsent);
  }

  if ((consentCollection.size() == 0) && documentManager->ShouldGetConsent())
  {
    consentCollection.push_back(docConsent);
  }

  // Nothing to consent - just return.
  if (consentCollection.size() == 0)
  {
    return;
  }

  // call user callback
  if (m_callback != nullptr) {
    auto results = m_callback->Consents(consentCollection);

    for (auto result : results)
    {
      //    if (!result->Result()) continue;

      // Persist the result.
      // If it is accepted - persist. If consent is not accepted - throw

      if (result->Result().Accepted() && !result->Result().ShowAgain())
      {
        // Consent manager to persist this result.
        if (result->Type() == ConsentType::ServiceUrlConsent)
        {
          serviceManager->PersistConsentResult(result->Result());
        }
        else
        {
          documentManager->PersistConsentResult(result->Result());
        }
      }

      // Handle the result
      if (!result->Result().Accepted())
      {
        throw exceptions::RMSNetworkException("User did not consent",
                                              exceptions::RMSNetworkException::UserNotConsented);
      }
    }
  }
}
} // namespace modernapi
} // namespace rmscore
