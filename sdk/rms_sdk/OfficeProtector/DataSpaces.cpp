/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "DataSpaces.h"
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Logger/Logger.h"
#include "Utils.h"

using namespace rmscore::common;
using namespace rmscore::officeprotector;
using namespace rmscore::pole;
using namespace rmscore::platform::logger;

namespace {

const std::string drmContent          = "\11DRMContent";
const std::string metroContent        = "EncryptedPackage";
const std::string dataSpace           = "\006DataSpaces";
const std::string version             = "Version";
const std::string dataSpaceMap        = "DataSpaceMap";
const std::string dataSpaceInfo       = "DataSpaceInfo";
const std::string drmDataSpace        = "\11DRMDataSpace";
const std::string metroDataSpace      = "DRMEncryptedDataSpace";
const std::string transformInfo       = "TransformInfo";
const std::string drmTransform        = "\11DRMTransform";
const std::string metroTransform      = "DRMEncryptedTransform";
const std::string passwordTransform   = "StrongEncryptionTransform";
const std::string passwordDataSpace   = "StrongEncryptionDataSpace";
const std::string primary             = "\006Primary";
const std::string versionFeature      = "Microsoft.Container.DataSpaces";
const std::string drmTransformFeature = "Microsoft.Metadata.DRMTransform";
const std::string drmTransformClass   = "{C73DFACD-061F-43B0-8B64-0C620D2A8B50}";
const std::string rootEntry = "/";
const std::string separator = "/";

// DataSpaceMap Header
struct DataSpaceMapHeader
{
    uint32_t headerLen;
    uint32_t entryCount;
};

// DataSpaceMap Entry Header
struct DataSpaceMapEntryHeader
{
    uint32_t entryLen;
    uint32_t componentCount;
};

// DRM Transform Header
struct DRMTransformHeader
{
    uint32_t headerLen;
    uint32_t txCount;
};

// DRM Transform Info
struct DRMTransformInfo
{
    uint32_t headerLen;
    uint32_t txClassType;
};

}   // namespace

