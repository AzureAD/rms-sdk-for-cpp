/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "DataSpaces.h"
#include <algorithm>
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Logger/Logger.h"
#include "Utils.h"

using namespace rmscore::common;
using namespace rmscore::platform::logger;

namespace {
const char drmContent[]          = "\11DRMContent";
const char metroContent[]        = "EncryptedPackage";
const char dataSpace[]           = "\006DataSpaces";
const char version[]             = "Version";
const char dataSpaceMap[]        = "DataSpaceMap";
const char dataSpaceInfo[]       = "DataSpaceInfo";
const char drmDataSpace[]        = "\11DRMDataSpace";
const char metroDataSpace[]      = "DRMEncryptedDataSpace";
const char transformInfo[]       = "TransformInfo";
const char drmTransform[]        = "\11DRMTransform";
const char metroTransform[]      = "DRMEncryptedTransform";
const char passwordTransform[]   = "StrongEncryptionTransform";
const char passwordDataSpace[]   = "StrongEncryptionDataSpace";
const char primary[]             = "\006Primary";
const char versionFeature[]      = "Microsoft.Container.DataSpaces";
const char drmTransformFeature[] = "Microsoft.Metadata.DRMTransform";
const char drmTransformClass[]   = "{C73DFACD-061F-43B0-8B64-0C620D2A8B50}";
const char rootEntry[] = "/";
const char separator[] = "/";
const uint8_t BOM_UTF8[] = {0xef, 0xbb, 0xbf};

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

DataSpaces::DataSpaces(bool isMetro, bool doesUseDeprecatedAlgorithm)
{
    m_isMetro = isMetro;
    m_doesUseDeprecatedAlgorithm = doesUseDeprecatedAlgorithm;
}

DataSpaces::~DataSpaces()
{
}

void DataSpaces::WriteDataspaces(GsfOutfile* stg,
                                 const ByteArray& publishingLicense)
{
    if(stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing dataspaces.");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to storage", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    Logger::Hidden("Writing DataSpaces");
    GsfOutput* dataSpaceStg = gsf_outfile_new_child(stg, dataSpace, true);

    Logger::Hidden("Writing Version.");
    GsfOutput*  versionStm = gsf_outfile_new_child(reinterpret_cast<GsfOutfile*>(dataSpaceStg),
                                                   version, false);
    gsf_output_seek(versionStm, 0, G_SEEK_SET);
    WriteVersion(versionStm, versionFeature);

    Logger::Hidden("Writing DataSpaceMap.");
    GsfOutput* dataSpaceMapStm = gsf_outfile_new_child(reinterpret_cast<GsfOutfile*>(dataSpaceStg),
                                                       dataSpaceMap, false);
    gsf_output_seek(dataSpaceMapStm, 0, G_SEEK_SET);
    WriteDataSpaceMap(dataSpaceMapStm);

    Logger::Hidden("Writing DRMDataSpace.");
    GsfOutput* dataSpaceInfoStg = gsf_outfile_new_child(reinterpret_cast<GsfOutfile*>(dataSpaceStg),
                                                        dataSpaceInfo, true);
    std::string drmDataSpaceStmName = m_isMetro ? metroDataSpace : drmDataSpace;
    GsfOutput* drmDataSpaceStm = gsf_outfile_new_child(reinterpret_cast<GsfOutfile*>(dataSpaceInfoStg),
                                                       drmDataSpaceStmName.c_str(), false);
    gsf_output_seek(drmDataSpaceStm, 0, G_SEEK_SET);
    WriteDRMDataSpace(drmDataSpaceStm);

    Logger::Hidden("Writing Primary in.");
    GsfOutput* transformInfoStg = gsf_outfile_new_child(reinterpret_cast<GsfOutfile*>(dataSpaceStg),
                                                        transformInfo, true);
    std::string drmTransformStgName = m_isMetro ? metroTransform : drmTransform;
    GsfOutput* drmTransformStg = gsf_outfile_new_child(reinterpret_cast<GsfOutfile*>(transformInfoStg),
                                                       drmTransformStgName.c_str(), true);
    GsfOutput* primaryStm = gsf_outfile_new_child(reinterpret_cast<GsfOutfile*>(drmTransformStg),
                                                  primary, false);
    gsf_output_seek(primaryStm, 0, G_SEEK_SET);
    WritePrimary(primaryStm, publishingLicense);

    gsf_output_close(dataSpaceStg);
    gsf_output_close(versionStm);
    gsf_output_close(dataSpaceMapStm);
    gsf_output_close(dataSpaceInfoStg);
    gsf_output_close(drmDataSpaceStm);
    gsf_output_close(transformInfoStg);
    gsf_output_close(drmTransformStg);
    gsf_output_close(primaryStm);
    g_object_unref(G_OBJECT(dataSpaceStg));
    g_object_unref(G_OBJECT(versionStm));
    g_object_unref(G_OBJECT(dataSpaceMapStm));
    g_object_unref(G_OBJECT(dataSpaceInfoStg));
    g_object_unref(G_OBJECT(drmDataSpaceStm));
    g_object_unref(G_OBJECT(transformInfoStg));
    g_object_unref(G_OBJECT(drmTransformStg));
    g_object_unref(G_OBJECT(primaryStm));
}

//todo add logging here
void DataSpaces::ReadDataspaces(GsfInfile *stg,
                                ByteArray& publishingLicense)
{
    if(stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading dataspaces.");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from storage",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }
    Logger::Hidden("Reading DataSpaces");
    GsfInput* dataSpaceStg = gsf_infile_child_by_name(stg, dataSpace);
    GsfInput* transformInfoStg = gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(dataSpaceStg),
                                                           transformInfo);
    GsfInput* passwordTransformStg = gsf_infile_child_by_name(
                reinterpret_cast<GsfInfile*>(transformInfoStg), passwordTransform);
    if(passwordTransformStg != nullptr)
    {
        Logger::Error("The file has been protected using non RMS technologies");
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been protected using non RMS technologies",
                    exceptions::RMSMetroOfficeFileException::NonRMSProtected);
    }

