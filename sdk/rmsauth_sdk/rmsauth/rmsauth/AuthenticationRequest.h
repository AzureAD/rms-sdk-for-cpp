/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef AUTHENTICATIONREQUEST_H
#define AUTHENTICATIONREQUEST_H

#include "types.h"
#include "PromptBehavior.h"

enum class UserIdentifierType
{
    UniqueId,
    LoginHint,
    NoUser
};

/**
 * Represent request and keeps authorization code and similar info.
 */
class AuthenticationRequest
{
public:
//    /**
//     * Developer can use acquiretoken(with loginhint) or acquireTokenSilent(with
//     * userid), so this sets the type of the request.
//     */

//    AuthenticationRequest()
//        : mIdentifierType (UserIdentifierType::NoUser)
//    {}

//    AuthenticationRequest(const String& authority, String resource, const String& client, const String& redirect,
//        const String& loginhint, PromptBehavior prompt, const String& extraQueryParams, UUID correlationId)
//        : mAuthority (authority)
//        , mResource (resource)
//        , mClientId (client)
//        , mRedirectUri (redirect)
//        , mLoginHint (loginhint)
//        , mPrompt (prompt)
//        , mExtraQueryParamsAuthentication (extraQueryParams)
//        , mCorrelationId (correlationId)
//        , mIdentifierType (UserIdentifierType::NoUser)
//    {}

//    AuthenticationRequest(const String& authority, const String& resource, const String& client, const String& redirect, const String& loginhint, UUID correlationId)
//        : mAuthority (authority)
//        , mResource (resource)
//        , mClientId (client)
//        , mRedirectUri (redirect)
//        , mLoginHint (loginhint)
//        , mCorrelationId (correlationId)
//    {}

//    AuthenticationRequest(const String& authority, const String& resource, const String& client, const String& redirect, const String& loginhint)
//        : mAuthority (authority)
//        , mResource (resource)
//        , mClientId (client)
//        , mRedirectUri (redirect)
//        , mLoginHint (loginhint)
//    {}

//    AuthenticationRequest(const String& authority, const String& resource, const String& clientid)
//        : mAuthority (authority)
//        , mResource (resource)
//        , mClientId (clientid)
//    {}

//    /**
//     * Cache usage and refresh token requests.
//     *
//     * @param authority
//     * @param resource
//     * @param clientid
//     * @param userid
//     * @param correlationId
//     */
//    AuthenticationRequest(const String& authority, const String& resource, const String& clientid, const String& userid, const UUID& correlationId)
//        : mAuthority (authority)
//        , mResource (resource)
//        , mClientId (clientid)
//        , mUserId (userid)
//        , mCorrelationId (correlationId)

//    {}

//    AuthenticationRequest(const String& authority, const String& resource, const String& clientId, const UUID& correlationId)
//        : mAuthority (authority)
//        , mResource (resource)
//        , mClientId (clientId)
//        , mCorrelationId (correlationId)

//    {}

//    const String& getAuthority() const
//    {
//        return mAuthority;
//    }

//    void setAuthority(const String& authority)
//    {
//        mAuthority = authority;
//    }

//    const String& getRedirectUri()  const
//    {
//        return mRedirectUri;
//    }

//    const String& getResource() const
//    {
//        return mResource;
//    }

//    const String& getClientId() const
//    {
//        return mClientId;
//    }

//    const String& getLoginHint() const
//    {
//        return mLoginHint;
//    }

//    const UUID& getCorrelationId() const
//    {
//        return this->mCorrelationId;
//    }

//    const String& getExtraQueryParamsAuthentication() const
//    {
//        return mExtraQueryParamsAuthentication;
//    }

//    String getLogInfo() const
//    {
//        StringStream ss;
//        ss  << "Request authority: " << mAuthority
//            << " resource: " << mResource
//            << " clientid: " << mClientId;
//        return ss.str();
//    }

//    const PromptBehavior& getPrompt() const
//    {
//        return mPrompt;
//    }

//    void setPrompt(const PromptBehavior& prompt)
//    {
//        this->mPrompt = prompt;
//    }

//    /**
//     * @return the mRequestId related to the delegate
//     */
//    int getRequestId() const
//    {
//        return mRequestId;
//    }

//    /**
//     * @param requestId the requestId to set
//     */
//    void setRequestId(int requestId)
//    {
//        this->mRequestId = requestId;
//    }

//    const String& getBrokerAccountName() const
//    {
//        return mBrokerAccountName;
//    }

//    void setBrokerAccountName(const String& brokerAccountName)
//    {
//        this->mBrokerAccountName = brokerAccountName;
//    }

//    void setLoginHint(const String& name)
//    {
//        mLoginHint = name;
//    }

//    const String& getUserId() const
//    {
//        return mUserId;
//    }

//    void setUserId(const String& userId)
//    {
//        this->mUserId = userId;
//    }

//    bool isSilent() const
//    {
//        return mSilent;
//    }

//    void setSilent(bool silent)
//    {
//        this->mSilent = silent;
//    }

//    const String& getVersion() const
//    {
//        return mVersion;
//    }

//    void setVersion(const String& version)
//    {
//        this->mVersion = version;
//    }

//    const UserIdentifierType& getUserIdentifierType() const
//    {
//        return mIdentifierType;
//    }

//    void setUserIdentifierType(const UserIdentifierType& user)
//    {
//        mIdentifierType = user;
//    }

//private:
//    int mRequestId = 0;

//    String mAuthority;

//    String mRedirectUri;

//    String mResource;

//    String mClientId;

//    String mLoginHint;

//    String mUserId;

//    String mBrokerAccountName;

//    UUID mCorrelationId;

//    String mExtraQueryParamsAuthentication;

//    PromptBehavior mPrompt;

//    bool mSilent = false;

//    String mVersion;

//    UserIdentifierType mIdentifierType;
};
#endif // AUTHENTICATIONREQUEST_H
