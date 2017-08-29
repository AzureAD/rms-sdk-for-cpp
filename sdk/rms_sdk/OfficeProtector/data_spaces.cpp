/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "data_spaces.h"
#include <gsf/gsf.h>
#include "../ModernAPI/RMSExceptions.h"
#include "../Platform/Logger/Logger.h"
#include "office_utils.h"

using namespace rmscore::common;
using namespace rmscore::platform::logger;

namespace {

const char kDrmContent[]          = "\11DRMContent";
const char kMetroContent[]        = "EncryptedPackage";
const char kDataSpace[]           = "\006DataSpaces";
const char kVersion[]             = "Version";
const char kDataSpaceMap[]        = "DataSpaceMap";
const char kDataSpaceInfo[]       = "DataSpaceInfo";
const char kDrmDataSpace[]        = "\11DRMDataSpace";
const char kMetroDataSpace[]      = "DRMEncryptedDataSpace";
const char KTransformInfo[]       = "TransformInfo";
const char kDrmTransform[]        = "\11DRMTransform";
const char lMetroTransform[]      = "DRMEncryptedTransform";
const char kPasswordTransform[]   = "StrongEncryptionTransform";
const char kPasswordDataSpace[]   = "StrongEncryptionDataSpace";
const char kPrimary[]             = "\006Primary";
const char kVersionFeature[]      = "Microsoft.Container.DataSpaces";
const char kDrmTransformFeature[] = "Microsoft.Metadata.DRMTransform";
const char kDrmTransformClass[]   = "{C73DFACD-061F-43B0-8B64-0C620D2A8B50}";
const char kRootEntry[] = "/";
const char kSeparator[] = "/";
const uint8_t kBOM_UTF8[] = {0xef, 0xbb, 0xbf};

// DataSpaceMap Header
struct DataSpaceMapHeader {
  uint32_t headerLen;
  uint32_t entryCount;
};

// DataSpaceMap Entry Header
struct DataSpaceMapEntryHeader {
  uint32_t entryLen;
  uint32_t componentCount;
};

// DRM Transform Header
struct DRMTransformHeader {
  uint32_t headerLen;
  uint32_t txCount;
};

// DRM Transform Info
struct DRMTransformInfo {
  uint32_t headerLen;
  uint32_t txClassType;
};

}   // namespace

namespace rmscore {
namespace officeprotector {

DataSpaces::DataSpaces(bool isMetro, bool doesUseDeprecatedAlgorithm)
  : mIsMetro(isMetro),
    mDoesUseDeprecatedAlgorithm(doesUseDeprecatedAlgorithm) {
}

DataSpaces::~DataSpaces() {
}

void DataSpaces::WriteDataSpaces(GsfOutfile* stg, const ByteArray& publishingLicense) {
  if (stg == nullptr) {
    Logger::Error("Invalid arguments provided for writing dataspaces.");
    throw exceptions::RMSOfficeFileException(
          "Error in writing to storage",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  Logger::Hidden("Writing DataSpaces");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceStg(
        gsf_outfile_new_child(stg, kDataSpace, true));
  Logger::Hidden("Writing Version.");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> versionStm(
        gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), kVersion, false));
  gsf_output_seek(versionStm.get(), 0, G_SEEK_SET);
  WriteVersion(versionStm.get(), kVersionFeature);
  Logger::Hidden("Writing DataSpaceMap.");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceMapStm(
        gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), kDataSpaceMap, false));
  gsf_output_seek(dataSpaceMapStm.get(), 0, G_SEEK_SET);
  WriteDataSpaceMap(dataSpaceMapStm.get());
  Logger::Hidden("Writing DRMDataSpace.");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceInfoStg(
        gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), kDataSpaceInfo, true));
  std::string drmDataSpaceStmName = mIsMetro ? kMetroDataSpace : kDrmDataSpace;
  std::unique_ptr<GsfOutput, GsfOutput_deleter> drmDataSpaceStm(
        gsf_outfile_new_child(
          GSF_OUTFILE(dataSpaceInfoStg.get()),
          drmDataSpaceStmName.c_str(),
          false));
  gsf_output_seek(drmDataSpaceStm.get(), 0, G_SEEK_SET);
  WriteDrmDataSpace(drmDataSpaceStm.get());
  Logger::Hidden("Writing Primary");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> transformInfoStg(
        gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), KTransformInfo, true));
  std::string drmTransformStgName = mIsMetro ? lMetroTransform : kDrmTransform;
  std::unique_ptr<GsfOutput, GsfOutput_deleter> drmTransformStg(
        gsf_outfile_new_child(
          GSF_OUTFILE(transformInfoStg.get()),
          drmTransformStgName.c_str(),
          true));
  std::unique_ptr<GsfOutput, GsfOutput_deleter> primaryStm(
        gsf_outfile_new_child(GSF_OUTFILE(drmTransformStg.get()), kPrimary, false));
  gsf_output_seek(primaryStm.get(), 0, G_SEEK_SET);
  WritePrimary(primaryStm.get(), publishingLicense);
}

