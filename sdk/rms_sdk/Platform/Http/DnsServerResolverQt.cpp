/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifdef QTFRAMEWORK

#include "DnsServerResolverQt.h"

#ifdef _WIN32
#include <windows.h>
#include <windns.h>
#elif __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>

#endif

#include "../../Platform/Logger/Logger.h"
#define MAX_DNS_NAME_BUF_SIZE 2048
using namespace rmscore::common;
using namespace rmscore::platform::logger;
using namespace std;

namespace rmscore {
namespace platform {
namespace http {

shared_ptr<IDnsServerResolver>IDnsServerResolver::Create() {
  return make_shared<DnsServerResolverQt>();
}


#ifdef _WIN32
std::string DnsServerResolverQt::lookup(const std::string& dnsRequest) {
  PDNS_RECORD dnsRecord;
  DNS_STATUS dnsStatus = ERROR_SUCCESS;
  std::wstring request(dnsRequest.begin(),dnsRequest.end());
  dnsStatus = DnsQuery(request.c_str(), DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL,
      &dnsRecord, NULL);
  if (ERROR_SUCCESS != dnsStatus) {
    Logger::Hidden("DNS Lookup failed looking up record for %s with %d",
        dnsRequest.c_str(), dnsStatus);
    return "";
  } else if ((nullptr != dnsRecord) && (dnsRecord->wType == DNS_TYPE_SRV)) {
    //Return first record
    std::wstring wdnsName= dnsRecord->Data.SRV.pNameTarget;
    std::string dnsName(wdnsName.begin(),wdnsName.end());
    DnsFree(dnsRecord, DnsFreeRecordList);
    return dnsName;
  } else {
    return "";
  }
}

#elif __linux__
std::string DnsServerResolverQt::lookup(const std::string& dnsRequest) {
  //Initialize dns lookup query
  res_init();
  unsigned char queryResult[1024];
  int response = res_query(dnsRequest.c_str(), C_IN, ns_t_srv,
      queryResult, sizeof(queryResult));
  if (response < 0) {
    Logger::Hidden("DNS Lookup failed looking up record for %s: Insufficient buffer",
        dnsRequest.c_str());
    return "";
  } else {
    int domainSize = -1;
    ns_msg result;
    char dnsName[MAX_DNS_NAME_BUF_SIZE] = {0};
    ns_initparse(queryResult,response,&result);
    for (int i = 0; i < ns_msg_count(result, ns_s_an); i++) {
      ns_rr resourceRecord;
      ns_parserr(&result, ns_s_an, i,&resourceRecord);
      domainSize = dn_expand(ns_msg_base(result), ns_msg_end(result),
          ns_rr_rdata(resourceRecord) + 6, dnsName, sizeof(dnsName));
      if (domainSize > 0) {
        //Found a succesful record.
        break;
      }
    }
    if (domainSize < 0) {
      Logger::Hidden("DNS Lookup failed looking up record for %s: Insufficient buffer",
        dnsRequest.c_str());
      return "";
    }
    return string(dnsName);
  }
}
#endif

}// namespace http
}// namespace platform
}// namespace rmscore
#endif // ifdef QTFRAMEWORK
