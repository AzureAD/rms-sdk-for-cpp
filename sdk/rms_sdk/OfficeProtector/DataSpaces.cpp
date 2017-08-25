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
#include "OfficeUtils.h"

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

void DataSpaces::WriteDataSpaces(GsfOutfile* stg, const ByteArray& publishingLicense)
{
    if (stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing dataspaces.");
        throw exceptions::RMSOfficeFileException(
                    "Error in writing to storage", exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    Logger::Hidden("Writing DataSpaces");
    std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceStg(
                gsf_outfile_new_child(stg, dataSpace, true));

    Logger::Hidden("Writing Version.");
    std::unique_ptr<GsfOutput, GsfOutput_deleter> versionStm(
                gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), version, false));
    gsf_output_seek(versionStm.get(), 0, G_SEEK_SET);    
    WriteVersion(versionStm.get(), versionFeature);

    Logger::Hidden("Writing DataSpaceMap.");
    std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceMapStm(
                gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), dataSpaceMap, false));
    gsf_output_seek(dataSpaceMapStm.get(), 0, G_SEEK_SET);
    WriteDataSpaceMap(dataSpaceMapStm.get());

    Logger::Hidden("Writing DRMDataSpace.");
    std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceInfoStg(
                gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), dataSpaceInfo, true));
    std::string drmDataSpaceStmName = m_isMetro ? metroDataSpace : drmDataSpace;
    std::unique_ptr<GsfOutput, GsfOutput_deleter> drmDataSpaceStm(
                gsf_outfile_new_child(GSF_OUTFILE(dataSpaceInfoStg.get()),
                                      drmDataSpaceStmName.c_str(), false));
    gsf_output_seek(drmDataSpaceStm.get(), 0, G_SEEK_SET);
    WriteDrmDataSpace(drmDataSpaceStm.get());

    Logger::Hidden("Writing Primary");
    std::unique_ptr<GsfOutput, GsfOutput_deleter> transformInfoStg(
                gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), transformInfo, true));
    std::string drmTransformStgName = m_isMetro ? metroTransform : drmTransform;
    std::unique_ptr<GsfOutput, GsfOutput_deleter> drmTransformStg(
                gsf_outfile_new_child(GSF_OUTFILE(transformInfoStg.get()),
                                      drmTransformStgName.c_str(), true));
    std::unique_ptr<GsfOutput, GsfOutput_deleter> primaryStm(
                gsf_outfile_new_child(GSF_OUTFILE(drmTransformStg.get()), primary, false));
    gsf_output_seek(primaryStm.get(), 0, G_SEEK_SET);
    WritePrimary(primaryStm.get(), publishingLicense);
}

void DataSpaces::ReadDataSpaces(GsfInfile *stg, ByteArray& publishingLicense)
{
    if (stg == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading dataspaces.");
        throw exceptions::RMSOfficeFileException( "Error in reading from storage",
                                                  exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    Logger::Hidden("Reading DataSpaces");
    std::unique_ptr<GsfInput, GsfInput_deleter> dataSpaceStg(
                gsf_infile_child_by_name(stg, dataSpace));
    std::unique_ptr<GsfInput, GsfInput_deleter> transformInfoStg(
                gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(dataSpaceStg.get()),
                                                           transformInfo));
    std::unique_ptr<GsfInput, GsfInput_deleter> passwordTransformStg(
                gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(transformInfoStg.get()),
                                         passwordTransform));
    if (passwordTransformStg)
    {
        Logger::Error("The file has been protected using non RMS technologies");
        throw exceptions::RMSOfficeFileException(
                    "The file has been protected using non RMS technologies",
                    exceptions::RMSOfficeFileException::Reason::NonRMSProtected);
    }

    std::string drmTransformStgName = m_isMetro ? metroTransform : drmTransform;
    std::unique_ptr<GsfInput, GsfInput_deleter> drmTransformStg(
                gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(transformInfoStg.get()),
                                         drmTransformStgName.c_str()));
    std::unique_ptr<GsfInput, GsfInput_deleter> primaryStm(
                gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(drmTransformStg.get()),
                                                    primary));

    if (primaryStm == nullptr)
    {
        Logger::Error("The primary stream doesn't exist.");
        throw exceptions::RMSOfficeFileException("The file has been corrupted",
                                                 exceptions::RMSOfficeFileException::Reason::CorruptFile);
    }

    Logger::Hidden("Reading Primary.");
    gsf_input_seek(primaryStm.get(), 0, G_SEEK_SET);
    ReadPrimary(primaryStm.get(), publishingLicense);
}

