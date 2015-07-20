/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "NonInteractiveTests.h"
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
#include <FileCacheEncrypted.h>
#include <UserCredential.h>
#include <ClientCredential.h>

Q_DECLARE_METATYPE(rmsauth::String)

using namespace std;
using namespace rmsauth;

NonInteractiveTests::NonInteractiveTests(QString clientId,
                                         QString resource,
                                         QString authority,
                                         QString userName,
                                         QString password,
                                         QString clientSecret)
  : clientId_(clientId), resource_(resource), authority_(authority),
    userName_(userName), password_(password), clientSecret_(clientSecret)
{}

void NonInteractiveTests::AuthenticationResultTest_data()
{
  QTest::addColumn<String>("authenticationResult");
  QTest::addColumn<int>(   "expiresOn");
  QTest::addColumn<String>("displayableId");
  QTest::newRow("auth1")
    << String(
    "{\"accessToken\":\"dummy-7MSw\",\"accessTokenType\":\"bearer\",\"expiresOn\":1430427072,\"idToken\":\"\",\"isMultipleResourceRefreshToken\":false,\"refreshToken\":\"dummy-PCYAfA\",\"resource\":\"\",\"tenantId_\":\"\"}")
    << 1430427072
    << String("")
  ;

  QTest::newRow("client.test.app")
    << String(
    "{\"accessToken\":\"dummy-eyJciOiJTaGEyNTYiLCFcp7msB2r0ehsA\",\"accessTokenType\":\"jwt\",\"expiresOn\":1430449424,\"idToken\":\"\",\"isMultipleResourceRefreshToken\":false,\"refreshToken\":\"dummy-hza0f5VdK3hmGFJFW-Wko976\",\"resource\":\"\",\"tenantId_\":\"\"}")
    << 1430449424
    << String("")
  ;

  QTest::newRow("TodoListClient")
    << String(
    "{\"accessToken\":\"dummy-eyJAiOiJKV1QiLCJhb_GSCkWgHncpw\",\"accessTokenType\":\"Bearer\",\"expiresOn\":1430416203,\"idToken\":\"dummy-eyJ0eXAiO3B4In0.\",\"isMultipleResourceRefreshToken\":true,\"refreshToken\":\"dummy-AAABAAAAiL9Kn2Z27P2yAA\",\"resource\":\"https://shch.onmicrosoft.com/TodoListService\",\"tenantId_\":\"b0570dfb-b143-4f35-b385-ed9796b2137a\",\"userInfo\":{\"displayableId\":\"qqq@shch.onmicrosoft.com\",\"familyName\":\"Q2\",\"forcePrompt\":true,\"givenName\":\"Q1\",\"identityProvider\":\"https://sts.windows.net/dummy--b143-4f35-b385-ed9796b2137a/\",\"passwordChangeUrl\":\"https://portal.microsoftonline.com/ChangePassword.aspx\",\"passwordExpiresOn\":1432773087,\"uniqueId\":\"dummy--9a6c-443c-8245-4d80676274fe\"}}")
    << 1430416203
    << String("qqq@shch.onmicrosoft.com")
  ;
}

void NonInteractiveTests::AuthenticationResultTest()
{
  QFETCH(String, authenticationResult);
  QFETCH(int,    expiresOn);
  QFETCH(String, displayableId);

  qDebug() << "====== NonInteractiveTests::AuthenticationResultTest() ======";
  auto authRes = AuthenticationResult::deserialize(authenticationResult);
  QVERIFY(authRes->expiresOn() == expiresOn);
  auto   userInfoPtr = authRes->userInfo();
  String dId         = userInfoPtr != nullptr ? userInfoPtr->displayableId() : "";
  QVERIFY(dId == displayableId);
}

