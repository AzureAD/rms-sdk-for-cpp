/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_UNIT_TESTS_FILEAPI_UT_FILEAPIPROTECTORSELECTORTEST_H
#define RMS_SDK_UNIT_TESTS_FILEAPI_UT_FILEAPIPROTECTORSELECTORTEST_H

#include <QtTest>

class FileAPIProtectorSelectorTest : public QObject
{
    Q_OBJECT
public:
    FileAPIProtectorSelectorTest();

private Q_SLOTS:
    void testProtectorSelector();
};

#endif // RMS_SDK_UNIT_TESTS_FILEAPI_UT_FILEAPIPROTECTORSELECTORTEST_H
