/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _HTTPCLIENTQT_H_
#define _HTTPCLIENTQT_H_

#include <atomic>

#include "IHttpClient.h"
#include <cpprest/http_client.h>
#include <pplx/pplxtasks.h>
#include <cpprest/http_msg.h>

namespace rmscore {
namespace platform {
namespace http {
class HttpClientQt : public IHttpClient {
  //     Q_OBJECT

public:

  HttpClientQt();
  ~HttpClientQt();

  virtual void       AddAuthorizationHeader(const std::string& authToken) override;
  virtual void       AddAcceptMediaTypeHeader(const std::string& mediaType) override;
  virtual void       AddAcceptLanguageHeader(const std::string& languages) override;
  virtual void       AddHeader(const std::string& headerName,
                               const std::string& headerValue) override;
  virtual StatusCode Post(const std::string                & url,
                          const common::ByteArray          & request,
                          const std::string                & mediaType,
                          common::ByteArray                & response,
                          std::shared_ptr<std::atomic<bool> >cancelState)
  override;

  virtual StatusCode Get(const std::string                & url,
                         common::ByteArray                & response,
                         std::shared_ptr<std::atomic<bool> >cancelState)
  override;

  virtual const std::string GetResponseHeader(const std::string& headerName) override;

  virtual void              SetAllowUI(bool allow) override;

private:
  pplx::task<void> HttpPostAsync(std::string uri, common::ByteArray& body,
                                 web::http::http_headers headers,common::ByteArray& response, http::StatusCode& code);
  pplx::task<void> HttpGetAsync(std::string uri, common::ByteArray& response,
                                web::http::http_headers headers, http::StatusCode& code);
  web::http::http_headers replyheaders_;
  web::http::http_headers requestheaders_;
};
}
}
} // namespace rmscore { namespace platform { namespace http {

#endif // _HTTPCLIENTQT_H_
