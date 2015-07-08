/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "InteractiveTests.h"
#include <types.h>
#include <Url.h>
#include <DateTime.h>
#include <utils.h>
#include <Authenticator.h>
#include <CallState.h>
#include <OAuthConstants.h>
#include <AuthenticationResult.h>
#include <AuthenticationContext.h>
#include <Exceptions.h>
#include <FileCache.h>
#include <QDebug>

Q_DECLARE_METATYPE(rmsauth::String)

using namespace std;
using namespace rmsauth;

void InteractiveTests::AcquireTokenInteractiveHandlerTest_data()
{
    QTest::addColumn<String>("clientId");
    QTest::addColumn<String>("redirectUri");
    QTest::addColumn<String>("resource");
    QTest::addColumn<String>("authority");

    QTest::newRow("TodoListClient")
        << String("")
        << String("")
        << String("")
        << String("")
        ;
}

void InteractiveTests::AcquireTokenInteractiveHandlerTest()
{
    QFETCH(String, clientId);
    QFETCH(String, redirectUri);
    QFETCH(String, resource);
    QFETCH(String, authority);

    const String Tag("CommonTests");

    auto fileCachePtr = std::make_shared<FileCache>();
    ptr<TokenCache> cachePtr = nullptr;
    AuthenticationContext authContext(authority, AuthorityValidationType::False, fileCachePtr);
    AuthenticationResultPtr result = nullptr;

    try
    {
        result = authContext.acquireToken(resource, clientId, redirectUri, PromptBehavior::Auto);
        qDebug() << Tag.c_str() << " Access token: " << result->accessToken().c_str();

        String resultString = result->serialize();
        qDebug() << Tag.c_str() << " AuthenticationResult string: " << resultString.c_str();
    }
    catch(const Exception& ex)
    {
        QTest::qFail(ex.error().data(), __FILE__, __LINE__);
    }
}