void DataSpaces::WriteVersion(GsfOutput *stm, const std::string& content)
{
    if (stm == nullptr || content.empty())
    {
        Logger::Error("Invalid arguments provided for writing version");
        throw exceptions::RMSOfficeFileException("Error in writing to stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
    }
    uint16_t readerMajor  = 1;
    uint16_t readerMinor  = 0;
    uint16_t updaterMajor = 1;
    uint16_t updaterMinor = 0;
    uint16_t writerMajor  = 1;
    uint16_t writerMinor  = 0;

    officeutils::WriteWideStringEntry(stm, content);
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&readerMajor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&readerMinor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&updaterMajor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&updaterMinor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&writerMajor));
    gsf_output_write(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&writerMinor));
}

void DataSpaces::ReadAndVerifyVersion(GsfInput* stm,
                                      const std::string& contentExpected)
{
    if (stm == nullptr || contentExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading version");
        throw exceptions::RMSOfficeFileException("Error in reading from stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
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

    officeutils::ReadWideStringEntry(stm, contentRead);
    gsf_input_read(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&readerMajorRead));
    gsf_input_read(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&readerMinorRead));
    gsf_input_read(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&updaterMajorRead));
    gsf_input_read(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&updaterMinorRead));
    gsf_input_read(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&writerMajorRead));
    gsf_input_read(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&writerMinorRead));

    if (contentRead.compare(contentExpected) != 0 || readerMajorRead != readerMajorExpected ||
            updaterMajorRead != updaterMajorExpected || writerMajorRead != writerMajorExpected)
    {
        Logger::Error("Major Version mismatch", contentRead, readerMajorRead,
                      updaterMajorRead, writerMajorRead);
        throw exceptions::RMSOfficeFileException("The file has been corrupted",
                                                 exceptions::RMSOfficeFileException::Reason::CorruptFile);
    }
}

void DataSpaces::WriteDataSpaceMap(GsfOutput *stm)
{
    DataSpaceMapHeader dsmh;
    DataSpaceMapEntryHeader dsmeh;
    uint32_t refVar = 0;

    if (stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing DataSpaceMap");
        throw exceptions::RMSOfficeFileException("Error in writing to stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    dsmh.headerLen = sizeof(dsmh);
    dsmh.entryCount = 1;
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmh.headerLen));
    gsf_output_write(stm, sizeof(uint32_t),
                     reinterpret_cast<const uint8_t*>(&dsmh.entryCount));

    dsmeh.entryLen = sizeof(dsmeh) + sizeof(uint32_t) +
                        (m_isMetro ? officeutils::FourByteAlignedWideStringLength(metroContent) +
                                     officeutils::FourByteAlignedWideStringLength(metroDataSpace)
                                   : officeutils::FourByteAlignedWideStringLength(drmContent) +
                                     officeutils::FourByteAlignedWideStringLength(drmDataSpace));
    dsmeh.componentCount = 1;

    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmeh.entryLen));
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmeh.componentCount));
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&refVar));
    officeutils::WriteWideStringEntry(stm, m_isMetro ? metroContent : drmContent);
    officeutils::WriteWideStringEntry(stm, m_isMetro ? metroDataSpace : drmDataSpace);
}

void DataSpaces::WriteDrmDataSpace(GsfOutput* stm)
{
    if (stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing DRMDataSpace");
        throw exceptions::RMSOfficeFileException("Error in writing to stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    DRMTransformHeader dth;
    dth.headerLen = sizeof(dth);
    dth.txCount = 1;

    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dth.headerLen));
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dth.txCount));
    officeutils::WriteWideStringEntry(stm, m_isMetro ? metroTransform : drmTransform);
}

void DataSpaces::WriteTxInfo(GsfOutput *stm,
                             const std::string& txClassName,
                             const std::string& featureName)
{
    if (stm == nullptr || txClassName.empty() || featureName.empty())
    {
        Logger::Error("Invalid arguments provided for writing Transform Info");
        throw exceptions::RMSOfficeFileException("Error in writing to stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    DRMTransformInfo dti;
    dti.headerLen = sizeof(dti) + officeutils::FourByteAlignedWideStringLength(txClassName);
    dti.txClassType = 1;

    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dti.headerLen));
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dti.txClassType));
    officeutils::WriteWideStringEntry(stm, txClassName);
    WriteVersion(stm, featureName);
}

