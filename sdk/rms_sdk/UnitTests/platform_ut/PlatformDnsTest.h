/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PLATFORMDNSTEST
#define PLATFORMDNSTEST
#include <QtTest>

class PlatformDnsTest : public QObject
{
    Q_OBJECT
public:
    PlatformDnsTest();

private Q_SLOTS:
    void testDns(bool enabled = true);
};
#endif // PLATFORMDNSTEST