void NonInteractiveTests::CacheTest_data()
{
  QTest::addColumn<String>("clientId");
  QTest::addColumn<String>("redirectUri");
  QTest::addColumn<String>("resource");
  QTest::addColumn<String>("authority");
  QTest::addColumn<String>("authenticationResult");

  QTest::newRow("auth1")
    << String("dummy-bd86-40a0-87ce-7c49cbab402d")
    << String("https://auth1")
    << String("auth1.com")
    << String("https://auth1/oauth2/authorize")
    << String(
    "{\"accessToken\":\"dummy-sdfdsafw\",\"accessTokenType\":\"bearer\",\"expiresOn\":1430427072,\"idToken\":\"\",\"isMultipleResourceRefreshToken\":false,\"refreshToken\":\"dummy--PCYAfA\",\"resource\":\"\",\"tenantId_\":\"\"}")
  ;

  QTest::newRow("auth3")
    << String("dummy-61b4-4235-bbc4-ca382dd3d52e")
    << String("http://auth3")
    << String("https://auth3")
    << String("https://login.windows.net/auth3.onmicrosoft.com")
    << String(
    "{\"accessToken\":\"dummy_GSCkWgHncpw\",\"accessTokenType\":\"Bearer\",\"expiresOn\":1430416203,\"idToken\":\"eyJ0eXAiOiJ-0c3B4In0.\",\"isMultipleResourceRefreshToken\":true,\"refreshToken\":\"dummy-AAABAAAAiL9Kn2Z27UqYPmQuP2yAA\",\"resource\":\"https://shch.onmicrosoft.com/TodoListService\",\"tenantId_\":\"b0570dfb-b143-4f35-b385-ed9796b2137a\",\"userInfo\":{\"displayableId\":\"qqq@shch.onmicrosoft.com\",\"familyName\":\"Q2\",\"forcePrompt\":true,\"givenName\":\"Q1\",\"identityProvider\":\"https://sts.windows.net/b0570dfb-b143-4f35-b385-ed9796b2137a/\",\"passwordChangeUrl\":\"https://portal.microsoftonline.com/ChangePassword.aspx\",\"passwordExpiresOn\":1432773087,\"uniqueId\":\"90965c9c-9a6c-443c-8245-4d80676274fe\"}}")
  ;
}

void NonInteractiveTests::CacheTest()
{
  QFETCH(String, clientId);
  QFETCH(String, redirectUri);
  QFETCH(String, resource);
  QFETCH(String, authority);
  QFETCH(String, authenticationResult);

  qDebug() <<  "====== NonInteractiveTests::CacheTest() ======";

  String cacheFilePath     = String(SRCDIR) + "cache.dat";
  ptr<TokenCache> cachePtr = make_shared<FileCache>(cacheFilePath);
  auto authResPtr          = AuthenticationResult::deserialize(
    authenticationResult);
  cachePtr->storeToCache(authResPtr,
                         authority,
                         resource,
                         clientId,
                         TokenSubjectType::User,
                         nullptr);
  cachePtr->onAfterAccess(TokenCacheNotificationArgs(nullptr));

  auto userInfoPtr = authResPtr->userInfo();

  String uId = userInfoPtr == nullptr ? "" : userInfoPtr->uniqueId();
  String dId = userInfoPtr == nullptr ? "" : userInfoPtr->displayableId();

  cachePtr->onBeforeAccess(TokenCacheNotificationArgs(nullptr));
  auto resultPtr = cachePtr->loadFromCache(authority,
                                           resource,
                                           clientId,
                                           TokenSubjectType::User,
                                           uId,

                                           /*dId, */ nullptr);
  QVERIFY(resultPtr != nullptr);
  QVERIFY(resultPtr->expiresOn() == authResPtr->expiresOn());
}

void NonInteractiveTests::FileCacheEncryptedTest_data()
{
  QTest::addColumn<String>("clientId");
  QTest::addColumn<String>("redirectUri");
  QTest::addColumn<String>("resource");
  QTest::addColumn<String>("authority");
  QTest::addColumn<String>("authenticationResult");

  QTest::newRow("auth1")
    << String("dummy-bd86-40a0-87ce-7c49cbab402d")
    << String("https://auth1.app")
    << String("api.auth1.rest.com")
    << String("https://auth1/oauth2/authorize")
    << String(
    "{\"accessToken\":\"asifthisisatokeneroPdddSlEzvINRV7MSw\",\"accessTokenType\":\"bearer\",\"expiresOn\":1430427072,\"idToken\":\"\",\"isMultipleResourceRefreshToken\":false,\"refreshToken\":\"asifthisisatokneroPdddSlEzvINRV7MSw-oLbp-PCYAfA\",\"resource\":\"\",\"tenantId_\":\"\"}")
  ;

  QTest::newRow("auth2")
    << String("dummy-cfa1-4ac6-bd2e-0d046cf1c3f7")
    << String("https://auth2.app")
    << String("api.auth2.com")
    << String("https://auth2/oauth/authorize")
    << String(
    "{\"accessToken\":\"asifthisisatokeneroPdddSlEzvINRV7MSw.hsA\",\"accessTokenType\":\"jwt\",\"expiresOn\":1430449424,\"idToken\":\"\",\"isMultipleResourceRefreshToken\":false,\"refreshToken\":\"asifthisisatokneroPdddSlEzvINRV7MSw976\",\"resource\":\"\",\"tenantId_\":\"\"}")
  ;

  QTest::newRow("auth3")
    << String("dummy-61b4-4235-bbc4-ca382dd3d52e")
    << String("http://auth3")
    << String("https://auth3.onmicrosoft.com/auth3")
    << String("https://login.windows.net/auth3.onmicrosoft.com")
    << String(
    "{\"accessToken\":\"dummy-asifthisisatoken.\",\"accessTokenType\":\"Bearer\",\"expiresOn\":1430416203,\"idToken\":\"asifthisisatoken\",\"isMultipleResourceRefreshToken\":true,\"refreshToken\":\"asifthisisatoken\",\"resource\":\"https://shch.onmicrosoft.com/TodoListService\",\"tenantId_\":\"dummy--b143-4f35-b385-ed9796b2137a\",\"userInfo\":{\"displayableId\":\"qqq@shch.onmicrosoft.com\",\"familyName\":\"Q2\",\"forcePrompt\":true,\"givenName\":\"Q1\",\"identityProvider\":\"https://sts.windows.net/b0570dfb-b143-4f35-b385-ed9796b2137a/\",\"passwordChangeUrl\":\"https://portal.microsoftonline.com/ChangePassword.aspx\",\"passwordExpiresOn\":1432773087,\"uniqueId\":\"dummy-9a6c-443c-8245-4d80676274fe\"}}")
  ;
}

