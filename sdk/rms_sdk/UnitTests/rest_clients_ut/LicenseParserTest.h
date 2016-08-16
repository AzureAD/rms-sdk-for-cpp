/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef LICENSEPARSERTEST_H
#define LICENSEPARSERTEST_H
#include <QtTest>

namespace UnitTests {
namespace RestClientsUT {
class LicenseParserTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test_UTF8_License();
    void test_UTF16LE_License();
};
} //UnitTests
} //RestClientsUT
#endif // LICENSEPARSERTEST_H_
