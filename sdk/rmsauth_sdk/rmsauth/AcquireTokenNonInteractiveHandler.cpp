/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AcquireTokenNonInteractiveHandler.h>
#include <UserRealmDiscoveryResponse.h>
#include <Logger.h>
#include <utils.h>

namespace rmsauth {

AcquireTokenNonInteractiveHandler::AcquireTokenNonInteractiveHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, const String& clientId, UserCredentialPtr userCredential, bool callSync)
    : AcquireTokenHandlerBase(authenticator, tokenCache, resource, std::make_shared<ClientKey>(clientId), TokenSubjectType::User, callSync)
{
    if (userCredential == nullptr)
    {
        throw RmsauthException("userCredential", "the value is null");
    }

    userCredential_ = userCredential;
}

AcquireTokenNonInteractiveHandler::AcquireTokenNonInteractiveHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, const String& clientId, UserAssertionPtr userAssertion, bool callSync)
    : AcquireTokenHandlerBase(authenticator, tokenCache, resource, std::make_shared<ClientKey>(clientId), TokenSubjectType::User, callSync)
{
    if (userAssertion == nullptr)
    {
        throw RmsauthException("userAssertion", "the value is null");
    }

    userAssertion_ = userAssertion;
}

void AcquireTokenNonInteractiveHandler::preRunAsync()
{
    AcquireTokenHandlerBase::preRunAsync();

    if (userCredential_ != nullptr)
    {
        if (userCredential_->userName().empty())
        {
            Logger::info(Tag(), "User name is empty");
            throw RmsauthException("userCredential_->userName()", "the value is empty");
        }

        displayableId_ = userCredential_->userName();
    }
    else if (userAssertion_ != nullptr)
    {
        displayableId_ = userAssertion_->userName();
    }
}

void AcquireTokenNonInteractiveHandler::preTokenRequest()
{
    AcquireTokenHandlerBase::preTokenRequest();

    if (userAssertion_ == nullptr)
    {
        auto userRealmResponse = UserRealmDiscoveryResponse::createByDiscoveryAsync(authenticator_->userRealmUri(), userCredential_->userName(), callState_);
        Logger::info(Tag(), "User '%' detected as '{%}'", userCredential_->userName(), userRealmResponse.accountType());

        if (StringUtils::compareIC(userRealmResponse.accountType(), "federated") == 0)
        {
            if (userRealmResponse.federationMetadataUrl().empty())
            {
                throw RmsauthException(Constants::rmsauthError().MissingFederationMetadataUrl);
            }

            throw RmsauthException("Not implemented");

//            Url wsTrustUrl = MexParser::fetchWsTrustAddressFromMexAsync(userRealmResponse.federationMetadataUrl(), userCredential_->userAuthType(), callState_);
//            Logger::info(Tag(), "WS-Trust endpoint '{%}' fetched from MEX at '{%}'", wsTrustUrl.toString(), userRealmResponse.federationMetadataUrl());

//            WsTrustResponse wsTrustResponse = WsTrustRequest::sendRequestAsync(wsTrustUrl, userCredential_, callState_);
//            Logger::info(Tag(), "Token of type '{%}' acquired from WS-Trust endpoint", wsTrustResponse.tokenType());

//            // We assume that if the response token type is not SAML 1.1, it is SAML 2
//            userAssertion_ = new UserAssertion(wsTrustResponse.token(), (wsTrustResponse.tokenType() == WsTrustResponse::Saml1Assertion) ? OAuthConstants::oAuthGrantType().Saml11Bearer : OAuthConstants::oAuthGrantType().Saml20Bearer);
        }
        else if (StringUtils::compareIC(userRealmResponse.accountType(), "managed") == 0)
        {
            // handle password grant flow for the managed user
            if (userCredential_->password().empty())
            {
                throw RmsauthException(Constants::rmsauthError().PasswordRequiredForManagedUserError);
            }
        }
        else
        {
            throw RmsauthException(Constants::rmsauthError().UnknownUserType);
        }
    }
}

} // namespace rmsauth {
