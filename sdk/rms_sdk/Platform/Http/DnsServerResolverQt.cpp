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
#include <QCoreApplication>
#include <QTimer>
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

std::string DnsServerResolverQt::doLookup(const std::string& dnsRequest)
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

std::string DnsServerResolverQt::lookup(const std::string& dnsRequest)
{
  // If a QCoreApplication does not exist, create a temporary instance.
  // QCoreApplication is a singleton, but it can keep getting created and destroyed.
  // QtNetwork calls need to be made from within the scope of the QCoreApplication created.
  if (!QCoreApplication::instance()){
      int argc = 1;
      char name[] = "DnsServerResolverQt::lookup";
      char **argv = new char *[argc];
      argv[0] = name;
      QCoreApplication a(argc, argv);

      auto result = doLookup(dnsRequest);

      QTimer::singleShot(0, &a, SLOT(quit()));
      a.exec();

      return result;
  }

  return doLookup(dnsRequest);
}


} // namespace http
} // namespace platform
} // namespace rmscore
#endif // ifdef QTFRAMEWORK
