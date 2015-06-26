/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef INTERACTIVETEST_H
#define INTERACTIVETEST_H
#include <QTest>

class InteractiveTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void AcquireTokenInteractiveHandlerTest_data();
    void AcquireTokenInteractiveHandlerTest();
};

#endif // INTERACTIVETEST_H
