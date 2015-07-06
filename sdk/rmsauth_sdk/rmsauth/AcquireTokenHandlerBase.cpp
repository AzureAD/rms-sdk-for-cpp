/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AcquireTokenHandlerBase.h>
#include <Logger.h>
#include <OAuth2Response.h>

namespace rmsauth {

AcquireTokenHandlerBase::AcquireTokenHandlerBase(AuthenticatorPtr authenticator, TokenCachePtr tokenCache, const String &resource, ClientKeyPtr clientKey, TokenSubjectType subjectType, bool callSync)
    : authenticator_(authenticator)
    , tokenCache_(tokenCache)
    , resource_(resource)
    , clientKey_(clientKey)
    , tokenSubjectType_(subjectType)
    , loadFromCache_(tokenCache != nullptr)
    , storeToCache_(tokenCache != nullptr)
    , supportADFS_(false)
{
    Logger::info(Tag(), "AcquireTokenHandlerBase");

    callState_ = createCallState(authenticator->correlationId(), callSync);

    StringStream chacheInfo;
    tokenCache != nullptr
        ? chacheInfo << tokenCache->getCacheName() << " (" << tokenCache->count() << " items)"
        : chacheInfo << "nullptr";

    Logger::info(Tag(), "=== Token Acquisition started:\n\tAuthority: %\n\tResource: %\n\tClientId: %\n\tCacheType: %\n\tAuthentication Target: %\n\t",
        authenticator->authority(), resource, clientKey->clientId(), chacheInfo.str(), static_cast<int>(subjectType));

    if (resource.empty())
    {
        Logger::error(Tag(), "AcquireTokenHandlerBase: resource is empty");
        throw IllegalArgumentException("resource");
    }
}

AuthenticationResultPtr AcquireTokenHandlerBase::runAsync()
{
    Logger::info(Tag(), "runAsync");
    bool notifiedBeforeAccessCache = false;

    try
    {
        preRunAsync();

        AuthenticationResultPtr result = nullptr;
        if (loadFromCache_)
        {
            notifyBeforeAccessCache();
            notifiedBeforeAccessCache = true;

            result = tokenCache_->loadFromCache(authenticator_->authority(), resource_, clientKey_->clientId(), tokenSubjectType_, uniqueId_, /*displayableId_, */callState_);

            if (result != nullptr && result->accessToken().empty() && !result->refreshToken().empty())
            {
                result = refreshAccessTokenAsync(result);
                if (result != nullptr)
                {
                    tokenCache_->storeToCache(result, authenticator_->authority(), resource_, clientKey_->clientId(), tokenSubjectType_, callState_);
                }
            }
        }

        if (result == nullptr)
        {
            preTokenRequest();
            result = sendTokenRequestAsync();
            postTokenRequest(result);

            if (this->storeToCache_)
            {
                if (!notifiedBeforeAccessCache)
                {
                    notifyBeforeAccessCache();
                    notifiedBeforeAccessCache = true;
                }

                tokenCache_->storeToCache(result, authenticator_->authority(), resource_, clientKey_->clientId(), tokenSubjectType_, callState_);
            }
        }

        postRunAsync(result);

        if (notifiedBeforeAccessCache)
        {
            notifyAfterAccessCache();
        }

        return result;
    }
    catch (const Exception& ex)
    {
        if (notifiedBeforeAccessCache)
        {
            this->notifyAfterAccessCache();
        }
        Logger::error(Tag(), "runAsync(): exception: %", ex.error());
        throw;
    }
}

CallStatePtr AcquireTokenHandlerBase::createCallState(const Guid& correlationId, bool callSync)
{
    Logger::info(Tag(), "createCallState");
    auto id = (!correlationId.empty()) ? correlationId : Guid::newGuid();

    return std::make_shared<CallState>(id, callSync);
}

void AcquireTokenHandlerBase::preRunAsync()
{
    Logger::info(Tag(), "preRunAsync");
    authenticator_-> updateFromTemplateAsync(callState_);
    validateAuthorityType();
}

void AcquireTokenHandlerBase::postRunAsync(AuthenticationResultPtr result)
{
    Logger::info(Tag(), "postRunAsync");
    logReturnedToken(result);
}

void AcquireTokenHandlerBase::preTokenRequest()
{
    Logger::info(Tag(), "preTokenRequest");
}

void AcquireTokenHandlerBase::postTokenRequest(AuthenticationResultPtr result)
{
    Logger::info(Tag(), "postTokenRequest");
    authenticator_->updateTenantId(result->tenantId());
}

AuthenticationResultPtr AcquireTokenHandlerBase::sendTokenRequestAsync()
{
    Logger::info(Tag(), "sendTokenRequestAsync");
    RequestParameters requestParameters(resource_, clientKey_);
    addAditionalRequestParameters(requestParameters);

    return sendHttpMessageAsync(requestParameters);
}

AuthenticationResultPtr AcquireTokenHandlerBase::sendTokenRequestByRefreshTokenAsync(const String& refreshToken)
{
    Logger::info(Tag(), "sendTokenRequestByRefreshTokenAsync");
    RequestParameters requestParameters(resource_, clientKey_);
    requestParameters.addParam(OAuthConstants::oAuthParameter().GrantType, OAuthConstants::oAuthGrantType().RefreshToken);
    requestParameters.addParam(OAuthConstants::oAuthParameter().RefreshToken, refreshToken);
    AuthenticationResultPtr result = sendHttpMessageAsync(requestParameters);

    if (result->refreshToken().empty())
    {
        result->refreshToken(refreshToken);
    }

    return result;
}

AuthenticationResultPtr AcquireTokenHandlerBase::refreshAccessTokenAsync(AuthenticationResultPtr result)
{
    Logger::info(Tag(), "refreshAccessTokenAsync");
    AuthenticationResultPtr newResult = nullptr;
    if (!resource_.empty())
    {
        try
        {
            newResult = sendTokenRequestByRefreshTokenAsync(result->refreshToken());
            authenticator_->updateTenantId(result->tenantId());

            if (newResult->idToken().empty())
            {
                // If Id token is not returned by token endpoint when refresh token is redeemed, we should copy tenant and user information from the cached token.
                newResult->updateTenantAndUserInfo(result->tenantId(), result->idToken(), result->userInfo());
            }
        }
        catch (const RmsauthServiceException& ex)
        {
            if(StringUtils::equalsIC(ex.error(), "invalid_request"))
            {
                throw RmsauthServiceException(
                    Constants::rmsauthError().FailedToRefreshToken,
                    Constants::rmsauthErrorMessage().FailedToRefreshToken + ". " + ex.message());
            }
        }
        catch (const RmsauthException&)
        {
            newResult = nullptr;
        }
    }

    return newResult;
}

AuthenticationResultPtr AcquireTokenHandlerBase::sendHttpMessageAsync(const RequestParameters& requestParameters)
{
    Logger::info(Tag(), "sendHttpMessageAsync");

//    String uri = HttpHelper::CheckForExtraQueryParameter(authenticator_->tokenUri());
    auto uri = authenticator_->tokenUri();

    TokenResponsePtr tokenResponse = HttpHelper::sendPostRequestAndDeserializeJsonResponseAsync(uri, requestParameters, callState_);

    return OAuth2Response::parseTokenResponse(tokenResponse, callState_);
}

void AcquireTokenHandlerBase::notifyBeforeAccessCache()
{
    Logger::info(Tag(), "notifyBeforeAccessCache");
    TokenCacheNotificationArgs args(
        tokenCache_.get(),
        resource_,
        clientKey_->clientId(),
        uniqueId_/*,
        displayableId_*/);

    tokenCache_->onBeforeAccess(args);
}

void AcquireTokenHandlerBase::notifyAfterAccessCache()
{
    Logger::info(Tag(), "notifyAfterAccessCache");
    TokenCacheNotificationArgs args(
        tokenCache_.get(),
        resource_,
        clientKey_->clientId(),
        uniqueId_/*,
        displayableId_*/);

    tokenCache_->onAfterAccess(args);
}

void AcquireTokenHandlerBase::logReturnedToken(AuthenticationResultPtr result)
{
    if (!result->accessToken().empty())
    {
        String accessTokenHash = HashUtils::createSha256Hash(result->accessToken());
        String refreshTokenHash;
        if (!result->refreshToken().empty())
        {
            refreshTokenHash = HashUtils::createSha256Hash(result->refreshToken());
        }
        else
        {
            refreshTokenHash = "[No Refresh Token]";
        }

        Logger::info(Tag(), "=== Token Acquisition finished successfully. An access token was retuned:\n\tAccess Token Hash: %\n\tRefresh Token Hash: %\n\tExpiration Time: % (%)\n\tUser Hash: %\n\t",
            accessTokenHash, refreshTokenHash, DateTime(result->expiresOn()).toString("HH:mm:ss MM.dd.yy"), result->expiresOn(),
            result->userInfo() != nullptr ?  HashUtils::createSha256Hash(result->userInfo()->uniqueId()) : "nullptr");
    }
}

void AcquireTokenHandlerBase::validateAuthorityType()
{
    Logger::info(Tag(), "validateAuthorityType");
    if (!supportADFS_ && authenticator_->authorityType() == AuthorityType::ADFS)
    {
        StringStream ds;
        ds << Constants::rmsauthError().InvalidAuthorityType
           << ", " << Constants::rmsauthErrorMessage().InvalidAuthorityTypeTemplate
           << authenticator_->authority();
        throw RmsauthException(ds.str());
    }
}

} // namespace rmsauth {
