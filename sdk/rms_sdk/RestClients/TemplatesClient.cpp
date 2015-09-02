/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "TemplatesClient.h"
#include "RestClientErrorHandling.h"
#include "RestHttpClient.h"
#include "RestServiceUrls.h"
#include "RestServiceUrlClient.h"

#include "../ModernAPI/RMSExceptions.h"
#include "../Json/IJsonSerializer.h"
using namespace std;
using namespace rmscore::modernapi;
using namespace rmscore::json;
using namespace rmscore::platform::http;
namespace rmscore {
namespace restclients {
TemplateListResponse TemplatesClient::GetTemplates(
  modernapi::IAuthenticationCallbackImpl& authenticationCallback,
  const std::string                     & sEmail,
  std::shared_ptr<std::atomic<bool> >     cancelState)
{
  auto pRestServiceUrlClient = IRestServiceUrlClient::Create();
  auto templatesUrl          = pRestServiceUrlClient->GetTemplatesUrl(sEmail,
                                                                      authenticationCallback,
                                                                      cancelState);

  auto result =
    RestHttpClient::Get(templatesUrl, authenticationCallback, cancelState);

  if (StatusCode::OK != result.status)
  {
    HandleRestClientError(result.status, result.responseBody);
  }

  auto pJsonSerializer = IJsonSerializer::Create();

  try
  {
    return pJsonSerializer->DeserializeTemplateListResponse(result.responseBody);
  }
  catch (exceptions::RMSException)
  {
    throw exceptions::RMSNetworkException(
            "TemplatesClient: Got an invalid json from the REST service",
            exceptions::RMSNetworkException::ServerError);
  }
}

shared_ptr<ITemplatesClient>ITemplatesClient::Create()
{
  return make_shared<TemplatesClient>();
}
} // namespace restclients
} // namespace rmscore
