/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "office_utils.h"
#include <codecvt>
#include <locale>
#include "CryptoAPI.h"
#include "../Platform/Logger/Logger.h"
#include "../PFile/utils.h"
#include "RMSExceptions.h"
#include "data_spaces.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace officeutils {

// Known issue with Visual C++. Please refer
// https://connect.microsoft.com/VisualStudio/feedback/details/1348277/link-error-when-using-std-codecvt-utf8-utf16-char16-t

#if _MSC_VER >= 1900

std::string utf16_to_utf8(const std::u16string& utf16_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
  auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
  return convert.to_bytes(p, p + utf16_string.size());
}

std::u16string utf8_to_utf16(const std::string& utf8_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;

  auto str = convert.from_bytes(utf8_string);
  std::u16string result{ str.begin(), str.end() };
  return result;
}

#else

std::string utf16_to_utf8(const std::u16string& utf16_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(utf16_string);
}

std::u16string utf8_to_utf16(const std::string& utf8_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.from_bytes(utf8_string);
}

#endif

void WriteWideStringEntry(GsfOutput* stm, const std::string& entry) {
  if (stm == nullptr || entry.empty()) {
    Logger::Error("Invalid arguments provided for writing string entry");
    throw exceptions::RMSOfficeFileException(
          "Error in writing to stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  auto entry_utf16 = utf8_to_utf16(entry);
  uint32_t entry_utf16_len = entry_utf16.length() * 2;
  // Write length of the UTF16 string in bytes
  gsf_output_write(stm, sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&entry_utf16_len));
  gsf_output_write(stm, entry_utf16_len, reinterpret_cast<const uint8_t*>(entry_utf16.data()));
  AlignOutputAtFourBytes(stm, entry_utf16_len);
}

void ReadWideStringEntry(GsfInput* stm, std::string& entry) {
  if (stm == nullptr) {
    Logger::Error("Invalid arguments provided for reading string entry");
    throw exceptions::RMSOfficeFileException(
          "Error in reading from stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  uint32_t entry_utf16_len = 0;
  // Read length of the UTF16 string in bytes
  gsf_input_read(stm, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&entry_utf16_len));
  if (entry_utf16_len % 2 != 0) {
    Logger::Error("Corrupt doc file.");
    throw exceptions::RMSOfficeFileException(
          "Corrupt doc file",
          exceptions::RMSOfficeFileException::Reason::CorruptFile);
  }
  std::vector<uint8_t> ent(entry_utf16_len);
  gsf_input_read(stm, entry_utf16_len, &ent[0]);
  std::u16string entry_utf16(reinterpret_cast<const char16_t*>(ent.data()), (ent.size()+1)/2);
  entry = utf16_to_utf8(entry_utf16);
  AlignInputAtFourBytes(stm, entry_utf16_len);
}

uint32_t FourByteAlignedWideStringLength(const std::string& entry) {
  size_t len = sizeof(uint32_t) + (entry.length() << 1);
  // The bitmask is used to round up to the nearest multiple of 4.
  return ((len + 3) & ~3);
}

void AlignOutputAtFourBytes(GsfOutput* stm, uint32_t contentLength) {
  if (stm == nullptr || contentLength < 1) {
    Logger::Error("Invalid arguments provided for byte alignment");
    throw exceptions::RMSOfficeFileException(
          "Error in aligning stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  // The bitmask is used to round to the nearest multiple of 4.
  uint32_t alignCount = ((contentLength + 3) & ~3) - contentLength;
  std::string alignBytes;
  for(uint32_t i = 0; i < alignCount; i++)
    alignBytes.push_back('\0');
  // Write null chars to stream
  gsf_output_write(stm, alignCount, reinterpret_cast<const uint8_t*>(alignBytes.data()));
}

void AlignInputAtFourBytes(GsfInput* stm, uint32_t contentLength) {
  if (stm == nullptr || contentLength < 1) {
    Logger::Error("Invalid arguments provided for byte alignment");
    throw exceptions::RMSOfficeFileException(
          "Error in aligning stream",
          exceptions::RMSOfficeFileException::Reason::Unknown);
  }
  uint32_t alignCount = ((contentLength + 3) & ~3) - contentLength;
  uint64_t pos = gsf_input_tell(stm);
  // Seek ahead since these would be null chars
  gsf_input_seek(stm, pos + alignCount, G_SEEK_SET);
}

void WriteStreamHeader(GsfOutput* stm, const uint64_t& contentLength) {
  if (stm == nullptr) {
    Logger::Error("Invalid arguments provided for writing stream header");
    throw exceptions::RMSLogicException(
          exceptions::RMSException::ErrorTypes::StreamError,
          "Error in writing to stream");
  }
  gsf_output_seek(stm, 0, G_SEEK_SET);
  gsf_output_write(stm, sizeof(uint64_t), reinterpret_cast<const uint8_t*>(&contentLength));
}

void ReadStreamHeader(GsfInput* stm, uint64_t& contentLength)
{
  if (stm == nullptr) {
    Logger::Error("Invalid arguments provided for reading stream header");
    throw exceptions::RMSLogicException(
          exceptions::RMSException::ErrorTypes::StreamError,
          "Error in reading from stream");
  }
  gsf_input_seek(stm, 0, G_SEEK_SET);
  gsf_input_read(stm, sizeof(uint64_t), reinterpret_cast<uint8_t*>(&contentLength));
}

modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptionsForOffice(
    const bool& allowAuditedExtraction,
    CryptoOptions cryptoOptions) {  
  auto userPolicyCreationOptions = allowAuditedExtraction ?
        modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
        modernapi::UserPolicyCreationOptions::USER_None;
  if (cryptoOptions == CryptoOptions::AUTO || cryptoOptions == CryptoOptions::AES128_ECB) {
    // Default option for office files is ECB
    userPolicyCreationOptions = static_cast<modernapi::UserPolicyCreationOptions>(
          userPolicyCreationOptions |
          modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
  } else {      //temporary until we have CBC for office files
    throw exceptions::RMSLogicException(
          exceptions::RMSException::ErrorTypes::NotSupported,
          "CBC Encryption with Office files is not yet supported");
  }
  return userPolicyCreationOptions;
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
    const rmscrypto::api::SharedStream& stream,
    uint64_t streamSize,
    std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider) {
  // Cache block size to be 512 for cbc512, 4096 for cbc4k and ecb
  uint64_t protectedStreamBlockSize = cryptoProvider->GetBlockSize() == 512 ? 512 : 4096;
  return rmscrypto::api::BlockBasedProtectedStream::Create(
        cryptoProvider,
        stream,
        0,
        streamSize,
        protectedStreamBlockSize);
}

bool IsProtectedInternal(
    std::istream* inputStream,
    std::string inputTempFileName,
    uint64_t inputFileSize) {
  // Copy from input file (which is encrypted) to a temporary file. This file will then be opened as
  // a Compound File through LibGsf
  utils::CopyFromIstreamToFile(inputStream, inputTempFileName, inputFileSize);
  try {
    std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
          gsf_input_stdio_new(inputTempFileName.c_str(), nullptr));
    std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> stg(
          gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
    // Try to read the license from this file. If it contains a license, it is already protected.
    auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(true);
    ByteArray publishingLicense;
    dataSpaces->ReadDataSpaces(stg.get(), publishingLicense);
  } catch (std::exception& e) {
    // Even if the file is protected using non RMS technologies, we'll return true for IsProtected()
    if (static_cast<exceptions::RMSException&>(e).error() ==
        static_cast<int>(exceptions::RMSException::ErrorTypes::OfficeFileError) &&
        (static_cast<exceptions::RMSOfficeFileException&>(e).reason() ==
         exceptions::RMSOfficeFileException::Reason::NonRMSProtected)) {
      return true;
    } else {
      return false;
    }
  }
  return true;
}

} // namespace officeutils
} // namespace rmscore
