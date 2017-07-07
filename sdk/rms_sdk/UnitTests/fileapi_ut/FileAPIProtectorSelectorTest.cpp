/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "FileAPIProtectorSelectorTest.h"
#include "../../FileAPI/ProtectorSelector.h"

FileAPIProtectorSelectorTest::FileAPIProtectorSelectorTest()
{
}

void FileAPIProtectorSelectorTest::testProtectorSelector()
{
    {   // Test Opc protection/unprotection
        std::string fileName = "temp.docx";
        rmscore::fileapi::ProtectorSelector selector(fileName);
        auto pType = selector.GetProtectorType();
        QVERIFY2(pType == rmscore::fileapi::ProtectorType::OPC, "Incorrect protector type"
                                                                "computed");
        auto outputFileName = selector.GetOutputFileName();
        QVERIFY2(outputFileName == fileName, "Incorrect output file name computed");
    }
    {   // Test Pfile protection
        std::string fileName = "temp.random";
        rmscore::fileapi::ProtectorSelector selector(fileName);
        auto pType = selector.GetProtectorType();
        QVERIFY2(pType == rmscore::fileapi::ProtectorType::PFILE, "Incorrect protector type"
                                                                  "computed");
        auto outputFileName = selector.GetOutputFileName();
        QVERIFY2(outputFileName == "temp.random.pfile", "Incorrect output file name computed");
    }
    {   // Test PStar protection
        std::string fileName = "temp.txt";
        rmscore::fileapi::ProtectorSelector selector(fileName);
        auto pType = selector.GetProtectorType();
        QVERIFY2(pType == rmscore::fileapi::ProtectorType::PFILE, "Incorrect protector type"
                                                                  "computed");
        auto outputFileName = selector.GetOutputFileName();
        QVERIFY2(outputFileName == "temp.ptxt", "Incorrect output file name computed");
    }
    {   // Test Pfile unprotection
        std::string fileName = "temp.random.pfile";
        rmscore::fileapi::ProtectorSelector selector(fileName);
        auto pType = selector.GetProtectorType();
        QVERIFY2(pType == rmscore::fileapi::ProtectorType::PFILE, "Incorrect protector type"
                                                                  "computed");
        auto outputFileName = selector.GetOutputFileName();
        QVERIFY2(outputFileName == "temp.random", "Incorrect output file name computed");
    }
    {   // Test PStar unprotection
        std::string fileName = "temp.ptxt";
        rmscore::fileapi::ProtectorSelector selector(fileName);
        auto pType = selector.GetProtectorType();
        QVERIFY2(pType == rmscore::fileapi::ProtectorType::PFILE, "Incorrect protector type"
                                                                  "computed");
        auto outputFileName = selector.GetOutputFileName();
        QVERIFY2(outputFileName == "temp.txt", "Incorrect output file name computed");
    }
}
