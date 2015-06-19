/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <QSsl>
#include <QSslConfiguration>
#include "HttpHelper.h"

namespace rmscore {
namespace modernapi {
bool addCACertificate(const std::vector<uint8_t>& certificate,
                      QSsl::EncodingFormat        format) {
  QSslConfiguration SslConfiguration(QSslConfiguration::defaultConfiguration());

  QList<QSslCertificate> certificates = SslConfiguration.caCertificates();

  QList<QSslCertificate> cert = QSslCertificate::fromData(
    QByteArray(reinterpret_cast<const char *>(certificate.data()),
               static_cast<int>(certificate.size())), format);

  if (cert.length() == 0) return false;

  certificates.append(cert);

  SslConfiguration.setCaCertificates(certificates);
  QSslConfiguration::setDefaultConfiguration(SslConfiguration);

  return true;
}

bool HttpHelper::addCACertificateBase64(const std::vector<uint8_t>& certificate) {
  return addCACertificate(certificate, QSsl::Pem);
}

bool HttpHelper::addCACertificateDer(const std::vector<uint8_t>& certificate) {
  return addCACertificate(certificate, QSsl::Der);
}
} // namespace modernapi
} // namespace rmscore
