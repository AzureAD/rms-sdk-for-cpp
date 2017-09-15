/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "data_spaces.h"
#include <gsf/gsf.h>
#include "RMSExceptions.h"
#include "Logger.h"
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
  officeutils::CheckGsfOutfile(stg);
  Logger::Hidden("Writing DataSpaces");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceStg(
      gsf_outfile_new_child(stg, kDataSpace, true));
  Logger::Hidden("Writing Version.");
  officeutils::CheckGsfOutput(dataSpaceStg.get());
  std::unique_ptr<GsfOutput, GsfOutput_deleter> versionStm(
      gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), kVersion, false));
  officeutils::GsfOutputSeek(versionStm.get(), 0, G_SEEK_SET);
  WriteVersion(versionStm.get(), kVersionFeature);
  Logger::Hidden("Writing DataSpaceMap.");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceMapStm(
      gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), kDataSpaceMap, false));
  officeutils::GsfOutputSeek(dataSpaceMapStm.get(), 0, G_SEEK_SET);
  WriteDataSpaceMap(dataSpaceMapStm.get());
  Logger::Hidden("Writing DRMDataSpace.");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> dataSpaceInfoStg(
      gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), kDataSpaceInfo, true));
  officeutils::CheckGsfOutput(dataSpaceInfoStg.get());
  std::string drmDataSpaceStmName = mIsMetro ? kMetroDataSpace : kDrmDataSpace;
  std::unique_ptr<GsfOutput, GsfOutput_deleter> drmDataSpaceStm(
      gsf_outfile_new_child(
          GSF_OUTFILE(dataSpaceInfoStg.get()),
          drmDataSpaceStmName.c_str(),
          false));
  officeutils::GsfOutputSeek(drmDataSpaceStm.get(), 0, G_SEEK_SET);
  WriteDrmDataSpace(drmDataSpaceStm.get());
  Logger::Hidden("Writing Primary");
  std::unique_ptr<GsfOutput, GsfOutput_deleter> transformInfoStg(
      gsf_outfile_new_child(GSF_OUTFILE(dataSpaceStg.get()), KTransformInfo, true));
  officeutils::CheckGsfOutput(transformInfoStg.get());
  std::string drmTransformStgName = mIsMetro ? lMetroTransform : kDrmTransform;
  std::unique_ptr<GsfOutput, GsfOutput_deleter> drmTransformStg(
      gsf_outfile_new_child(
          GSF_OUTFILE(transformInfoStg.get()),
          drmTransformStgName.c_str(),
          true));
  officeutils::CheckGsfOutput(drmTransformStg.get());
  std::unique_ptr<GsfOutput, GsfOutput_deleter> primaryStm(
      gsf_outfile_new_child(GSF_OUTFILE(drmTransformStg.get()), kPrimary, false));
  officeutils::GsfOutputSeek(primaryStm.get(), 0, G_SEEK_SET);
  WritePrimary(primaryStm.get(), publishingLicense);
}

void DataSpaces::ReadDataSpaces(GsfInfile* stg, ByteArray& publishingLicense) {
  officeutils::CheckGsfInfile(stg);
  Logger::Hidden("Reading DataSpaces");
  std::unique_ptr<GsfInput, GsfInput_deleter> dataSpaceStg(
      gsf_infile_child_by_name(stg, kDataSpace));
  officeutils::CheckGsfInput(dataSpaceStg.get());
  std::unique_ptr<GsfInput, GsfInput_deleter> transformInfoStg(
      gsf_infile_child_by_name(GSF_INFILE(dataSpaceStg.get()), KTransformInfo));
  officeutils::CheckGsfInput(transformInfoStg.get());
  std::unique_ptr<GsfInput, GsfInput_deleter> passwordTransformStg(
      gsf_infile_child_by_name(
          reinterpret_cast<GsfInfile*>(transformInfoStg.get()),
          kPasswordTransform));
  // Presence of password transform means file is not protected by RMS.
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
  officeutils::CheckGsfInput(drmTransformStg.get());
  std::unique_ptr<GsfInput, GsfInput_deleter> primaryStm(
      gsf_infile_child_by_name(reinterpret_cast<GsfInfile*>(drmTransformStg.get()), kPrimary));
  Logger::Hidden("Reading Primary.");
  officeutils::GsfInputSeek(primaryStm.get(), 0, G_SEEK_SET);
  ReadPrimary(primaryStm.get(), publishingLicense);
}

