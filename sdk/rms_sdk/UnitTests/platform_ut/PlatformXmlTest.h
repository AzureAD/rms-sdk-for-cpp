/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PLATFORMXMLTEST
#define PLATFORMXMLTEST
#include <QtTest>

class PlatformXmlTest : public QObject
{
    Q_OBJECT
public:
    PlatformXmlTest();

private Q_SLOTS:
    void testSelectSingleNode(bool enabled = true);
    void testSelectSingleNode_data();
};
#endif // PLATFORMXMLTEST

