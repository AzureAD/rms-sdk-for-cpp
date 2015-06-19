/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PLATFORMJSONARRAYTEST
#define PLATFORMJSONARRAYTEST
#include <QtTest>

class PlatformJsonArrayTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testGetStringAt();
    void testGetStringAt_data();

//    void testGetObjectAt();
//    void testGetObjectAt_data();

    void testAppendObject();

    void testAppendString();
};
#endif // PLATFORMJSONARRAYTEST

