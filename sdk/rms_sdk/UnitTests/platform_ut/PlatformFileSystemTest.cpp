/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/
#include "../../Platform/Filesystem/IFileSystem.h"

#include <fstream>

#include "../../Common/CommonTypes.h"
#include "gtest/gtest.h"

using namespace rmscore::platform::filesystem;
using std::string;

TEST(PlatformFileSystemTest, testOpenLocalStorageFile) {   
    string path = string(SRCDIR) + "data/doesnotexist.no";
    auto fs = IFileSystem::Create();
    auto pf = fs->OpenLocalStorageFile(path, FileOpenModes::FILE_OPEN_READ);
    EXPECT_EQ(pf, nullptr);
  
    string path1 = string(SRCDIR) + "data/testRead.txt";
    auto pf1 = fs->OpenLocalStorageFile(path1, FileOpenModes::FILE_OPEN_READ);
    EXPECT_NE(pf1, nullptr);
}

TEST(PlatformFileSystemTest, testDeleteLocalStorageFile) {
    auto fs = IFileSystem::Create();
    string path1 = string(SRCDIR) + "data/tmpdelete.txt";
    auto pf1 = fs->OpenLocalStorageFile(path1, FileOpenModes::FILE_OPEN_WRITE);
    EXPECT_NE(pf1, nullptr);
    pf1->Close();
    fs->DeleteLocalStorageFile(path1);
    std::ifstream infile(path1);
    EXPECT_FALSE(infile.good());
    infile.close();
}

TEST (PlatformFileSystemTest, testQueryLocalStorageFiles) {
    string path1 = string(SRCDIR) + "data/";
    auto pfs = IFileSystem::Create();
    auto l = pfs->QueryLocalStorageFiles(path1, "log_1213*.log");
    EXPECT_EQ(l.size(), 1);
    EXPECT_EQ(l[0], std::string("log_121328-3103.log"));
}
