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
#include <QDebug>
#include <QThread>

using namespace std;
using namespace rmscore::common;

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
    qDebug() << "dnsRequest: " << dnsRequest.c_str();
    dns.setName(dnsRequest.c_str());
    dns.lookup();
    QEventLoop loop;
    QObject::connect(&dns, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (dns.error() != QDnsLookup::NoError)
    {
        qWarning("DNS lookup failed");
    }
    foreach (const QDnsServiceRecord &record, dns.serviceRecords())
    {
        qDebug() << "QDnsServiceRecord record: " << record.name() << " --> " << record.target();
        return record.target().toStdString();
    }
    return "";
}

} // namespace http
} // namespace platform
} // namespace rmscore
#endif