void DataSpaces::WriteVersion(GsfOutput* stm, const std::string& content) {
  if (content.empty()) {
    Logger::Error("Invalid arguments provided for writing Version");
    throw exceptions::RMSOfficeFileException(
        "Invalid arguments provided for writing Version",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  officeutils::CheckGsfOutput(stm);
  uint16_t readerMajor  = 1;
  uint16_t readerMinor  = 0;
  uint16_t updaterMajor = 1;
  uint16_t updaterMinor = 0;
  uint16_t writerMajor  = 1;
  uint16_t writerMinor  = 0;
  officeutils::WriteWideStringEntry(stm, content);
  officeutils::GsfWrite(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&readerMajor));
  officeutils::GsfWrite(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&readerMinor));
  officeutils::GsfWrite(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&updaterMajor));
  officeutils::GsfWrite(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&updaterMinor));
  officeutils::GsfWrite(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&writerMajor));
  officeutils::GsfWrite(stm, sizeof(uint16_t), reinterpret_cast<const uint8_t*>(&writerMinor));
}

void DataSpaces::ReadAndVerifyVersion(GsfInput* stm, const std::string& contentExpected) {
  if (contentExpected.empty()) {
    Logger::Error("Invalid arguments provided for reading Version");
    throw exceptions::RMSOfficeFileException(
        "Invalid arguments provided for reading Version",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  officeutils::CheckGsfInput(stm);
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
  officeutils::GsfRead(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&readerMajorRead));
  officeutils::GsfRead(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&readerMinorRead));
  officeutils::GsfRead(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&updaterMajorRead));
  officeutils::GsfRead(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&updaterMinorRead));
  officeutils::GsfRead(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&writerMajorRead));
  officeutils::GsfRead(stm, sizeof(uint16_t), reinterpret_cast<uint8_t*>(&writerMinorRead));
  if (contentRead != contentExpected || readerMajorRead != readerMajorExpected ||
      updaterMajorRead != updaterMajorExpected || writerMajorRead != writerMajorExpected) {
    Logger::Error(
        "Version mismatch",
        contentRead,
        readerMajorRead,
        updaterMajorRead,
        writerMajorRead);
    throw exceptions::RMSOfficeFileException(
        "The file has been corrupted",
        exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
}

void DataSpaces::WriteDataSpaceMap(GsfOutput* stm) {
  officeutils::CheckGsfOutput(stm);
  DataSpaceMapHeader dsmh;
  DataSpaceMapEntryHeader dsmeh;
  uint32_t refVar = 0;
  dsmh.headerLen = sizeof(DataSpaceMapHeader);
  dsmh.entryCount = 1;
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmh.headerLen));
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmh.entryCount));
  dsmeh.entryLen = sizeof(dsmeh) + sizeof(uint32_t) +
      (mIsMetro ? officeutils::FourByteAlignedWideStringLength(kMetroContent) +
                  officeutils::FourByteAlignedWideStringLength(kMetroDataSpace)
                : officeutils::FourByteAlignedWideStringLength(kDrmContent) +
                  officeutils::FourByteAlignedWideStringLength(kDrmDataSpace));
  dsmeh.componentCount = 1;
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmeh.entryLen));
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dsmeh.componentCount));
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&refVar));
  officeutils::WriteWideStringEntry(stm, mIsMetro ? kMetroContent : kDrmContent);
  officeutils::WriteWideStringEntry(stm, mIsMetro ? kMetroDataSpace : kDrmDataSpace);
}

void DataSpaces::WriteDrmDataSpace(GsfOutput* stm) {
  officeutils::CheckGsfOutput(stm);
  DRMTransformHeader dth;
  dth.headerLen = sizeof(dth);
  dth.txCount = 1;
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dth.headerLen));
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dth.txCount));
  officeutils::WriteWideStringEntry(stm, mIsMetro ? lMetroTransform : kDrmTransform);
}

