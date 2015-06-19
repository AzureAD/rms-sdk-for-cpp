/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AcquireTokenInteractiveHandler.h>
#include <OAuth2Response.h>

namespace rmsauth {

AcquireTokenInteractiveHandler::AcquireTokenInteractiveHandler(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String& resource, const String& clientId, const String& redirectUri, PromptBehavior promptBehavior, UserIdentifierPtr userId, const String& extraQueryParameters, IWebUIPtr webUI, bool callSync)
    : AcquireTokenHandlerBase(authenticator, tokenCache, resource, std::make_shared<ClientKey>(clientId), TokenSubjectType::User, callSync)
{
    Logger::info(Tag(), "AcquireTokenInteractiveHandler");

    if (redirectUri.empty())
    {
        throw new IllegalArgumentException("redirectUri");
    }

    redirectUri_ = redirectUri;

    if (!Url(redirectUri).fragment().empty())
    {
        throw new IllegalArgumentException(Constants::rmsauthErrorMessage().RedirectUriContainsFragment, "redirectUri");
    }

    setRedirectUriRequestParameter();

    if (userId == nullptr)
    {
        throw new IllegalArgumentException("userId", Constants::rmsauthErrorMessage().SpecifyAnyUser);
    }

    userId_ = userId;

    promptBehavior_ = promptBehavior;

    if (!extraQueryParameters.empty() && extraQueryParameters[0] == '&')
    {
        extraQueryParameters_ = extraQueryParameters.substr(1);
    }

    extraQueryParameters_ = extraQueryParameters;

    webUi_ = webUI;

    uniqueId_ = userId->uniqueId();
    displayableId_ = userId->displayableId();
    userIdentifierType_ = userId->type();

    loadFromCache_ = (tokenCache_ != nullptr && promptBehavior_ != PromptBehavior::Always && promptBehavior_ != PromptBehavior::RefreshSession);

    supportADFS_ = true;
}

void AcquireTokenInteractiveHandler::addAditionalRequestParameters(RequestParameters& requestParameters)
{
    requestParameters.addParam(OAuthConstants::oAuthParameter().GrantType, OAuthConstants::oAuthGrantType().AuthorizationCode);
    requestParameters.addParam(OAuthConstants::oAuthParameter().Code, authorizationResult_->code());
    requestParameters.addParam(OAuthConstants::oAuthParameter().RedirectUri, redirectUriRequestParameter_);
}

void AcquireTokenInteractiveHandler::postTokenRequest(AuthenticationResultPtr result)
{
    Logger::info(Tag(), "postTokenRequest");

    AcquireTokenHandlerBase::postTokenRequest(result);
    if ((displayableId_.empty() && uniqueId_.empty()) || userIdentifierType_ == UserIdentifierType::OptionalDisplayableId)
    {
        return;
    }

    String uniqueId = (result->userInfo() != nullptr && !result->userInfo()->uniqueId().empty()) ? result->userInfo()->uniqueId() : "NULL";
    String displayableId = (result->userInfo() != nullptr) ? result->userInfo()->displayableId() : "NULL";

    if (userIdentifierType_ == UserIdentifierType::UniqueId && uniqueId_.compare(uniqueId_) != 0)
    {
        throw RmsauthUserMismatchException(uniqueId_, uniqueId);
    }

    if (userIdentifierType_ == UserIdentifierType::RequiredDisplayableId && displayableId.compare(displayableId_) != 0)
    {
        throw RmsauthUserMismatchException(displayableId_, displayableId);
    }
}

String AcquireTokenInteractiveHandler::createAuthorizationUri(bool includeFormsAuthParam)
{
    Logger::info(Tag(), "createAuthorizationUri");

    String loginHint;

    if (!userId_->isAnyUser()
        && (userId_->type() == UserIdentifierType::OptionalDisplayableId
            || userId_->type() == UserIdentifierType::RequiredDisplayableId))
    {
        loginHint = userId_->id();
    }

    RequestParameters requestParameters = createAuthorizationRequest(loginHint, includeFormsAuthParam);

    String urlString = authenticator_->authorizationUri() + "?" + requestParameters.toString();
//        auto authorizationUri = std::make_shared<Url>(urlString);
//        authorizationUri = new Uri(HttpHelper.CheckForExtraQueryParameter(authorizationUri.AbsoluteUri));

    return urlString;
}

RequestParameters AcquireTokenInteractiveHandler::createAuthorizationRequest(const String& loginHint, bool includeFormsAuthParam)
{
    Logger::info(Tag(), "createAuthorizationRequest");

    RequestParameters authorizationRequestParameters(resource_, clientKey_);
    authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().ResponseType, OAuthConstants::oAuthResponseType().Code);

    authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().RedirectUri, redirectUriRequestParameter_);

    if (!loginHint.empty())
    {
        authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().LoginHint, loginHint);
    }

    if (callState_ != nullptr && !callState_->correlationId().empty())
    {
        authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().CorrelationId, callState_->correlationId().toString());
    }

    // ADFS currently ignores the parameter for now.
    if (promptBehavior_ == PromptBehavior::Always)
    {
        authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().Prompt, OAuthConstants::promptValue().Login);
    }
    else if (promptBehavior_ == PromptBehavior::RefreshSession)
    {
        authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().Prompt, OAuthConstants::promptValue().RefreshSession);
    }
    else if (promptBehavior_ == PromptBehavior::Never)
    {
        authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().Prompt, OAuthConstants::promptValue().AttemptNone);
    }

    if (includeFormsAuthParam)
    {
        authorizationRequestParameters.addParam(OAuthConstants::oAuthParameter().FormsAuth, OAuthConstants::oAuthValue().FormsAuth);
    }

    AcquireTokenInteractiveHandler::addHeadersToRequestParameters(authorizationRequestParameters, RmsauthIdHelper::getProductHeaders());
    AcquireTokenInteractiveHandler::addHeadersToRequestParameters(authorizationRequestParameters, RmsauthIdHelper::getPlatformHeaders());

    if (!extraQueryParameters_.empty())
    {
// TODO.shch: Checks for extraQueryParameters duplicating standard parameters
        authorizationRequestParameters.extraQueryParameter(extraQueryParameters_);
    }

    return authorizationRequestParameters;
}

void AcquireTokenInteractiveHandler::addHeadersToRequestParameters(RequestParameters& requestParameters, Headers headers)
{
    for(auto& header : headers)
    {
        requestParameters.addParam(header.first, header.second);
    }
}

void AcquireTokenInteractiveHandler::verifyAuthorizationResult()
{
    Logger::info(Tag(), "verifyAuthorizationResult");

    if (promptBehavior_ == PromptBehavior::Never && authorizationResult_->error() == OAuthConstants::oAuthError().LoginRequired)
    {
        throw RmsauthException(Constants::rmsauthError().UserInteractionRequired);
    }

    if (authorizationResult_->status() != AuthorizationStatus::Success)
    {
        throw RmsauthServiceException(authorizationResult_->error(), authorizationResult_->errorDescription());
    }
}

void AcquireTokenInteractiveHandler::preTokenRequest()
{
    Logger::info(Tag(), "preTokenRequest");

    AcquireTokenHandlerBase::preTokenRequest();

    acquireAuthorization();
    verifyAuthorizationResult();
}

void AcquireTokenInteractiveHandler::acquireAuthorization()
{
    Logger::info(Tag(), "acquireAuthorization");
    sendAuthorizeRequest();
}

void AcquireTokenInteractiveHandler::sendAuthorizeRequest()
{
    Logger::info(Tag(), "sendAuthorizeRequest");
    String authorizationUri = createAuthorizationUri(includeFormsAuthParams());
    String resultUri = webUi_->authenticate(authorizationUri, redirectUri_);
    authorizationResult_ = OAuth2Response::parseAuthorizeResponse(resultUri, callState_);
}

bool AcquireTokenInteractiveHandler::includeFormsAuthParams()
{
    Logger::info(Tag(), "includeFormsAuthParams");
    return isUserLocal() && isDomainJoined();
}

String AcquireTokenInteractiveHandler::createAuthorizationUriAsync(const Guid& correlationId)
{
    Logger::info(Tag(), "createAuthorizationUriAsync");
    callState_->correlationId(correlationId);
    authenticator_->updateFromTemplateAsync(callState_);
    return createAuthorizationUri(false);
}

bool AcquireTokenInteractiveHandler::isDomainJoined()
{
// TODO.shch: figure out what it is.
    bool returnValue = false;
    return returnValue;
}

bool AcquireTokenInteractiveHandler::isUserLocal()
{
// TODO.shch: figure out what it is.
//        string prefix = WindowsIdentity.GetCurrent().Name.Split('\\')[0].ToUpperInvariant();
//        return prefix.Equals(Environment.MachineName.ToUpperInvariant());
    return true;
}

void AcquireTokenInteractiveHandler::setRedirectUriRequestParameter()
{
    redirectUriRequestParameter_ = redirectUri_;
}

} // namespace rmsauth {
