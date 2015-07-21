/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "../Platform/Http/IHttpClient.h"
#include "../Platform/Logger/Logger.h"
#include "../Json/IJsonSerializer.h"
#include "../ModernAPI/RMSExceptions.h"
#include "RestClientErrorHandling.h"
using namespace std;
using namespace rmscore::platform::http;
using namespace rmscore::json;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace restclients {
void HandleRestClientError(StatusCode         httpStatusCode,
                           common::ByteArray& sResponse)
{
  if (StatusCode::OK == httpStatusCode)
  {
    // no error
    return;
  }

  Logger::Hidden("Http request failed with response: %s", sResponse.data());

  switch (httpStatusCode)
  {
  case StatusCode::UNAUTHORIZED:
    throw exceptions::RMSRightsException(
            "The service didn't accept the auth token.");

  case StatusCode::NOT_FOUND:
    throw exceptions::RMSNetworkException("RMS service - resource not available.",
                                          exceptions::RMSNetworkException::ServiceNotAvailable);

  case StatusCode::BAD_GATEWAY:
    throw exceptions::RMSNetworkException("RMS service is unavailable.",
                                          exceptions::RMSNetworkException::ServiceNotAvailable);

  case StatusCode::BAD_REQUEST:
  case StatusCode::INTERNAL_SERVER_ERROR:
  {
    shared_ptr<IJsonSerializer> pJson = IJsonSerializer::Create();

    ServerErrorResponse response;
    try
    {
      response = pJson->DeserializeErrorResponse(sResponse);
    }
    catch (exceptions::RMSException)
    {
      throw exceptions::RMSNetworkException("RMS service error.",
                                            exceptions::RMSNetworkException::ServerError);
    }
    Logger::Hidden("Service error response: code='%s', message='%s'",
                   response.code.c_str(),
                   response.message.c_str());

    if ((0 ==
              _stricmp(
           "Microsoft.RightsManagement.Exceptions.UnrecognizedUrlException",
           response.code.c_str())) ||
        (0 ==
              _stricmp(
           "Microsoft.RightsManagement.Exceptions.UnknownTenantException",
           response.code.c_str())))
    {
      throw exceptions::RMSNetworkException(
              "Content protected by the on prem servers is not supported in this version.",
              exceptions::RMSNetworkException::OnPremNotSupported);
    }
    else if (0 ==
              _stricmp("Microsoft.RightsManagement.Exceptions.BadInputException",
                      response.code.c_str()))
    {
      throw exceptions::RMSNetworkException("Invalid PL",
                                            exceptions::RMSNetworkException::InvalidPL);
    }
    else if (0 ==
              _stricmp(
               "Microsoft.RightsManagement.Exceptions.ServiceDisabledException",
               response.code.c_str()))
    {
      throw exceptions::RMSNetworkException(
              "REST Service is disabled for publishing for this user.",
              exceptions::RMSNetworkException::ServiceDisabled);
    }
    else if ((0 ==
              _stricmp(
                "Microsoft.RightsManagement.Exceptions.DevicePlatformNotRecognizedException",
                response.code.c_str())) ||
             (0 ==
              _stricmp(
                "Microsoft.RightsManagement.Exceptions.DevicePlatformDisabledException",
                response.code.c_str())))
    {
      throw exceptions::RMSNetworkException(
              "REST Service is disabled for this device",
              exceptions::RMSNetworkException::DeviceRejected);
    }
    else if (0 ==
              _stricmp(
               "Microsoft.RightsManagement.Exceptions.ServiceDisabledForApplicationException",
               response.code.c_str()))
    {
      throw exceptions::RMSNetworkException(
              "REST Service is disabled for this platform",
              exceptions::RMSNetworkException::ServiceDisabled);
    }
    else if (0 ==
              _stricmp(
               "Microsoft.RightsManagement.Exceptions.NotSignedUpException",
               response.code.c_str()))
    {
      throw exceptions::RMSNetworkException(
              "REST Service is disabled for this tenant",
              exceptions::RMSNetworkException::ServiceDisabled);
    }

    // else go to the default case
  }

  default:
    throw exceptions::RMSNetworkException("RMS service error.",
                                          exceptions::RMSNetworkException::ServerError);
  }
}
} // namespace restclients
} // namespace rmscore
