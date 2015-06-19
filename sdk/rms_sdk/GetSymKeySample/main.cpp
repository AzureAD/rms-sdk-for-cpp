/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

//#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QDnsLookup>
#include <memory>
#include "../PFile/IPfileHeaderReader.h"
#include "../PFile/PfileHeader.h"
#include "../Platform/Logger/Logger.h"
#include "../RestClients/LicenseParser.h"
#include "../RestClients/Domain.h"
#include "../RestClients/IDnsLookupClient.h"
#include "../RestClients/IServiceDiscoveryClient.h"
#include "./../ModernAPI/IAuthenticationCallback.h"
#include "./../ModernAPI/AuthenticationCallbackImpl.h"
#include "../Common/CommonTypes.h"
#include "../AuthDialog/Dialog.h"

using namespace rmslib::pfile;
using namespace rmslib::restclients;
using namespace rmslib::common;
using namespace rmslib::modernapi;
using namespace std;

StringArray GetPossibleDomains(std::string& domain);
class AuthCallback : public IAuthenticationCallback {
public:

    virtual std::string GetToken(
        std::shared_ptr<AuthenticationParameters>& ap) override
    {
        Dialog w;
        w.resource = ap->Resource().data();
        w.client_id = ap->UserId().data();
        w.redirect_uri = ap->Scope().data();
        w.authorizeEp = ap->Authority().data();
        w.tokenEp = ap->Authority().data();
        w.exec();
        if(w.result() == QDialog::Accepted)
        {
            qDebug() << "token: " << w.accessToken;
            return w.accessToken.toStdString();
        }
        return std::string();
    }

};

const QString RMS_QUERY_PREFIX = "_rmsdisco._http._tcp.";


int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
    qDebug() << "Hello world";

//    Dialog w;
//    w.exec();
//    if(w.result() == QDialog::Accepted)
//    {
//        qDebug() << "token: " << w.accessToken;

//    }


// --> Get domain from pfile header
    QString pfilePath = QString(SRCDIR) + "data/Sample.ptxt";
    QFile pfile(pfilePath);
    if(!pfile.open(QIODevice::ReadOnly))
    {
        Logger::Error("Can't open the file: %s", pfilePath.toStdString().data());
        return 0;
    }

    auto pfReader = IPfileHeaderReader::Create();
    QDataStream instream(&pfile);
    shared_ptr<PfileHeader> pheader = pfReader->Read(instream);

    auto baMetadata = pheader->GetMetadata();
    string strMetadata((char*)&baMetadata[0], baMetadata.size());
    // qDebug() << strMetadata.data();

    auto baPl = pheader->GetPublishingLicense();
    // std::string strPl((char*)&baPl[0], baPl.size());
    // Logger::Debug("PL:\n%s", strPl.data());

    auto domains = LicenseParser::ExtractDomainsFromPublishingLicense(&baPl[0], baPl.size());

// --> Service discovery call

    auto dnsLookupClient = IDnsLookupClient::Create();
    for (auto domain : domains)
    {

        qDebug() << "==> domain: " << domain->GetOriginalInput().data();

        // DNS lookup to find discovery service URL.
        //auto dnsClientResult = dnsLookupClient->LookupDiscoveryService(domain);
        //auto discoveryUrl = dnsClientResult->GetDiscoveryUrl();
        // Query Discovery service for services details.
//        auto serviceDiscoveryClient = IServiceDiscoveryClient::Create();




        std::string domainString = domain->GetDomainStringForDnsLookup();
        qDebug() << "--> domainString: " << domainString.data();

        auto possibleDomains = GetPossibleDomains(domainString);


        for(auto pd: possibleDomains)
        {


            qDebug() << "--> possible domain: " << pd.data();
            QString dnsRequest = RMS_QUERY_PREFIX + pd.data();
            qDebug() << "--> dnsRequest: " << dnsRequest;

/*
            auto dns = new QDnsLookup();
            QEventLoop loop;
            QObject::connect(dns, SIGNAL(finished()), &loop, SLOT(quit()));

            dns->setType(QDnsLookup::SRV);
            dns->setName(dnsRequest);
            dns->lookup();
            qDebug() << "running loop...";
            loop.exec();
            qDebug() << "loop quited";


            // Check the lookup succeeded.
            if (dns->error() != QDnsLookup::NoError)
            {
                qWarning() << "DNS lookup failed";
                dns->deleteLater();
                continue;
            }

            // Handle the results.
            qDebug() << "dns->canonicalNameRecords(): " << dns->canonicalNameRecords().count();
*/
//            qDebug() << "dns->hostAddressRecords(): " << dns->hostAddressRecords().count();
//            qDebug() << "dns->mailExchangeRecords(): " << dns->mailExchangeRecords().count();
//            qDebug() << "dns->nameServerRecords(): " << dns->nameServerRecords().count();
//            qDebug() << "dns->pointerRecords(): " << dns->pointerRecords().count();
//            qDebug() << "dns->serviceRecords(): " << dns->serviceRecords().count();
//            qDebug() << "dns->textRecords(): " << dns->textRecords().count();
/*
            foreach (const auto &record, dns->canonicalNameRecords())
            {

                string discoveryUrl = record.value().toStdString();
                qDebug() << "record value: " << discoveryUrl.data();
                string userId("4a63455a-cfa1-4ac6-bd2e-0d046cf1c3f7");

                AuthCallback ac;
                AuthenticationCallbackImpl authCallbackImpl(ac,userId);

                auto list = serviceDiscoveryClient->GetServiceDiscoveryDetails(
                    *domain, authCallbackImpl, discoveryUrl);




            }
            dns->deleteLater();
*/
        }
    }
    qDebug() << " --> Service discovery call done!";


// --> Send PL to EUL and get SK + rights

// --> Decrypt data

    return a.exec();
}

StringArray GetPossibleDomains(std::string& domain)
{
    const int MINIMUM_NUMBER_OF_ELEMENTS_IN_DOMAIN = 1;

    vector<string> possibleDomains;
    vector<string> members = Domain::SplitDomains(domain);
    int numberElements     = static_cast<int>(members.size()) -
                           MINIMUM_NUMBER_OF_ELEMENTS_IN_DOMAIN;

    for (int i = 0, lengthFromStart = 0; i < numberElements; ++i)
    {
        possibleDomains.push_back(domain.substr(lengthFromStart));
        lengthFromStart += static_cast<int>(members[i].length()) + 1;
    }

    return possibleDomains;
}



