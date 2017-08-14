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

#elif __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>

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
                char *dnsName=NULL;
                auto len= wcstombs(dnsName,dnsRecord->Data.SRV.pNameTarget,sizeof(dnsRecord->Data.SRV.pNameTarget));
                DnsFree(dnsRecord, DnsFreeRecordList);
                return dnsName ;
            }
        else
            {
                return "";
            }
    }

    #elif __linux__

    //Initialize query

    res_init();
    unsigned char queryResult[1024];
    int response= res_query(dnsRequest.c_str(),C_IN,ns_t_srv,queryResult,sizeof(queryResult));
    if(response<0)
        {
            Logger::Hidden("DNS Lookup failed looking up record for %s: Insufficient buffer",
                       dnsRequest.c_str());
            return "";
        }
    else
        {
            ns_msg result;
            char dnsName[1024];
            ns_initparse(queryResult,response,&result);
            for(int i=0;i<ns_msg_count(result,ns_s_an);i++)
                {
                    ns_rr resourceRecord;
                    ns_parserr(&result,ns_s_an, i,&resourceRecord);
                    dn_expand(ns_msg_base(result),ns_msg_end(result),ns_rr_rdata(resourceRecord)+6,dnsName,sizeof(dnsName));
                    return string(dnsName);
                }

        }
    #endif

    return "";
}
} // namespace http
} // namespace platform
} // namespace rmscore
#endif // ifdef QTFRAMEWORK
