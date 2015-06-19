/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PLATFORMFILETEST_H
#define PLATFORMFILETEST_H
#include <QtTest>

class PlatformFileTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRead();
    void testWrite();
    void testClear();
    void testReadAllAsText();
    void testAppendText();
    void testGetSize();
};

#endif // PLATFORMFILETEST_H
