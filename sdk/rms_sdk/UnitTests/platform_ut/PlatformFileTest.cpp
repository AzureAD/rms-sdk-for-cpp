/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PlatformFileTest.h"
#include "../../Platform/Logger/Logger.h"
#include "../../Platform/Filesystem/IFile.h"
#include "../../Common/CommonTypes.h"
#include <QFile>
#include <sstream>

using namespace std;
using namespace rmscore::platform::filesystem;
using namespace rmscore::common;

void PlatformFileTest::testRead()
{
    QString path1 = QString(SRCDIR) + "data/tmpread.txt";
    QFile tmp(path1);
    tmp.open(QIODevice::WriteOnly | QIODevice::Text);
    QDataStream out(&tmp);

    std::string outdata = "This is test data stream !#$%";
    auto wn = out.writeRawData(outdata.data(), (int)outdata.length());
    tmp.close();

    auto pFile = IFile::Create(path1.toStdString(), FileOpenModes::FILE_OPEN_READ);
    QVERIFY(pFile!=nullptr);

    CharArray bytes;
    auto rn = pFile->Read(bytes, wn);
    pFile->Close();
    auto ok = QFile::remove(path1);
    QVERIFY(ok);

    std::string indata(bytes.data(), rn);

    QCOMPARE(outdata, indata);
}
void PlatformFileTest::testWrite()
{
    QString path1 = QString(SRCDIR) + "data/tmpwrite.txt";
    auto pFile = IFile::Create(path1.toStdString(), FileOpenModes::FILE_OPEN_WRITE);
    QVERIFY(pFile!=nullptr);

    std::string outdata = "This is test data stream !#$%";
    auto wn = pFile->Write(outdata.data(), outdata.length());
    pFile->Close();

    QFile tmp(path1);
    tmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QDataStream in(&tmp);
    CharArray bytes(wn);
    auto rn = in.readRawData(bytes.data(), (int)wn);

    tmp.close();
    auto ok = QFile::remove(path1);
    QVERIFY(ok);

    std::string indata(bytes.data(), rn);
    QCOMPARE(outdata, indata);
}
void PlatformFileTest::testClear()
{
    QString path1 = QString(SRCDIR) + "data/tmpclear.txt";
    auto pFile = IFile::Create(path1.toStdString(), FileOpenModes::FILE_OPEN_WRITE);
    QVERIFY(pFile!=nullptr);

    std::string outdata = "This is test data stream !#$%";
    auto wn = pFile->Write(outdata.data(), outdata.length());
    auto size = pFile->GetSize();
    QVERIFY(size==wn);

    pFile->Clear();
    auto size1 = pFile->GetSize();
    QVERIFY(size1==0);
    pFile->Close();
    auto ok = QFile::remove(path1);
    QVERIFY(ok);
}
void PlatformFileTest::testReadAllAsText()
{
    QString path1 = QString(SRCDIR) + "data/testreadall.txt";
    auto pFile = IFile::Create(path1.toStdString(), FileOpenModes::FILE_OPEN_READ);
    QVERIFY(pFile!=nullptr);
    auto text = pFile->ReadAllAsText();
    pFile->Close();
    QFile tmp(path1);
    tmp.open(QIODevice::ReadOnly| QIODevice::Text);
    auto expectedText = string(tmp.readAll());
    QCOMPARE(text, expectedText);
    tmp.close();
}
void PlatformFileTest::testAppendText()
{
    QString path1 = QString(SRCDIR) + "data/tmpappend.txt";
    auto pFile = IFile::Create(path1.toStdString(), FileOpenModes::FILE_OPEN_WRITE);
    QVERIFY(pFile!=nullptr);


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
    QFile tmp(path1);
    tmp.open(QIODevice::ReadOnly | QIODevice::Text);
    auto expectedText = string(tmp.readAll());
    tmp.close();
    QCOMPARE(text, expectedText);
    auto ok = QFile::remove(path1);
    QVERIFY(ok);
}
void PlatformFileTest::testGetSize()
{
    QString path1 = QString(SRCDIR) + "data/testsize.h";
    auto pFile = IFile::Create(path1.toStdString(), FileOpenModes::FILE_OPEN_READ);
    QVERIFY(pFile!=nullptr);
    auto size = pFile->GetSize();
    QFile qFile(path1);

    auto expectedSize = (size_t)qFile.size();
    QCOMPARE(size, expectedSize);
}