void DataSpaces::WriteTxInfo(
    GsfOutput* stm,
    const std::string& txClassName,
    const std::string& featureName) {
  if (txClassName.empty() || featureName.empty()) {
    Logger::Error("Invalid arguments provided for writing Transform Info");
    throw exceptions::RMSOfficeFileException(
        "Invalid arguments provided for writing Transform Info",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  officeutils::CheckGsfOutput(stm);
  DRMTransformInfo dti;
  dti.headerLen = sizeof(dti) + officeutils::FourByteAlignedWideStringLength(txClassName);
  dti.txClassType = 1;
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dti.headerLen));
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&dti.txClassType));
  officeutils::WriteWideStringEntry(stm, txClassName);
  WriteVersion(stm, featureName);
}

void DataSpaces::ReadTxInfo(
    GsfInput* stm,
    const std::string& txClassNameExpected,
    const std::string& featureNameExpected) {
  if (txClassNameExpected.empty() || featureNameExpected.empty()) {
    Logger::Error("Invalid arguments provided for reading Transform Info");
    throw exceptions::RMSOfficeFileException(
        "Invalid arguments provided for writing Transform Info",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  officeutils::CheckGsfInput(stm);
  DRMTransformInfo dtiExpected;
  dtiExpected.headerLen = sizeof(dtiExpected) +
      officeutils::FourByteAlignedWideStringLength(txClassNameExpected);
  dtiExpected.txClassType = 1;
  DRMTransformInfo dtiRead;
  officeutils::GsfRead(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&dtiRead.headerLen));
  officeutils::GsfRead(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&dtiRead.txClassType));
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
  if (publishingLicense.empty()) {
    Logger::Error("Publishing license is empty. Nothing to write");
    throw exceptions::RMSOfficeFileException(
        "Empty publishing license",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  officeutils::CheckGsfOutput(stm);
  uint32_t headerLen = sizeof(headerLen);
  WriteTxInfo(stm, kDrmTransformClass, kDrmTransformFeature);
  officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&headerLen));
  if (mDoesUseDeprecatedAlgorithm) {
    std::u16string pl_utf16(
        reinterpret_cast<const char16_t*>(publishingLicense.data()),
        (publishingLicense.size()+1)/2);
    auto pl_utf8 = officeutils::utf16_to_utf8(pl_utf16);
    uint32_t publishingLicenseLen = pl_utf8.length();
    officeutils::GsfWrite(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&publishingLicenseLen));
    officeutils::GsfWrite(stm, publishingLicenseLen, reinterpret_cast<const uint8_t*>(pl_utf8.data()));
    officeutils::AlignOutputAtFourBytes(stm, publishingLicenseLen);
  }
}

void DataSpaces::ReadPrimary(GsfInput* stm, ByteArray& publishingLicense) {
  officeutils::CheckGsfInput(stm);
  uint32_t headerLenRead = 0;
  // Read and verify information about transformation used.
  ReadTxInfo(stm, kDrmTransformClass, kDrmTransformFeature);
  officeutils::GsfRead(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&headerLenRead));
  headerLenRead -= sizeof(uint32_t);
  if (headerLenRead != 0) {
    Logger::Error("Primary stream header length mismatch", std::to_string(headerLenRead));
    throw exceptions::RMSOfficeFileException(
        "The file has been corrupted",
        exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
  uint32_t publishingLicenseLen = 0;
  // Read length of the publishing license.
  officeutils::GsfRead(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&publishingLicenseLen));
  if (publishingLicenseLen == 0) {
    Logger::Error("Length of publishing license read is 0");
    throw exceptions::RMSOfficeFileException(
        "Publishing license is empty",
        exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
  if (mDoesUseDeprecatedAlgorithm) {
    std::vector<uint8_t> pl(publishingLicenseLen);
    officeutils::GsfRead(stm, publishingLicenseLen, &pl[0]);
    // Create string from the vector. This is in UTF8.
    std::string pl_utf8((char*)pl.data(), pl.size());
    auto pl_utf16 = officeutils::utf8_to_utf16(pl_utf8);
    publishingLicense.clear();
    publishingLicense.assign(
        reinterpret_cast<const unsigned char*>(pl_utf16.data()),
        reinterpret_cast<const unsigned char*>(pl_utf16.data()) +  pl_utf16.size() * 2);
    officeutils::AlignInputAtFourBytes(stm, publishingLicenseLen);
  }
}

} // namespace officeprotector
} // namespace rmscore
