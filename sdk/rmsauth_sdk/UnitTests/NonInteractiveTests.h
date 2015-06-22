/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef NONINTERACTIVETEST_H
#define NONINTERACTIVETEST_H
#include <QTest>

class NonInteractiveTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void AuthenticationResultTest_data();
    void AuthenticationResultTest();
    void CacheTest_data();
    void CacheTest();
    void FileCacheEncryptedTest_data();
    void FileCacheEncryptedTest();
//    void AcquireTokenNonInteractiveHandlerTest_data();
//    void AcquireTokenNonInteractiveHandlerTest();
//    void AcquireTokenForClientHandlerTest_data();
//    void AcquireTokenForClientHandlerTest();

};

#endif // NONINTERACTIVETEST_H
