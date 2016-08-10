/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifdef QTFRAMEWORK

#include "HttpClientQt.h"
#include <QThread>
#include <QMutex>
#include <QEventLoop>
#include <QTimer>

#include "../Logger/Logger.h"
#include "../../ModernAPI/RMSExceptions.h"
#include "mscertificates.h"
#include "HttpClientQt.h"

using namespace std;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace platform {
namespace http {
common::ByteArray ReadAllBytes(QIODevice *from) {
  common::ByteArray result;
  auto bytesAvailable = from->bytesAvailable();

  if (bytesAvailable > 0) {
    result.resize(static_cast<size_t>(bytesAvailable));
    char *buf = reinterpret_cast<char *>(&result[0]);
    size_t offset = 0;
    while (bytesAvailable > 0) {
      auto read = from->read(&buf[offset], bytesAvailable);

      if (read <= 0) break;
      bytesAvailable -= read;
      offset += read;
    }
  }

  return result;
}

shared_ptr<IHttpClient>IHttpClient::Create() {
  static bool initialized = false;

  // add Microsoft certificates to trust list
  if (!initialized) {
    QSslConfiguration SslConfiguration(QSslConfiguration::defaultConfiguration());

    QList<QSslCertificate> certificates = SslConfiguration.caCertificates();
    certificates.append(QSslCertificate::fromData(MicrosoftCertCA));
    certificates.append(QSslCertificate::fromData(MicrosoftCertSubCA));
    SslConfiguration.setCaCertificates(certificates);
    QSslConfiguration::setDefaultConfiguration(SslConfiguration);
    initialized = true;
  }
  return make_shared<HttpClientQt>();
}

HttpClientQt::HttpClientQt() : lastReply_(nullptr) {
  this->request_.setSslConfiguration(QSslConfiguration::defaultConfiguration());
}

HttpClientQt::~HttpClientQt() {}

void HttpClientQt::AddAuthorizationHeader(const string& authToken) {
  this->AddHeader("Authorization", authToken);
}

void HttpClientQt::AddAcceptMediaTypeHeader(const string& mediaType) {
  this->AddHeader("Accept", mediaType);
}

void HttpClientQt::AddAcceptLanguageHeader(const string& language) {
  this->AddHeader("Accept-Language", language);
}

void HttpClientQt::AddHeader(const string& headerName,
                             const string& headerValue) {
  this->request_.setRawHeader(headerName.c_str(), headerValue.c_str());
}

StatusCode HttpClientQt::Post(const string                     & url,
                              const common::ByteArray          & request,
                              const string                     & mediaType,
                              common::ByteArray                & response,
                              std::shared_ptr<std::atomic<bool> >cancelState)
{
  Logger::Info("==> HttpClientQt::POST %s", url.data());

  this->request_.setUrl(QUrl(url.c_str()));
  this->AddAcceptMediaTypeHeader(mediaType);

  Logger::Hidden("==> Request Headers:");
  foreach(const QByteArray &hdrName, this->request_.rawHeaderList()) {
    QByteArray hdrValue = this->request_.rawHeader(hdrName);
    Logger::Hidden("%s : %s", hdrName.data(), hdrValue.data());
  }

  std::string req(request.begin(), request.end());
  Logger::Hidden("==> Request Body: %s", req.c_str());

  // first abort previous

  lastReply_ = this->manager_.post(
    this->request_,
    QByteArray(reinterpret_cast<const char *>(request.data()),
               static_cast<int>(request.size())));

  QTimer timer;
  QEventLoop loop;
  QObject::connect(&timer,     SIGNAL(timeout()),  &loop, SLOT(quit()));
  QObject::connect(lastReply_, SIGNAL(finished()), &loop, SLOT(quit()));
  QObject::connect(lastReply_, &QNetworkReply::sslErrors,
                   [ = ](QList<QSslError>errorList) {
          for (auto& error : errorList) {
            Logger::Error("QSslError: %s",
                          error.errorString().toStdString().c_str());
            throw exceptions::RMSNetworkException(
              error.errorString().toStdString(),
              exceptions::RMSNetworkException::ServerError);
          }
        });

  do {
    timer.start(500);
    loop.exec();

    // check abandom
    if ((cancelState != nullptr) && cancelState->load()) {
      throw exceptions::RMSNetworkException(
              "Network operation was cancelled by user",
              exceptions::RMSNetworkException::CancelledByUser);
    }
  } while (!timer.isActive() || !lastReply_->isFinished());

  QVariant statusCode = lastReply_->attribute(
    QNetworkRequest::HttpStatusCodeAttribute);
  Logger::Info("Response StatusCode: %i", statusCode.toInt());

  Logger::Hidden("--> Response Headers:");
  foreach(const QNetworkReply::RawHeaderPair & pair,
          lastReply_->rawHeaderPairs()) {
    Logger::Hidden("%s : %s", pair.first.data(), pair.second.data());
  }

  response = ReadAllBytes(lastReply_);
  Logger::Hidden("--> Response Body:");
  Logger::Hidden(string(response.begin(), response.end()));

  QNetworkReply::NetworkError error_type = lastReply_->error();

  if (error_type != QNetworkReply::NoError) {
    Logger::Error(QString("error: %1").arg(
                    lastReply_->errorString()).toStdString());
  }

  return StatusCode(statusCode.toInt());
}

StatusCode HttpClientQt::Get(const string                     & url,
                             common::ByteArray                & response,
                             std::shared_ptr<std::atomic<bool> >cancelState)
{
  Logger::Info("==> HttpClientQt::GET %s", url.data());

  this->request_.setUrl(QUrl(url.c_str()));

  Logger::Hidden("==> Request headers:");
  foreach(const QByteArray &hdrName, this->request_.rawHeaderList()) {
    QByteArray hdrValue = this->request_.rawHeader(hdrName);
    Logger::Hidden("%s : %s", hdrName.data(), hdrValue.data());
  }

  lastReply_ = this->manager_.get(this->request_);
  QTimer timer;
  QEventLoop loop;
  QObject::connect(&timer,     SIGNAL(timeout()),  &loop, SLOT(quit()));
  QObject::connect(lastReply_, SIGNAL(finished()), &loop, SLOT(quit()));
  QObject::connect(lastReply_, &QNetworkReply::sslErrors,
                   [ = ](QList<QSslError>errorList) {
          for (auto& error : errorList) {
            Logger::Error("QSslError: %s",
                          error.errorString().toStdString().c_str());
            throw exceptions::RMSNetworkException(
              error.errorString().toStdString(),
              exceptions::RMSNetworkException::ServerError);
          }
        });

  do {
    timer.start(500);
    loop.exec();

    // check abandom
    if ((cancelState != nullptr) && cancelState->load()) {
      throw exceptions::RMSNetworkException(
              "Network operation was cancelled by user",
              exceptions::RMSNetworkException::CancelledByUser);
    }
  } while (!timer.isActive() || !lastReply_->isFinished());

  QVariant statusCode = lastReply_->attribute(
    QNetworkRequest::HttpStatusCodeAttribute);
  Logger::Info("Response StatusCode: %i", statusCode.toInt());

  Logger::Hidden("--> Response Headers:");
  foreach(const QNetworkReply::RawHeaderPair & pair,
          lastReply_->rawHeaderPairs()) {
    Logger::Hidden("%s : %s", pair.first.data(), pair.second.data());
  }

  response = ReadAllBytes(lastReply_);
  Logger::Hidden("--> Response Body:");
  Logger::Hidden(string(response.begin(), response.end()));

  QNetworkReply::NetworkError error_type = lastReply_->error();

  if (error_type != QNetworkReply::NoError) {
    Logger::Error(QString("error: %1").arg(
                    lastReply_->errorString()).toStdString());
  }

  return StatusCode(statusCode.toInt());
}

const string HttpClientQt::GetResponseHeader(const string& headerName) {
  return string(lastReply_->rawHeader(headerName.c_str()).data());
}

void HttpClientQt::SetAllowUI(bool /* allow*/)
{
  throw exceptions::RMSNotFoundException("Not implemented");
}
}
}
} // namespace rmscore { namespace platform { namespace http {
#endif // ifdef QTFRAMEWORK