void DataSpaces::ReadDataSpaces(GsfInfile *stg, ByteArray& publishingLicense) {
  if (stg == nullptr) {
    Logger::Error("Invalid arguments provided for reading dataspaces.");
    throw exceptions::RMSOfficeFileException(
          "Error in reading from storage",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  Logger::Hidden("Reading DataSpaces");
  std::unique_ptr<GsfInput, GsfInput_deleter> dataSpaceStg(
        gsf_infile_child_by_name(stg, kDataSpace));
  std::unique_ptr<GsfInput, GsfInput_deleter> transformInfoStg(
        gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(dataSpaceStg.get()), KTransformInfo));
  std::unique_ptr<GsfInput, GsfInput_deleter> passwordTransformStg(
        gsf_infile_child_by_name(
          reinterpret_cast<GsfInfile*>(transformInfoStg.get()),
          kPasswordTransform));
  if (passwordTransformStg) {
    Logger::Error("The file has been protected using non RMS technologies");
    throw exceptions::RMSOfficeFileException(
          "The file has been protected using non RMS technologies",
          exceptions::RMSOfficeFileException::Reason::NonRMSProtected);
  }
  std::string drmTransformStgName = mIsMetro ? lMetroTransform : kDrmTransform;
  std::unique_ptr<GsfInput, GsfInput_deleter> drmTransformStg(
        gsf_infile_child_by_name(
          reinterpret_cast<GsfInfile*>(transformInfoStg.get()),
          drmTransformStgName.c_str()));
  std::unique_ptr<GsfInput, GsfInput_deleter> primaryStm(
        gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(drmTransformStg.get()), kPrimary));
  if (primaryStm == nullptr) {
    Logger::Error("The primary stream doesn't exist.");
    throw exceptions::RMSOfficeFileException(
          "The file has been corrupted",
          exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
  Logger::Hidden("Reading Primary.");
  gsf_input_seek(primaryStm.get(), 0, G_SEEK_SET);
  ReadPrimary(primaryStm.get(), publishingLicense);
}

void DataSpaces::WriteVersion(GsfOutput *stm, const std::string& content) {
  if (stm == nullptr || content.empty()) {
    Logger::Error("Invalid arguments provided for writing version");
    throw exceptions::RMSOfficeFileException(
          "Error in writing to stream",
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

void DataSpaces::ReadAndVerifyVersion(GsfInput* stm, const std::string& contentExpected) {
  if (stm == nullptr || contentExpected.empty()) {
    Logger::Error("Invalid arguments provided for reading version");
    throw exceptions::RMSOfficeFileException(
          "Error in reading from stream",
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
      updaterMajorRead != updaterMajorExpected || writerMajorRead != writerMajorExpected) {
    Logger::Error(
          "Major Version mismatch",
          contentRead,
          readerMajorRead,
          updaterMajorRead,
          writerMajorRead);
    throw exceptions::RMSOfficeFileException(
          "The file has been corrupted",
          exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
}

void DataSpaces::WriteDataSpaceMap(GsfOutput *stm) {
  DataSpaceMapHeader dsmh;
  DataSpaceMapEntryHeader dsmeh;
  uint32_t refVar = 0;
  if (stm == nullptr) {
    Logger::Error("Invalid arguments provided for writing DataSpaceMap");
    throw exceptions::RMSOfficeFileException(
          "Error in writing to stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  dsmh.headerLen = sizeof(dsmh);
  dsmh.entryCount = 1;
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmh.headerLen));
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmh.entryCount));
  dsmeh.entryLen = sizeof(dsmeh) + sizeof(uint32_t) +
      (mIsMetro ? officeutils::FourByteAlignedWideStringLength(kMetroContent) +
                  officeutils::FourByteAlignedWideStringLength(kMetroDataSpace)
                : officeutils::FourByteAlignedWideStringLength(kDrmContent) +
                  officeutils::FourByteAlignedWideStringLength(kDrmDataSpace));
  dsmeh.componentCount = 1;
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmeh.entryLen));
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmeh.componentCount));
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&refVar));
  officeutils::WriteWideStringEntry(stm, mIsMetro ? kMetroContent : kDrmContent);
  officeutils::WriteWideStringEntry(stm, mIsMetro ? kMetroDataSpace : kDrmDataSpace);
}

void DataSpaces::WriteDrmDataSpace(GsfOutput* stm) {
  if (stm == nullptr) {
    Logger::Error("Invalid arguments provided for writing DRMDataSpace");
    throw exceptions::RMSOfficeFileException(
          "Error in writing to stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  DRMTransformHeader dth;
  dth.headerLen = sizeof(dth);
  dth.txCount = 1;
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dth.headerLen));
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dth.txCount));
  officeutils::WriteWideStringEntry(stm, mIsMetro ? lMetroTransform : kDrmTransform);
}

