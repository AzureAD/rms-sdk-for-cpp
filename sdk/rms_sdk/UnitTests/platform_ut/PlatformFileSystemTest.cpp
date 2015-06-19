/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PlatformFileSystemTest.h"
#include "../../Platform/Filesystem/IFileSystem.h"
#include "../../Common/CommonTypes.h"
#include <QDir>

using namespace rmscore::platform::filesystem;

void PlatformFileSystemTest::testOpenLocalStorageFile()
{
    QString path = QString(SRCDIR) + "data/doesnotexist.no";
    auto fs = IFileSystem::Create();
    auto pf = fs->OpenLocalStorageFile(path.toStdString(), FileOpenModes::FILE_OPEN_READ);
    QVERIFY(pf == nullptr);

    QString path1 = QString(SRCDIR) + "data/testRead.txt";
    auto pf1 = fs->OpenLocalStorageFile(path1.toStdString(), FileOpenModes::FILE_OPEN_READ);
    QVERIFY(pf1 != nullptr);
}
void PlatformFileSystemTest::testDeleteLocalStorageFile()
{
    auto fs = IFileSystem::Create();
    QString path1 = QString(SRCDIR) + "data/tmpdelete.txt";
    auto pf1 = fs->OpenLocalStorageFile(path1.toStdString(), FileOpenModes::FILE_OPEN_WRITE);
    QVERIFY(pf1 != nullptr);
    pf1->Close();
    fs->DeleteLocalStorageFile(path1.toStdString());
    QDir dir;
    QVERIFY(dir.exists(path1)==false);
}
void PlatformFileSystemTest::testQueryLocalStorageFiles()
{
    QString path1 = QString(SRCDIR) + "data/";
    auto pfs = IFileSystem::Create();
    auto l = pfs->QueryLocalStorageFiles(path1.toStdString(), "log_1213*.log");
    QVERIFY(l.size() == 1);
    QVERIFY(l[0] == std::string("log_121328-3103.log"));
}
