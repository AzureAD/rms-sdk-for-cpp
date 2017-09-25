/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "../../Platform/Filesystem/IFile.h"

#include <sstream>
#include <fstream>

#include "../../Platform/Logger/Logger.h"
#include "../../Common/CommonTypes.h"
#include "gtest/gtest.h"


using namespace std;
using namespace rmscore::platform::filesystem;
using namespace rmscore::common;
using std::string;

TEST (PlatformFileTest, testRead) {
    string path1 = string(SRCDIR) + "data/tmpread.txt";
    std::ofstream out(path1, std::ofstream::out);

    std::string outdata = "This is test data stream !#$%";
    out.write(outdata.c_str(), outdata.length());
    auto wn = out.tellp();
    out.close();

    auto pFile = IFile::Create(path1, FileOpenModes::FILE_OPEN_READ);
    EXPECT_NE(pFile, nullptr);

    CharArray bytes;
    auto rn = pFile->Read(bytes, wn);
    pFile->Close();
    auto ok = std::remove(path1.c_str());
    EXPECT_EQ(ok, 0);

    std::string indata(bytes.data(), rn);

    EXPECT_EQ(outdata, indata);
}

TEST (PlatformFileTest, testWrite) {
    string path1 = string(SRCDIR) + "data/tmpwrite.txt";
    auto pFile = IFile::Create(path1, FileOpenModes::FILE_OPEN_WRITE);
    EXPECT_NE(pFile, nullptr);

    std::string outdata = "This is test data stream !#$%";
    auto wn = pFile->Write(outdata.data(), outdata.length());
    pFile->Close();

    std::ifstream in(path1, std::ifstream::in);
    CharArray bytes(wn);
    in.read(bytes.data(), (int)wn);
    auto rn = in.gcount();

    in.close();
    auto ok = std::remove(path1.c_str());
    EXPECT_EQ(ok, 0);

    std::string indata(bytes.data(), rn);
    EXPECT_EQ(outdata, indata);
}

TEST (PlatformFileTest, testClear) {
    string path1 = string(SRCDIR) + "data/tmpclear.txt";
    auto pFile = IFile::Create(path1, FileOpenModes::FILE_OPEN_WRITE);
    EXPECT_NE(pFile, nullptr);

    std::string outdata = "This is test data stream !#$%";
    auto wn = pFile->Write(outdata.data(), outdata.length());
    auto size = pFile->GetSize();
    EXPECT_EQ(size, wn);

    pFile->Clear();
    auto size1 = pFile->GetSize();
    EXPECT_EQ(size1, 0);
    pFile->Close();
    auto ok = std::remove(path1.c_str());
    EXPECT_EQ(ok, 0);
}

TEST (PlatformFileTest, testReadAllAsText)
{
    string path1 = string(SRCDIR) + "data/testreadall.txt";
    auto pFile = IFile::Create(path1, FileOpenModes::FILE_OPEN_READ);
    EXPECT_NE(pFile, nullptr);
    auto text = pFile->ReadAllAsText();
    pFile->Close();
    std::ifstream in(path1, std::ifstream::in);
    stringstream ss;
    ss << in.rdbuf();
    string expectedText = ss.str();
    EXPECT_EQ(text, expectedText);
    in.close();
}

TEST (PlatformFileTest, testAppendText) {
    string path1 = string(SRCDIR) + "data/tmpappend.txt";
    auto pFile = IFile::Create(path1, FileOpenModes::FILE_OPEN_WRITE);
    EXPECT_NE(pFile, nullptr);


    std::string text;
    std::stringstream ss;

    ss << "line 1" << std::endl << "line 2\n" << "line 3" << std::endl;
    text += ss.str();
    auto str = ss.str();
    pFile->AppendText(std::vector<uint8_t>(str.begin(), str.end()));
    ss.str("");

    ss << "line 4" << std::endl << "\tline 5" << std::endl << "line 6" << std::endl;
    text += ss.str();
    str = ss.str();
    pFile->AppendText(std::vector<uint8_t>(str.begin(), str.end()));
    ss.str("");

    ss << "line 7" << std::endl << "line 8" << std::endl << "line 9" << std::endl;
    text += ss.str();
    str = ss.str();
    pFile->AppendText(std::vector<uint8_t>(str.begin(), str.end()));
    ss.str("");

    pFile->Close();
    std::ifstream in(path1, std::ifstream::in);
    stringstream ss1;
    ss1 << in.rdbuf();
    string expectedText = ss1.str();
    in.close();
    EXPECT_EQ(text, expectedText);
    auto ok = std::remove(path1.c_str());
    EXPECT_EQ(ok, 0);
}

TEST (PlatformFileTest, testGetSize) {
    string path1 = string(SRCDIR) + "data/testsize.h";
    auto pFile = IFile::Create(path1, FileOpenModes::FILE_OPEN_READ);
    EXPECT_NE(pFile, nullptr);
    auto size = pFile->GetSize();
    ifstream file(path1.c_str(), ifstream::in | ifstream::binary);
    
    EXPECT_TRUE(file.is_open());

    file.seekg(0, ios::end);
    auto expectedSize = file.tellg();
    file.close();
    EXPECT_EQ(size, expectedSize);
}


