#include "../ModernAPI/IAuthenticationCallback.h"
#include "../ModernAPI/IAuthenticationCallbackImpl.h"
#include "../ModernAPI/AuthenticationCallbackImpl.h"
#include "../rmsauth/FileCache.h"
#include "../RestClients/AuthenticationHandler.h"
#include "../ModernAPI/AuthenticationParameters.h"
#include "../rmsauth/AuthenticationContext.h"

#ifndef AUTHCALLBACK_H
#define AUTHCALLBACK_H

using namespace rmsauth;
using namespace rmscore::modernapi;
using namespace rmscore::restclients;

class AuthCallback : public IAuthenticationCallback {

public:
    AuthCallback(const std::string& clientId, const std::string& uri);
    virtual std::string GetToken(std::shared_ptr<AuthenticationParameters>& ap) override;

private:
    const std::string& clientId;
    const std::string& redirectionURI;
};


#endif // AUTHCALLBACK_H
