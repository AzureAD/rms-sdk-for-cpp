/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifdef QTFRAMEWORK
#include "DnsServerResolverQt.h"
#include "../../Platform/Logger/Logger.h"

#ifdef WIN32
#include <windows.h>
#include <windns.h>

#elif POSIX


#endif
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
    #ifdef WIN32
    PDNS_RECORD dnsRecord;
    DNS_STATUS dnsStatus = ERROR_SUCCESS;
    wchar_t *request=NULL;
    auto size= mbstowcs(request, dnsRequest.c_str(), dnsRequest.length());
    if(size<dnsRequest.length()){
        Logger::Hidden("Unable to create request looking up record for %s with %d",
                       dnsRequest.c_str(),dnsStatus);
    }

    dnsStatus = DnsQuery(request, DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &dnsRecord, NULL);
    if(ERROR_SUCCESS != dnsStatus)
    {
        Logger::Hidden("DNS Lookup failed looking up record for %s with %d",
                       dnsRequest.c_str(),dnsStatus);
        return "";
    }

    else
    {
        if((nullptr != dnsRecord) && (dnsRecord->wType == DNS_TYPE_SRV))
            {
                //Return first record
                char *target=NULL;
                auto len= wcstombs(target,dnsRecord->Data.SRV.pNameTarget,sizeof(dnsRecord->Data.SRV.pNameTarget));
                DnsFree(dnsRecord, DnsFreeRecordList);
                return target ;
            }
        else
            {
                return "";
            }
    }

    #elif POSIX


    #endif

 /* QDnsLookup dns;

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

  */
}
} // namespace http
} // namespace platform
} // namespace rmscore
#endif // ifdef QTFRAMEWORK
