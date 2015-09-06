
/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_RESTHTTPCLIENT_H_
#define _RMS_LIB_RESTHTTPCLIENT_H_

#include <string>
#include "../Common/FrameworkSpecificTypes.h"
#include "../ModernAPI/IAuthenticationCallbackImpl.h"
#include "../Platform/Http/IHttpClient.h"

namespace rmscore {
namespace restclients {
class RestHttpClient {
public:

  struct Result
  {
    platform::http::StatusCode status;
    common::ByteArray          responseBody;
  };

  static Result Get(
    const std::string                     & sUrl,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState);

  static Result Post(
    const std::string                     & sUrl,
    common::ByteArray                    && requestBody,
    modernapi::IAuthenticationCallbackImpl& authenticationCallback,
    std::shared_ptr<std::atomic<bool> >     cancelState);

private:

  enum HttpRequestType
  {
    HTTP_GET,
    HTTP_POST,
  };

  struct HttpRequestParameters
  {
    HttpRequestType                    type;
    std::string                        requestUrl;
    common::ByteArray                  requestBody;
    std::string                        accessToken;
    std::shared_ptr<std::atomic<bool> >cancelState;
  };

  static Result      DoHttpRequest(const HttpRequestParameters& parameters);

  static std::string ConstructAuthTokenHeader(const std::string& accessToken);
  static std::string ConstructLanguageHeader();
  static std::string GenerateRequestId();
  static std::string GetPlatformIdHeader();

  static std::string platformIdHeaderCache;
};
} // namespace restclients
} // namespace rmscore
#endif // _RMS_LIB_RESTHTTPCLIENT_H_