void DataSpaces::ReadTxInfo(GsfInput* stm,
                            const std::string& txClassNameExpected,
                            const std::string& featureNameExpected)
{
    if (stm == nullptr || txClassNameExpected.empty() || featureNameExpected.empty())
    {
        Logger::Error("Invalid arguments provided for reading Transform Info");
        throw exceptions::RMSOfficeFileException("Error in reading from stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    DRMTransformInfo dtiExpected;
    dtiExpected.headerLen = sizeof(dtiExpected) +
            officeutils::FourByteAlignedWideStringLength(txClassNameExpected);
    dtiExpected.txClassType = 1;

    DRMTransformInfo dtiRead;
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&dtiRead.headerLen));
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&dtiRead.txClassType));

    if (dtiRead.headerLen != dtiExpected.headerLen || dtiRead.txClassType != dtiExpected.txClassType)
    {
        Logger::Error("DRMTransformInfo mismatch",
                      std::to_string(dtiRead.headerLen), std::to_string(dtiRead.txClassType));
        throw exceptions::RMSOfficeFileException("The file has been corrupted",
                                                 exceptions::RMSOfficeFileException::Reason::CorruptFile);
    }

    std::string txClassNameRead;
    officeutils::ReadWideStringEntry(stm, txClassNameRead);
    if (txClassNameRead.compare(txClassNameExpected) != 0)
    {
        Logger::Error("Transform Class mismatch", txClassNameRead);
        throw exceptions::RMSOfficeFileException("The file has been corrupted",
                                                 exceptions::RMSOfficeFileException::Reason::CorruptFile);
    }

    ReadAndVerifyVersion(stm, featureNameExpected);
}

void DataSpaces::WritePrimary(GsfOutput* stm,
                              const ByteArray& publishingLicense)
{
    if (stm == nullptr || publishingLicense.empty())
    {
        Logger::Error("Invalid arguments provided for writing Primary stream");
        throw exceptions::RMSOfficeFileException("Error in writing to stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    uint32_t headerLen = sizeof(headerLen);
    WriteTxInfo(stm, drmTransformClass, drmTransformFeature);
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&headerLen));

    if (m_doesUseDeprecatedAlgorithm)
    {
        std::u16string pl_utf16(reinterpret_cast<const char16_t*>(publishingLicense.data()),
                                (publishingLicense.size()+1)/2);
        auto pl_utf8 = officeutils::utf16_to_utf8(pl_utf16);
        uint32_t publishingLicenseLen = pl_utf8.length();
        gsf_output_write(stm, sizeof(uint32_t),
                         reinterpret_cast<const uint8_t*>(&publishingLicenseLen));
        gsf_output_write(stm, publishingLicenseLen,
                         reinterpret_cast<const uint8_t*>(pl_utf8.data()));
        officeutils::AlignOutputAtFourBytes(stm, publishingLicenseLen);
    }
}

void DataSpaces::ReadPrimary(GsfInput *stm, ByteArray& publishingLicense)
{
    if (stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading Primary stream");
        throw exceptions::RMSOfficeFileException("Error in reading from stream",
                                                 exceptions::RMSOfficeFileException::Reason::Unknown);
    }

    uint32_t headerLenRead = 0;
    ReadTxInfo(stm, drmTransformClass, drmTransformFeature);
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&headerLenRead));
    headerLenRead -= sizeof(headerLenRead);
    if (headerLenRead != 0)
    {
        Logger::Error("Primary stream header length mismatch", std::to_string(headerLenRead));
        throw exceptions::RMSOfficeFileException("The file has been corrupted",
                                                 exceptions::RMSOfficeFileException::Reason::CorruptFile);
    }
    uint32_t publishingLicenseLen = 0;
    gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&publishingLicenseLen));
    if(m_doesUseDeprecatedAlgorithm)
    {
        std::vector<uint8_t> pl(publishingLicenseLen);
        gsf_input_read(stm, publishingLicenseLen, &pl[0]);
        std::string pl_utf8((char*)pl.data(), pl.size());
        auto pl_utf16 = officeutils::utf8_to_utf16(pl_utf8);
        publishingLicense.clear();
        publishingLicense.assign(reinterpret_cast<const unsigned char*>(pl_utf16.data()),
                                 reinterpret_cast<const unsigned char*>(pl_utf16.data()) +
                                 pl_utf16.size() * 2);
        officeutils::AlignInputAtFourBytes(stm, publishingLicenseLen);
    }
}

std::shared_ptr<IDataSpaces>IDataSpaces::Create(bool isMetro)
{
  Logger::Hidden("DataSpaces::Create");

  return std::make_shared<DataSpaces>(isMetro);
}

} // namespace officeprotector
} // namespace rmscore