void NonInteractiveTests::FileCacheEncryptedTest()
{
  QFETCH(String, clientId);
  QFETCH(String, redirectUri);
  QFETCH(String, resource);
  QFETCH(String, authority);
  QFETCH(String, authenticationResult);

  qDebug() << "====== NonInteractiveTests::FileCacheEncryptedTest() ======";

  try
  {
    String cacheFilePath     = String(SRCDIR) + "cacheEncrypted.dat";
    ptr<TokenCache> cachePtr = make_shared<FileCacheEncrypted>(cacheFilePath);
    auto authResPtr          = AuthenticationResult::deserialize(
      authenticationResult);
    cachePtr->storeToCache(authResPtr,
                           authority,
                           resource,
                           clientId,
                           TokenSubjectType::User,
                           nullptr);
    cachePtr->onAfterAccess(TokenCacheNotificationArgs(nullptr));

    auto userInfoPtr = authResPtr->userInfo();

    String uId = userInfoPtr == nullptr ? "" : userInfoPtr->uniqueId();
    String dId = userInfoPtr == nullptr ? "" : userInfoPtr->displayableId();

    cachePtr->onBeforeAccess(TokenCacheNotificationArgs(nullptr));
    auto resultPtr = cachePtr->loadFromCache(authority,
                                             resource,
                                             clientId,
                                             TokenSubjectType::User,
                                             uId,

                                             /*dId, */ nullptr);
    QVERIFY(resultPtr != nullptr);
    QVERIFY(resultPtr->expiresOn() == authResPtr->expiresOn());
  }
  catch (const std::exception& e)
  {
    QTest::qFail(e.what(), __FILE__, __LINE__);
  }
}

void NonInteractiveTests::AcquireTokenNonInteractiveHandlerTest()
{
  qDebug() <<
    "====== NonInteractiveTests::AcquireTokenNonInteractiveHandlerTest() ======";

  const String Tag("AcquireTokenNonInteractiveHandlerTest");

  ptr<TokenCache> cachePtr = nullptr;
  AuthenticationContext authContext(
    authority_.toStdString(), AuthorityValidationType::False,
    cachePtr);
  AuthenticationResultPtr result = nullptr;

  try
  {
    auto userCredential = std::make_shared<UserCredential>(userName_.toStdString(),
                                                           password_.toStdString());
    result = authContext.acquireToken(
      resource_.toStdString(), clientId_.toStdString(), userCredential);
    qDebug() << Tag.c_str() << "Access token: " << result->accessToken().c_str();
    QVERIFY(result != nullptr);
  }
  catch (const Exception& ex)
  {
    QTest::qFail(ex.what(), __FILE__, __LINE__);
  }
}

void NonInteractiveTests::AcquireTokenForClientHandlerTest()
{
  qDebug() <<
    "====== NonInteractiveTests::AcquireTokenForClientHandlerTest() ======";

  const String Tag("AcquireTokenForClientHandlerTest");

  AuthenticationContext authContext(
    authority_.toStdString(), AuthorityValidationType::False,
    nullptr);
  AuthenticationResultPtr result = nullptr;

  try
  {
    auto clientCredential = std::make_shared<ClientCredential>(
      clientId_.toStdString(), clientSecret_.toStdString());

    result = authContext.acquireToken(resource_.toStdString(), clientCredential);
    qDebug() << Tag.c_str() << "Access token: " << result->accessToken().c_str();
    QVERIFY(result != nullptr);
  }
  catch (const Exception& ex)
  {
    QTest::qFail(ex.what(), __FILE__, __LINE__);
  }
}
