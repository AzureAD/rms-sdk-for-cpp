/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifdef QTFRAMEWORK
#include <QDnsLookup>
#include <QEventLoop>
#include "DnsServerResolverQt.h"
#include <QUdpSocket>
#include <QThread>
#include "../../Platform/Logger/Logger.h"

using namespace std;
using namespace rmscore::common;
using namespace rmscore::platform::logger;

namespace rmscore {
namespace platform {
namespace http {
shared_ptr<IDnsServerResolver>IDnsServerResolver::Create()
{
  return make_shared<DnsServerResolverQt>();
}

std::string DnsServerResolverQt::lookup(const std::string& dnsRequest)
{
  QDnsLookup dns;

  dns.setType(QDnsLookup::SRV);
  Logger::Hidden("dnsRequest: %s", dnsRequest.c_str());
  dns.setName(dnsRequest.c_str());
  dns.lookup();
  QEventLoop loop;
  QObject::connect(&dns, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec();

  if (dns.error() != QDnsLookup::NoError)
  {
    qWarning("DNS lookup failed");
  }
  foreach(const QDnsServiceRecord &record, dns.serviceRecords())
  {
    Logger::Hidden("QDnsServiceRecord record: %s --> %s",
                   record.name().toStdString().c_str(),
                   record.target().toStdString().c_str());

    return record.target().toStdString();
  }
  return "";
}
} // namespace http
} // namespace platform
} // namespace rmscore
#endif // ifdef QTFRAMEWORK
