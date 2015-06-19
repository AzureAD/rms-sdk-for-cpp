/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <AcquireTokenNonInteractiveHandler.h>
#include <QByteArray>
#include <types.h>
#include <OAuthConstants.h>

namespace rmsauth {

void AcquireTokenNonInteractiveHandler::addAditionalRequestParameters(RequestParameters& requestParameters)
{
    if (userAssertion_ != nullptr)
    {
        requestParameters.addParam(OAuthConstants::oAuthParameter().GrantType, userAssertion_->assertionType());
        QByteArray qbaBase64Assertion = QByteArray(userAssertion_->assertion().data(), (int)userAssertion_->assertion().size()).toBase64();
        requestParameters.addParam(OAuthConstants::oAuthParameter().Assertion, String(qbaBase64Assertion.begin(), qbaBase64Assertion.end()));
    }
    else
    {
        requestParameters.addParam(OAuthConstants::oAuthParameter().GrantType, OAuthConstants::oAuthGrantType().Password);
        requestParameters.addParam(OAuthConstants::oAuthParameter().Username, userCredential_->userName());
        requestParameters.addParam(OAuthConstants::oAuthParameter().Password, userCredential_->password());
    }

    // To request id_token in response
    requestParameters.addParam(OAuthConstants::oAuthParameter().Scope, OAuthConstants::oAuthValue().ScopeOpenId);
}

} // namespace rmsauth {
