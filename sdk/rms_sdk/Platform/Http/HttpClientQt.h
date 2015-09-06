/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _HTTPCLIENTQT_H_
#define _HTTPCLIENTQT_H_

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <exception>
#include <atomic>
#include "IHttpClient.h"

namespace rmscore {
namespace platform {
namespace http {
class HttpClientQt : public IHttpClient {
  //     Q_OBJECT

public:

  HttpClientQt();
  ~HttpClientQt();

  virtual void       AddAuthorizationHeader(const std::string& authToken) override;
  virtual void       AddAcceptMediaTypeHeader(const std::string& mediaType)
  override;
  virtual void       AddAcceptLanguageHeader(const std::string& languages)
  override;

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

  virtual const std::string GetResponseHeader(const std::string& headerName)
  override;

  virtual void              SetAllowUI(bool allow) override;

private:

  QNetworkAccessManager manager_;
  QNetworkRequest request_;
  QNetworkReply  *lastReply_;
};
}
}
} // namespace rmscore { namespace platform { namespace http {

#endif // _HTTPCLIENTQT_H_
