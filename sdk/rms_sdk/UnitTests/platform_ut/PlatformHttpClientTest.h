/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PLATFORMHTTPCLIENTTEST
#define PLATFORMHTTPCLIENTTEST
#include <QtTest>

class PlatformHttpClientTest : public QObject
{
    Q_OBJECT
public:
    PlatformHttpClientTest();

private Q_SLOTS:
    void testHttpClient(bool enabled = true);
};
#endif // PLATFORMHTTPCLIENTTEST

