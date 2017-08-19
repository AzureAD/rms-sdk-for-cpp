/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _IHTTPCLIENT_H_
#define _IHTTPCLIENT_H_

#include <memory>
#include <string>
#include <atomic>

#include "../../Common/FrameworkSpecificTypes.h"

namespace rmscore {
namespace platform {
namespace http {
enum class StatusCode {
  OK                    = 200,
  BAD_REQUEST           = 400,
  UNAUTHORIZED          = 401,
  NOT_FOUND             = 404,
  INTERNAL_SERVER_ERROR = 500,
  BAD_GATEWAY           = 502,
};

class IHttpClient {
public:

  virtual void       AddAuthorizationHeader(const std::string& authToken)   = 0;
  virtual void       AddAcceptMediaTypeHeader(const std::string& mediaType) = 0;
  virtual void       AddAcceptLanguageHeader(const std::string& language)   = 0;

  virtual void       AddHeader(const std::string& headerName,
                               const std::string& headerValue) = 0;

  virtual StatusCode Post(const std::string                & url,
                          const common::ByteArray          & request,
                          const std::string                & mediaType,
                          common::ByteArray                & response,
                          std::shared_ptr<std::atomic<bool> >cancelState) = 0;

  virtual StatusCode Get(const std::string                & url,
                         common::ByteArray                & response,
                         std::shared_ptr<std::atomic<bool> >cancelState) = 0;

  virtual const std::string GetResponseHeader(const std::string& headerName) = 0;

  virtual void              SetAllowUI(bool allow) = 0;
  virtual ~IHttpClient() {}

public:

  static std::shared_ptr<IHttpClient>Create();
};
}
}
} // namespace rmscore { namespace platform { namespace http {

#endif // _IHTTPCLIENT_H_