    std::string drmTransformStgName = m_isMetro ? metroTransform : drmTransform;
    GsfInput* drmTransformStg = gsf_infile_child_by_name(
                reinterpret_cast<GsfInfile*>(transformInfoStg), drmTransformStgName.c_str());
    GsfInput* primaryStm = gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(drmTransformStg),
                                                    primary);

    if(primaryStm == nullptr)
    {
        Logger::Error("The primary stream doesn't exist.");
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    Logger::Hidden("Reading Primary.");
    gsf_input_seek(primaryStm, 0, G_SEEK_SET);
    ReadPrimary(primaryStm, publishingLicense);

    g_object_unref(G_OBJECT(dataSpaceStg));
    g_object_unref(G_OBJECT(transformInfoStg));
    g_object_unref(G_OBJECT(passwordTransformStg));
    g_object_unref(G_OBJECT(drmTransformStg));
    g_object_unref(G_OBJECT(primaryStm));
}

void DataSpaces::WriteVersion(GsfOutput *stm, const std::string& content)
{
    if( stm == nullptr || content.empty())
    {
        Logger::Error("Invalid arguments provided for writing version");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }
    uint16_t readerMajor  = 1;
    uint16_t readerMinor  = 0;
    uint16_t updaterMajor = 1;
    uint16_t updaterMinor = 0;
    uint16_t writerMajor  = 1;
    uint16_t writerMinor  = 0;

    WriteWideStringEntry(stm, content);
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const unsigned char*>(&readerMajor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const unsigned char*>(&readerMinor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const unsigned char*>(&updaterMajor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const unsigned char*>(&updaterMinor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const unsigned char*>(&writerMajor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const unsigned char*>(&writerMinor));
}

void DataSpaces::ReadAndVerifyVersion(GsfInput* stm,
                                      const std::string& contentExpected)
{
    if( stm == nullptr || contentExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading version");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from stream",
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
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&readerMajorRead));
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&readerMinorRead));
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&updaterMajorRead));
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&updaterMinorRead));
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&writerMajorRead));
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&writerMinorRead));

    if( contentRead.compare(contentExpected) != 0 || readerMajorRead != readerMajorExpected ||
            updaterMajorRead != updaterMajorExpected || writerMajorRead != writerMajorExpected)
    {
        Logger::Error("Major Version mismatch", contentRead, readerMajorRead,
                      updaterMajorRead, writerMajorRead);
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
}

void DataSpaces::WriteDataSpaceMap(GsfOutput *stm)
{
    DataSpaceMapHeader dsmh;
    DataSpaceMapEntryHeader dsmeh;
    uint32_t refVar = 0;

    if( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing DataSpaceMap");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }

    dsmh.headerLen = sizeof(dsmh);
    dsmh.entryCount = 1;
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&dsmh.headerLen));
    gsf_output_write(stm, sizeof(uint32_t),
                     reinterpret_cast<const unsigned char*>(&dsmh.entryCount));

    dsmeh.entryLen = sizeof(dsmeh) + sizeof(uint32_t) +
                        (m_isMetro ? FourByteAlignedWideStringLength(metroContent) +
                                     FourByteAlignedWideStringLength(metroDataSpace)
                                   : FourByteAlignedWideStringLength(drmContent) +
                                     FourByteAlignedWideStringLength(drmDataSpace));
    dsmeh.componentCount = 1;

    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&dsmeh.entryLen));
    gsf_output_write(stm, sizeof(uint32_t),
                     reinterpret_cast<const unsigned char*>(&dsmeh.componentCount));
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&refVar));
    WriteWideStringEntry(stm, m_isMetro ? metroContent : drmContent);
    WriteWideStringEntry(stm, m_isMetro ? metroDataSpace : drmDataSpace);
}

void DataSpaces::WriteDRMDataSpace(GsfOutput* stm)
{
    if( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing DRMDataSpace");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformHeader dth;
    dth.headerLen = sizeof(dth);
    dth.txCount = 1;

    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&dth.headerLen));
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&dth.txCount));
    WriteWideStringEntry(stm, m_isMetro ? metroTransform : drmTransform);
}

