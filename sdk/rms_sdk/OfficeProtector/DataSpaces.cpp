/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <DataSpaces.h>
#include "../Platform/Logger/Logger.h"
#include "../ModernAPI/RMSExceptions.h"
#include "Utils.h"

using namespace rmscore::officeprotector;
using namespace rmscore::pole;
using namespace rmscore::platform::logger;
using namespace rmscore::common;

namespace rmscore {
namespace officeprotector {

DataSpaces::DataSpaces(bool isMetro)
{
    this->isMetro = isMetro;
}

DataSpaces::~DataSpaces()
{
}

//todo add logging here
void DataSpaces::WriteDataspaces(Storage *stg, ByteArray publishingLicense)
{
    if(stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing dataspaces.");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to storage",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    Logger::Hidden("Deleting DataSpaces if exists");
    std::string rootEntry = "/";
    std::string dataSpaceStgName = rootEntry + dataSpace;
    stg->deleteByName(dataSpaceStgName);

    std::string versionStmName = dataSpaceStgName + "/" + version;
    Logger::Hidden("Writing Version in " + versionStmName);
    Stream* versionStm = new Stream(stg, versionStmName, true);
    versionStm->seek(0);
    WriteVersion(versionStm, versionFeature);
    versionStm->flush();

    std::string dataSpaceMapStmName = dataSpaceStgName + "/" + dataSpaceMap;
    Logger::Hidden("Writing DataSpaceMap in " + dataSpaceMapStmName);
    Stream* dataSpaceMapStm = new Stream(stg, dataSpaceMapStmName, true);
    dataSpaceMapStm->seek(0);
    WriteDataSpaceMap(dataSpaceMapStm);
    dataSpaceMapStm->flush();

    std::string dataSpaceInfoStgName = dataSpaceStgName + "/" + dataSpaceInfo;
    std::string drmDataSpaceStmName = dataSpaceInfoStgName + "/" + (isMetro? metroDataSpace : drmDataSpace);
    Logger::Hidden("Writing DRMDataSpace in " + drmDataSpaceStmName);
    Stream* drmDataSpaceStm = new Stream(stg, drmDataSpaceStmName, true);
    drmDataSpaceStm->seek(0);
    WriteDRMDataSpace(drmDataSpaceStm);
    drmDataSpaceStm->flush();

    std::string transformInfoStgName = dataSpaceStgName + "/" + transformInfo;
    std::string drmTransformStgName = transformInfoStgName + "/" + (isMetro? metroTransform : drmTransform);
    std::string primaryStmName = drmTransformStgName + "/" + primary;
    Logger::Hidden("Writing Primary in " + primaryStmName);
    Stream* primaryStm = new Stream(stg, primaryStmName, true);
    primaryStm->seek(0);
    WritePrimary(primaryStm, publishingLicense);
    primaryStm->flush();

    delete versionStm;
    delete dataSpaceMapStm;
    delete drmDataSpaceStm;
    delete primaryStm;
}

void DataSpaces::ReadDataspaces(Storage *stg, ByteArray &publishingLicense)
{
    if(stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading dataspaces.");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from storage",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }
    std::string rootEntry = "/";
    std::string dataSpaceStgName = rootEntry + dataSpace;
    std::string transformInfoStgName = dataSpaceStgName + "/" + transformInfo;
    std::string passwordTransformStgName = transformInfoStgName + "/" + passwordTransform;

    if(stg->exists(passwordTransformStgName))
    {
        Logger::Error("The file has been protected using non RMS technologies");
        exceptions::RMSMetroOfficeFileException("The file has been protected using non RMS technologies",
                                                exceptions::RMSMetroOfficeFileException::NonRMSProtected);
    }

    std::string primaryStmName = transformInfoStgName + "/" + (isMetro? metroTransform : drmTransform)
            + "/" + primary;

    if(!stg->exists(primaryStmName))
    {
        Logger::Error("The primary stream doesn't exist.");
        exceptions::RMSMetroOfficeFileException("The file has been corrupted",
                                                exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    Stream *stm = new Stream(stg, primaryStmName, false);
    stm->seek(0);
    ReadPrimary(stm, publishingLicense);
}

void DataSpaces::WriteVersion(Stream *stm, std::string content)
{
    if( stm == nullptr || content.empty())
    {
        Logger::Error("Invalid arguments provided for writing version");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }
    uint16_t readerMajor  = 1;
    uint16_t readerMinor  = 0;
    uint16_t updaterMajor = 1;
    uint16_t updaterMinor = 0;
    uint16_t writerMajor  = 1;
    uint16_t writerMinor  = 0;

    WriteWideStringEntry(stm, content);
    stm->write(reinterpret_cast<unsigned char*>(&readerMajor), sizeof(uint16_t));
    stm->write(reinterpret_cast<unsigned char*>(&readerMinor), sizeof(uint16_t));
    stm->write(reinterpret_cast<unsigned char*>(&updaterMajor), sizeof(uint16_t));
    stm->write(reinterpret_cast<unsigned char*>(&updaterMinor), sizeof(uint16_t));
    stm->write(reinterpret_cast<unsigned char*>(&writerMajor), sizeof(uint16_t));
    stm->write(reinterpret_cast<unsigned char*>(&writerMinor), sizeof(uint16_t));
}

void DataSpaces::ReadVersion(Stream *stm, std::string contentExpected)
{
    if( stm == nullptr || contentExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading version");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint16_t readerMajorExpected  = 1;
    uint16_t readerMinorExpected  = 0;
    uint16_t updaterMajorExpected = 1;
    uint16_t updaterMinorExpected = 0;
    uint16_t writerMajorExpected  = 1;
    uint16_t writerMinorExpected  = 0;

    std::string contentRead;
    uint16_t readerMajorRead  = 0;
    uint16_t readerMinorRead  = 0;
    uint16_t updaterMajorRead = 0;
    uint16_t updaterMinorRead = 0;
    uint16_t writerMajorRead  = 0;
    uint16_t writerMinorRead  = 0;

    ReadWideStringEntry(stm, contentRead);
    stm->read(reinterpret_cast<unsigned char*>(&readerMajorRead), sizeof(uint16_t));
    stm->read(reinterpret_cast<unsigned char*>(&readerMinorRead), sizeof(uint16_t));
    stm->read(reinterpret_cast<unsigned char*>(&updaterMajorRead), sizeof(uint16_t));
    stm->read(reinterpret_cast<unsigned char*>(&updaterMinorRead), sizeof(uint16_t));
    stm->read(reinterpret_cast<unsigned char*>(&writerMajorRead), sizeof(uint16_t));
    stm->read(reinterpret_cast<unsigned char*>(&writerMinorRead), sizeof(uint16_t));

    if( contentRead.compare(contentExpected) != 0 || readerMajorRead != readerMajorExpected || readerMinorRead != readerMinorExpected
            || updaterMajorRead != updaterMajorExpected || updaterMinorRead != updaterMinorExpected
            || writerMajorRead != writerMajorExpected || writerMinorRead != writerMinorExpected)
    {
        Logger::Error("Version mismatch", contentRead, readerMajorRead, readerMinorRead, updaterMajorRead,
                      updaterMinorRead, writerMajorRead, writerMinorRead);
        throw exceptions::RMSMetroOfficeFileException("The file has been corrupted",
                                                      exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
}

void DataSpaces::WriteDataSpaceMap(Stream *stm)
{
    DataSpaceMapHeader dsmh;
    DataSpaceMapEntryHeader dsmeh;
    uint32_t refVar = 0;

    if( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing DataSpaceMap");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    dsmh.headerLen = sizeof(dsmh);
    dsmh.entryCount = 1;
    stm->write(reinterpret_cast<unsigned char*>(&dsmh.headerLen), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&dsmh.entryCount), sizeof(uint32_t));

    dsmeh.entryLen = sizeof(dsmeh) + sizeof(uint32_t) + (isMetro ? WideStringEntryLength(metroContent)
                                                                   + WideStringEntryLength(metroDataSpace) :
                                                                   WideStringEntryLength(drmContent)
                                                                   + WideStringEntryLength(drmDataSpace));
    dsmeh.componentCount = 1;

    stm->write(reinterpret_cast<unsigned char*>(&dsmeh.entryLen), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&dsmeh.componentCount), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&refVar), sizeof(uint32_t));
    WriteWideStringEntry(stm, isMetro ? metroContent : drmContent);
    WriteWideStringEntry(stm, isMetro ? metroDataSpace : drmDataSpace);
}

void DataSpaces::WriteDRMDataSpace(Stream *stm)
{
    if( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing DRMDataSpace");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformHeader dth;
    dth.headerLen = sizeof(dth);
    dth.txCount = 1;

    stm->write(reinterpret_cast<unsigned char*>(&dth.headerLen), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&dth.txCount), sizeof(uint32_t));
    WriteWideStringEntry(stm, isMetro? metroTransform : drmTransform);
}

void DataSpaces::WriteTxInfo(Stream *stm, std::string txClassName, std::string featureName)
{
    if( stm == nullptr || txClassName.empty() || featureName.empty())
    {
        Logger::Error("Invalid arguments provided for writing Transform Info");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformInfo dti;
    dti.headerLen = sizeof(dti) + WideStringEntryLength(txClassName);
    dti.txClassType = 1;

    stm->write(reinterpret_cast<unsigned char*>(&dti.headerLen), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&dti.txClassType), sizeof(uint32_t));
    WriteWideStringEntry(stm, txClassName);
    WriteVersion(stm, featureName);

}

void DataSpaces::ReadTxInfo(Stream *stm, std::string txClassNameExpected, std::string featureNameExpected)
{
    if( stm == nullptr || txClassNameExpected.empty() || featureNameExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading Transform Info");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformInfo dtiExpected;
    dtiExpected.headerLen = sizeof(dtiExpected) + WideStringEntryLength(txClassNameExpected);
    dtiExpected.txClassType = 1;

    DRMTransformInfo dtiRead;
    stm->read(reinterpret_cast<unsigned char*>(&dtiRead.headerLen), sizeof(uint32_t));
    stm->read(reinterpret_cast<unsigned char*>(&dtiRead.txClassType), sizeof(uint32_t));

    if(dtiRead.headerLen != dtiExpected.headerLen || dtiRead.txClassType != dtiExpected.txClassType)
    {
        Logger::Error("DRMTransformInfo mismatch",
                      std::to_string(dtiRead.headerLen), std::to_string(dtiRead.txClassType));
        throw exceptions::RMSMetroOfficeFileException("The file has been corrupted",
                                                      exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    std::string txClassNameRead;
    ReadWideStringEntry(stm, txClassNameRead);
    if(txClassNameRead.compare(txClassNameExpected) != 0)
    {
        Logger::Error("Transform Class mismatch", txClassNameRead);
        throw exceptions::RMSMetroOfficeFileException("The file has been corrupted",
                                                      exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    ReadVersion(stm, featureNameExpected);

}

void DataSpaces::WritePrimary(Stream *stm, ByteArray publishingLicense)
{
    if(stm == nullptr || publishingLicense.empty())
    {
        Logger::Error("Invalid arguments provided for writing Primary stream");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t headerLen = 0;
    WriteTxInfo(stm, drmTransformClass, drmTransformFeature);
    stm->write(reinterpret_cast<unsigned char*>(&headerLen), sizeof(uint32_t));
    uint32_t publishingLicenseLen = publishingLicense.size();
    stm->write(reinterpret_cast<unsigned char*>(&publishingLicenseLen), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(publishingLicense.data()), publishingLicenseLen);
    FourByteAlign(stm, publishingLicenseLen, true);
}

void DataSpaces::ReadPrimary(Stream *stm, ByteArray &publishingLicense)
{
    if(stm == nullptr || publishingLicense.empty())
    {
        Logger::Error("Invalid arguments provided for reading Primary stream");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t headerLenRead = 0;
    ReadTxInfo(stm, drmTransformClass, drmTransformFeature);
    stm->read(reinterpret_cast<unsigned char*>(&headerLenRead), sizeof(uint32_t));
    if(headerLenRead != 0)
    {
        Logger::Error("Primary stream header length mismatch", std::to_string(headerLenRead));
        throw exceptions::RMSMetroOfficeFileException("The file has been corrupted",
                                                      exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
    uint32_t publishingLicenseLen = 0;
    stm->read(reinterpret_cast<unsigned char*>(&publishingLicenseLen), sizeof(uint32_t));

    publishingLicense.clear();
    publishingLicense.resize(publishingLicenseLen);
    stm->read((unsigned char*)&publishingLicense[0], publishingLicenseLen);
    FourByteAlign(stm, publishingLicenseLen, false);
}

std::shared_ptr<IDataSpaces>IDataSpaces::Create(bool isMetro)
{
  Logger::Hidden("DataSpaces::Create");

  return std::make_shared<DataSpaces>(isMetro);
}

} // namespace officeprotector
} //namespace rmscore