namespace rmscore {
namespace officeprotector {

DataSpaces::DataSpaces(bool isMetro)
{
    this->isMetro = isMetro;
}

DataSpaces::~DataSpaces()
{
}

void DataSpaces::WriteDataspaces(std::shared_ptr<Storage> stg, ByteArray publishingLicense)
{
    if(stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing dataspaces.");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to storage",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }
    Logger::Hidden("Writing DataSpaces");
    Logger::Hidden("Deleting DataSpaces if exists");
    std::string dataSpaceStgName = rootEntry + dataSpace;
    stg->deleteByName(dataSpaceStgName);

    std::string versionStmName = dataSpaceStgName + separator + version;
    Logger::Hidden("Writing Version in " + versionStmName);
    std::shared_ptr<Stream> versionStm = std::make_shared<Stream>(stg.get(), versionStmName, true);
    versionStm->seek(0);
    WriteVersion(versionStm, versionFeature);
    versionStm->flush();

    std::string dataSpaceMapStmName = dataSpaceStgName + separator + dataSpaceMap;
    Logger::Hidden("Writing DataSpaceMap in " + dataSpaceMapStmName);
    std::shared_ptr<Stream> dataSpaceMapStm = std::make_shared<Stream>(stg.get(), dataSpaceMapStmName, true);
    dataSpaceMapStm->seek(0);
    WriteDataSpaceMap(dataSpaceMapStm);
    dataSpaceMapStm->flush();

    std::string dataSpaceInfoStgName = dataSpaceStgName + separator + dataSpaceInfo;
    std::string drmDataSpaceStmName = dataSpaceInfoStgName + separator +
            (isMetro ? metroDataSpace : drmDataSpace);
    Logger::Hidden("Writing DRMDataSpace in " + drmDataSpaceStmName);
    std::shared_ptr<Stream> drmDataSpaceStm = std::make_shared<Stream>(stg.get(), drmDataSpaceStmName, true);
    drmDataSpaceStm->seek(0);
    WriteDRMDataSpace(drmDataSpaceStm);
    drmDataSpaceStm->flush();

    std::string transformInfoStgName = dataSpaceStgName + separator + transformInfo;
    std::string drmTransformStgName = transformInfoStgName + separator +
            (isMetro ? metroTransform : drmTransform);
    std::string primaryStmName = drmTransformStgName + separator + primary;
    Logger::Hidden("Writing Primary in " + primaryStmName);
    std::shared_ptr<Stream> primaryStm = std::make_shared<Stream>(stg.get(), primaryStmName, true);
    primaryStm->seek(0);
    WritePrimary(primaryStm, publishingLicense);
    primaryStm->flush();
}

//todo add logging here
void DataSpaces::ReadDataspaces(std::shared_ptr<Storage> stg, ByteArray& publishingLicense)
{
    if(stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading dataspaces.");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from storage",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }
    Logger::Hidden("Reading DataSpaces");
    std::string dataSpaceStgName = rootEntry + dataSpace;
    std::string transformInfoStgName = dataSpaceStgName + separator + transformInfo;
    std::string passwordTransformStgName = transformInfoStgName + separator + passwordTransform;

    if(stg->exists(passwordTransformStgName))
    {
        Logger::Error("The file has been protected using non RMS technologies");
        exceptions::RMSMetroOfficeFileException("The file has been protected using non RMS technologies",
                                                exceptions::RMSMetroOfficeFileException::NonRMSProtected);
    }

    std::string primaryStmName = transformInfoStgName + separator + (isMetro ? metroTransform : drmTransform)
            + separator + primary;

    if(!stg->exists(primaryStmName))
    {
        Logger::Error("The primary stream doesn't exist.");
        exceptions::RMSMetroOfficeFileException("The file has been corrupted",
                                                exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    Logger::Hidden("Reading Primary from " + primaryStmName);
    std::shared_ptr<Stream> stm = std::make_shared<Stream>(stg.get(), primaryStmName, false);
    stm->seek(0);
    ReadPrimary(stm, publishingLicense);
}

void DataSpaces::WriteVersion(std::shared_ptr<Stream> stm, const std::string& content)
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

void DataSpaces::ReadAndVerifyVersion(std::shared_ptr<Stream> stm, const std::string& contentExpected)
{
    if( stm == nullptr || contentExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading version");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint16_t readerMajorExpected  = 1;
    uint16_t updaterMajorExpected = 1;
    uint16_t writerMajorExpected  = 1;

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

    if( contentRead.compare(contentExpected) != 0 || readerMajorRead != readerMajorExpected ||
            updaterMajorRead != updaterMajorExpected || writerMajorRead != writerMajorExpected)
    {
        Logger::Error("Major Version mismatch", contentRead, readerMajorRead,
                      updaterMajorRead, writerMajorRead);
        throw exceptions::RMSMetroOfficeFileException("The file has been corrupted",
                                                      exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
}

void DataSpaces::WriteDataSpaceMap(std::shared_ptr<Stream> stm)
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

    dsmeh.entryLen = sizeof(dsmeh) + sizeof(uint32_t) + (isMetro ? FourByteAlignedWideStringLength(metroContent)
                                                                   + FourByteAlignedWideStringLength(metroDataSpace) :
                                                                   FourByteAlignedWideStringLength(drmContent)
                                                                   + FourByteAlignedWideStringLength(drmDataSpace));
    dsmeh.componentCount = 1;

    stm->write(reinterpret_cast<unsigned char*>(&dsmeh.entryLen), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&dsmeh.componentCount), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&refVar), sizeof(uint32_t));
    WriteWideStringEntry(stm, isMetro ? metroContent : drmContent);
    WriteWideStringEntry(stm, isMetro ? metroDataSpace : drmDataSpace);
}

void DataSpaces::WriteDRMDataSpace(std::shared_ptr<Stream> stm)
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
    WriteWideStringEntry(stm, isMetro ? metroTransform : drmTransform);
}

void DataSpaces::WriteTxInfo(std::shared_ptr<Stream> stm, const std::string& txClassName, const std::string& featureName)
{
    if( stm == nullptr || txClassName.empty() || featureName.empty())
    {
        Logger::Error("Invalid arguments provided for writing Transform Info");
        throw exceptions::RMSMetroOfficeFileException("Error in writing to stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformInfo dti;
    dti.headerLen = sizeof(dti) + FourByteAlignedWideStringLength(txClassName);
    dti.txClassType = 1;

    stm->write(reinterpret_cast<unsigned char*>(&dti.headerLen), sizeof(uint32_t));
    stm->write(reinterpret_cast<unsigned char*>(&dti.txClassType), sizeof(uint32_t));
    WriteWideStringEntry(stm, txClassName);
    WriteVersion(stm, featureName);

}

void DataSpaces::ReadTxInfo(std::shared_ptr<Stream> stm, const std::string& txClassNameExpected, const std::string& featureNameExpected)
{
    if( stm == nullptr || txClassNameExpected.empty() || featureNameExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading Transform Info");
        throw exceptions::RMSMetroOfficeFileException("Error in reading from stream",
                                                      exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformInfo dtiExpected;
    dtiExpected.headerLen = sizeof(dtiExpected) + FourByteAlignedWideStringLength(txClassNameExpected);
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

    ReadAndVerifyVersion(stm, featureNameExpected);

}

void DataSpaces::WritePrimary(std::shared_ptr<Stream> stm, ByteArray publishingLicense)
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
    AlignAtFourBytes(stm, publishingLicenseLen, true);
}

void DataSpaces::ReadPrimary(std::shared_ptr<Stream> stm, ByteArray &publishingLicense)
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
    AlignAtFourBytes(stm, publishingLicenseLen, false);
}

std::shared_ptr<IDataSpaces>IDataSpaces::Create(bool isMetro)
{
  Logger::Hidden("DataSpaces::Create");

  return std::make_shared<DataSpaces>(isMetro);
}

} // namespace officeprotector
} // namespace rmscore