void DataSpaces::WriteTxInfo(
    GsfOutput *stm,
    const std::string& txClassName,
    const std::string& featureName) {
  if (stm == nullptr || txClassName.empty() || featureName.empty()) {
    Logger::Error("Invalid arguments provided for writing Transform Info");
    throw exceptions::RMSOfficeFileException(
          "Error in writing to stream",
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

void DataSpaces::ReadTxInfo(
    GsfInput* stm,
    const std::string& txClassNameExpected,
    const std::string& featureNameExpected) {
  if (stm == nullptr || txClassNameExpected.empty() || featureNameExpected.empty()) {
    Logger::Error("Invalid arguments provided for reading Transform Info");
    throw exceptions::RMSOfficeFileException(
          "Error in reading from stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  DRMTransformInfo dtiExpected;
  dtiExpected.headerLen = sizeof(dtiExpected) +
      officeutils::FourByteAlignedWideStringLength(txClassNameExpected);
  dtiExpected.txClassType = 1;
  DRMTransformInfo dtiRead;
  gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&dtiRead.headerLen));
  gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&dtiRead.txClassType));
  if (dtiRead.headerLen != dtiExpected.headerLen || dtiRead.txClassType != dtiExpected.txClassType) {
    Logger::Error(
          "DRMTransformInfo mismatch",
          std::to_string(dtiRead.headerLen),
          std::to_string(dtiRead.txClassType));
    throw exceptions::RMSOfficeFileException(
          "The file has been corrupted",
          exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
  std::string txClassNameRead;
  officeutils::ReadWideStringEntry(stm, txClassNameRead);
  if (txClassNameRead.compare(txClassNameExpected) != 0) {
    Logger::Error("Transform Class mismatch", txClassNameRead);
    throw exceptions::RMSOfficeFileException(
          "The file has been corrupted",
          exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
  ReadAndVerifyVersion(stm, featureNameExpected);
}

void DataSpaces::WritePrimary(GsfOutput* stm, const ByteArray& publishingLicense) {
  if (stm == nullptr || publishingLicense.empty()) {
    Logger::Error("Invalid arguments provided for writing Primary stream");
    throw exceptions::RMSOfficeFileException(
          "Error in writing to stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  uint32_t headerLen = sizeof(headerLen);
  WriteTxInfo(stm, kDrmTransformClass, kDrmTransformFeature);
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&headerLen));
  if (mDoesUseDeprecatedAlgorithm) {
    std::u16string pl_utf16(
          reinterpret_cast<const char16_t*>(publishingLicense.data()),
          (publishingLicense.size()+1)/2);
    auto pl_utf8 = officeutils::utf16_to_utf8(pl_utf16);
    uint32_t publishingLicenseLen = pl_utf8.length();
    gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&publishingLicenseLen));
    gsf_output_write(stm, publishingLicenseLen, reinterpret_cast<const uint8_t*>(pl_utf8.data()));
    officeutils::AlignOutputAtFourBytes(stm, publishingLicenseLen);
  }
}

void DataSpaces::ReadPrimary(GsfInput *stm, ByteArray& publishingLicense) {
  if (stm == nullptr) {
    Logger::Error("Invalid arguments provided for reading Primary stream");
    throw exceptions::RMSOfficeFileException(
          "Error in reading from stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  uint32_t headerLenRead = 0;
  ReadTxInfo(stm, kDrmTransformClass, kDrmTransformFeature);
  gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&headerLenRead));
  headerLenRead -= sizeof(headerLenRead);
  if (headerLenRead != 0) {
    Logger::Error("Primary stream header length mismatch", std::to_string(headerLenRead));
    throw exceptions::RMSOfficeFileException(
          "The file has been corrupted",
          exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
  uint32_t publishingLicenseLen = 0;
  gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&publishingLicenseLen));
  if (mDoesUseDeprecatedAlgorithm) {
    std::vector<uint8_t> pl(publishingLicenseLen);
    gsf_input_read(stm, publishingLicenseLen, &pl[0]);
    std::string pl_utf8((char*)pl.data(), pl.size());
    auto pl_utf16 = officeutils::utf8_to_utf16(pl_utf8);
    publishingLicense.clear();
    publishingLicense.assign(
          reinterpret_cast<const unsigned char*>(pl_utf16.data()),
          reinterpret_cast<const unsigned char*>(pl_utf16.data()) +  pl_utf16.size() * 2);
    officeutils::AlignInputAtFourBytes(stm, publishingLicenseLen);
  }
}

std::shared_ptr<IDataSpaces>IDataSpaces::Create(bool isMetro) {
  Logger::Hidden("DataSpaces::Create");
  return std::make_shared<DataSpaces>(isMetro);
}

} // namespace officeprotector
} // namespace rmscore
