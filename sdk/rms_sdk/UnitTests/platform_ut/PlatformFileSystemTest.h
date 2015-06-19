/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef PLATFORMFILESYSTEMTEST_H
#define PLATFORMFILESYSTEMTEST_H
#include <QtTest>

class PlatformFileSystemTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testOpenLocalStorageFile();
    void testDeleteLocalStorageFile();
    void testQueryLocalStorageFiles();
};

#endif // PLATFORMFILESYSTEMTEST_H
