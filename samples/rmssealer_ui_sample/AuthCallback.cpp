#include "AuthCallback.h"

AuthCallback::AuthCallback(const std::string& clientId, const std::string& uri): clientId(clientId), redirectionURI(uri){}

std::string AuthCallback::GetToken(std::shared_ptr<AuthenticationParameters>& ap){
    try{
        AuthenticationContext authContext(ap->Authority(), AuthorityValidationType::False, nullptr);
        auto result = authContext.acquireToken(ap->Resource(),
                                               clientId,
                                               redirectionURI,
                                               PromptBehavior::Auto,
                                               ap->UserId());
        return result->accessToken();
      }catch (const rmsauth::Exception&){
        throw;
      }
}



