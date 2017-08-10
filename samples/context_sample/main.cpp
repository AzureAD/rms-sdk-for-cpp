#include <QCoreApplication>
#include "ModernAPI/WritableDataCollection.h"
#include "ModernAPI/PrebuiltAuthenticationHandler.h"
#include "ModernAPI/Context.h"
#include <IRMSEnvironment.h>
#include <iostream>
#include <rmsauth/IRMSAuthEnvironment.h>

using namespace rmscore::modernapi;

int main(int argc, char *argv[])
{
    auto envRMS = rmscore::modernapi::RMSEnvironment();
    auto envAuth = rmsauth::RMSAuthEnvironment();
    envRMS->LogOption(rmscore::modernapi::IRMSEnvironment::LoggerOption::Always);
    envAuth->LogOption(rmsauth::IRMSAuthEnvironment::LoggerOption::Always);
    std::shared_ptr<IContext> context = IContext::MakeContext(std::string(argv[0]));
    std::string userEmail = "t-joanto@microsoft.com";
    std::string clientID = "4a63455a-cfa1-4ac6-bd2e-0d046cf1c3f7";
    std::string redirect = "https://client.test.app";
    PrebuiltAuthenticationHandler prebuilt(clientID, redirect);
    std::shared_ptr<WritableDataCollection> writableDataCollectionPointer = WritableDataCollection::AcquireWritableDataCollection(userEmail, prebuilt, clientID);
    context->Finish();
}