void DataSpaces::WriteTxInfo(GsfOutput *stm,
                             const std::string& txClassName,
                             const std::string& featureName)
{
    if( stm == nullptr || txClassName.empty() || featureName.empty())
    {
        Logger::Error("Invalid arguments provided for writing Transform Info");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformInfo dti;
    dti.headerLen = sizeof(dti) + FourByteAlignedWideStringLength(txClassName);
    dti.txClassType = 1;

    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&dti.headerLen));
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&dti.txClassType));
    WriteWideStringEntry(stm, txClassName);
    WriteVersion(stm, featureName);
}

void DataSpaces::ReadTxInfo(GsfInput* stm,
                            const std::string& txClassNameExpected,
                            const std::string& featureNameExpected)
{
    if( stm == nullptr || txClassNameExpected.empty() || featureNameExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading Transform Info");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }

    DRMTransformInfo dtiExpected;
    dtiExpected.headerLen = sizeof(dtiExpected) + FourByteAlignedWideStringLength(txClassNameExpected);
    dtiExpected.txClassType = 1;

    DRMTransformInfo dtiRead;
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&dtiRead.headerLen));
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&dtiRead.txClassType));

    if(dtiRead.headerLen != dtiExpected.headerLen || dtiRead.txClassType != dtiExpected.txClassType)
    {
        Logger::Error("DRMTransformInfo mismatch",
                      std::to_string(dtiRead.headerLen), std::to_string(dtiRead.txClassType));
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    std::string txClassNameRead;
    ReadWideStringEntry(stm, txClassNameRead);
    if(txClassNameRead.compare(txClassNameExpected) != 0)
    {
        Logger::Error("Transform Class mismatch", txClassNameRead);
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    ReadAndVerifyVersion(stm, featureNameExpected);
}

void DataSpaces::WritePrimary(GsfOutput* stm,
                              const ByteArray& publishingLicense)
{
    if(stm == nullptr || publishingLicense.empty())
    {
        Logger::Error("Invalid arguments provided for writing Primary stream");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t headerLen = sizeof(headerLen);
    WriteTxInfo(stm, drmTransformClass, drmTransformFeature);
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const unsigned char*>(&headerLen));
    std::string publishingLicenseStr(reinterpret_cast<const char*>(publishingLicense.data()),
                                     publishingLicense.size());
    if ((publishingLicenseStr.length() > sizeof(BOM_UTF8)) &&
            (memcmp(publishingLicenseStr.data(), BOM_UTF8, sizeof(BOM_UTF8)) == 0))
    {
        std::string utf8NoBOM(publishingLicenseStr.data() + sizeof(BOM_UTF8),
                         publishingLicenseStr.data() + publishingLicenseStr.length());
        publishingLicenseStr = utf8NoBOM;
    }

    if(m_doesUseDeprecatedAlgorithm)
    {
        publishingLicenseStr = ConvertWideStrToCharStr(publishingLicenseStr);
    }

    uint32_t publishingLicenseLen = publishingLicenseStr.length();
    gsf_output_write(stm, sizeof(uint32_t),
                     reinterpret_cast<const unsigned char*>(&publishingLicenseLen));
    gsf_output_write(stm, sizeof(uint32_t),
                     reinterpret_cast<const unsigned char*>(publishingLicenseStr.data()));
    AlignOutputAtFourBytes(stm, publishingLicenseLen);
}

void DataSpaces::ReadPrimary(GsfInput *stm, ByteArray& publishingLicense)
{
    if(stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading Primary stream");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }

    uint32_t headerLenRead = 0;
    ReadTxInfo(stm, drmTransformClass, drmTransformFeature);
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&headerLenRead));
    headerLenRead -= sizeof(headerLenRead);
    if(headerLenRead != 0)
    {
        Logger::Error("Primary stream header length mismatch", std::to_string(headerLenRead));
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }
    uint32_t publishingLicenseLen = 0;
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<unsigned char*>(&publishingLicenseLen));
    std::unique_ptr<unsigned char[]> pl(new unsigned char[publishingLicenseLen]);
    gsf_input_read(stm, publishingLicenseLen, pl.get());
    std::string publishingLicenseStr((char*)pl.get(), publishingLicenseLen);
    if(m_doesUseDeprecatedAlgorithm)
    {
        publishingLicenseStr = ConvertCharStrToWideStr(publishingLicenseStr);
    }

    publishingLicense.clear();
    std::copy(publishingLicenseStr.begin(), publishingLicenseStr.end(), std::back_inserter(publishingLicense));
    AlignInputAtFourBytes(stm, publishingLicenseLen);
}

std::shared_ptr<IDataSpaces>IDataSpaces::Create(bool isMetro)
{
  Logger::Hidden("DataSpaces::Create");

  return std::make_shared<DataSpaces>(isMetro);
}

} // namespace officeprotector
} // namespace rmscore
