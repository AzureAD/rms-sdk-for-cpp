/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "utils.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include "RMSExceptions.h"
#include "../Common/CommonTypes.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;
using namespace rmscore::common;

namespace rmscore {
namespace utils {

modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptionsForPfile(
        const bool& allowAuditedExtraction,
        CryptoOptions cryptoOptions) {
  auto userPolicyCreationOptions = allowAuditedExtraction ?
        modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
        modernapi::UserPolicyCreationOptions::USER_None;
  if (cryptoOptions == CryptoOptions::AES128_ECB) {
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

void CopyFromFileToOstream(std::string fileName, std::ostream* stream) {
  std::unique_ptr<FILE, FILE_deleter> file(fopen(fileName.c_str(), "r+b"));
  fseek(file.get(), 0L, SEEK_END);
  uint64_t fileSize = ftell(file.get());
  rewind(file.get());
  stream->seekp(0);
  std::vector<uint8_t> buffer(BUF_SIZE_BYTES);
  auto count = fileSize;
  while (count > BUF_SIZE_BYTES) {
    fread(&buffer[0], BUF_SIZE_BYTES, 1, file.get());
    stream->write(reinterpret_cast<const char*>(buffer.data()), BUF_SIZE_BYTES);
    count -= BUF_SIZE_BYTES;
  }
  fread((&buffer[0]), count, 1, file.get());
  stream->write(reinterpret_cast<const char*>(buffer.data()), count);
  stream->flush();
}

void CopyFromIstreamToFile(
    std::istream* stream,
    const std::string& tempFileName,
    uint64_t inputFileSize) {
  std::unique_ptr<FILE, FILE_deleter> tempFile(fopen(tempFileName.c_str(), "w+b"));
  stream->seekg(0);
  fseek(tempFile.get(), 0L, SEEK_SET);
  std::vector<uint8_t> buffer(BUF_SIZE_BYTES);
  auto count = inputFileSize;
  while (count > BUF_SIZE_BYTES) {
    stream->read(reinterpret_cast<char *>(&buffer[0]), BUF_SIZE_BYTES);
    fwrite(reinterpret_cast<const char*>(buffer.data()), BUF_SIZE_BYTES, 1, tempFile.get());
    count -= BUF_SIZE_BYTES;
  }
  stream->read(reinterpret_cast<char *>(&buffer[0]), count);
  fwrite(reinterpret_cast<const char*>(buffer.data()), count, 1, tempFile.get());
  fflush(tempFile.get());
}

std::string CreateTemporaryFileName(const std::string& fileName) {
  srand(time(NULL));
  uint32_t random = rand() % 10000;
  return (fileName + std::to_string(random) + ".tmp");
}

uint64_t ValidateAndGetFileSize(std::istream* stream, uint64_t maxFileSize) {
  stream->seekg(0, std::ios::end);
  uint64_t fileSize = stream->tellg();
  stream->seekg(0);
  if (maxFileSize < fileSize) {
    Logger::Error("Input file too large");
    throw exceptions::RMSLogicException(
          exceptions::RMSLogicException::ErrorTypes::NotSupported,
          "The file is too large. The limit is 1GB for encryption and 3GB for decryption");
  }
  return fileSize;
}

uint64_t ValidateAndGetFileSize(FILE* file, uint64_t maxFileSize) {
  fseek(file, 0L, SEEK_END);
  uint64_t fileSize = ftell(file);
  rewind(file);
  if (maxFileSize < fileSize) {
    Logger::Error("Input file too large");
    throw exceptions::RMSLogicException(
          exceptions::RMSLogicException::ErrorTypes::NotSupported,
          "The file is too large. The limit is 1GB for encryption and 3GB for decryption");
  }
  return fileSize;
}

} // namespace utils
} // namespace rmscore
