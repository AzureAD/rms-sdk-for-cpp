/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "../Core/ProtectionPolicy.h"
#include "../RestClients/TemplatesClient.h"
#include "../ModernAPI/RMSExceptions.h"
#include "TemplateDescriptor.h"
#include "AuthenticationCallbackImpl.h"

using namespace std;
using namespace rmscore::common;
using namespace rmscore::core;
using namespace rmscore::restclients;

namespace rmscore {
namespace modernapi {
TemplateDescriptor::TemplateDescriptor(const string& id,
                                       const string& name,
                                       const string& description)
  : m_id(id)
  , m_name(name)
  , m_description(description)
{}

TemplateDescriptor::TemplateDescriptor(shared_ptr<rmscore::core::
                                                  ProtectionPolicy>policy)
  : m_id(policy->GetId())
  , m_name(policy->GetName())
  , m_description(policy->GetDescription())
{
  if (policy->IsAdhoc()) {
    throw exceptions::RMSInvalidArgumentException("Invalid policy");
  }
}

shared_future<shared_ptr<vector<TemplateDescriptor> > >
TemplateDescriptor::GetTemplateListAsync(
  const string                     & userId,
  IAuthenticationCallback          & authenticationCallback,
  launch                             launchType,
  std::shared_ptr<std::atomic<bool> >cancelState)
{
  auto authenticationCallbackImpl = std::make_shared<AuthenticationCallbackImpl>(
    authenticationCallback, userId);

  return async(launchType,
               [authenticationCallbackImpl, cancelState](const string _userId)
               -> shared_ptr<vector<TemplateDescriptor> >
      {
        auto result = std::make_shared<vector<TemplateDescriptor> >();

        auto pTemplatesClient = ITemplatesClient::Create();
        auto response         = pTemplatesClient->GetTemplates(
          *authenticationCallbackImpl.get(), _userId, cancelState);

        for_each(
          begin(response.templates),
          end(response.templates),
          [&](const TemplateResponse& templateResponse)
        {
          result->push_back(move(TemplateDescriptor(templateResponse.id,
                                                    templateResponse.name,
                                                    templateResponse.description)));
        });

        return result;
      }, userId);
}
} // namespace modernapi
} // namespace rmscore
