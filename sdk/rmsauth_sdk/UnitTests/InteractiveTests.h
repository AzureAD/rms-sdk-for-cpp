/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef INTERACRIVETEST_H
#define INTERACRIVETEST_H
#include <QTest>

class InteractiveTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void AcquireTokenInteractiveHandlerTest_data();
    void AcquireTokenInteractiveHandlerTest();
};

#endif // INTERACRIVETEST_H
