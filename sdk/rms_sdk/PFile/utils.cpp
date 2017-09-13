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
#include "CommonTypes.h"
#include "Logger.h"

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
    // Default option for pfiles is CBC
    userPolicyCreationOptions = static_cast<modernapi::UserPolicyCreationOptions>(
          userPolicyCreationOptions |
          modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
  } else {
    // temporary until we have CBC for office files
    throw exceptions::RMSLogicException(
          exceptions::RMSException::ErrorTypes::NotSupported,
          "CBC Encryption with Office files is not yet supported");
  }
  return userPolicyCreationOptions;
}

void CopyFromFileToOstream(std::string fileName, std::ostream* stream) {
  CheckStream(stream, "The stream is invalid");
  std::unique_ptr<FILE, FILE_closer> file(fopen(fileName.c_str(), "r+b"));
  if (!file) {
    Logger::Hidden("Failed to open file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to open temporary file");
  }
  if (fseek(file.get(), 0L, SEEK_END) != 0) {
    Logger::Hidden("Failed to seek to the end of the file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to seek to the end of the file");
  }
  uint64_t fileSize = ftell(file.get());
  if (fileSize == -1) {
    Logger::Hidden("Failed to get current position in the file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to get current position in the file");
  }
  rewind(file.get());
  stream->seekp(0);
  CheckStream(stream, "Failed to seek to the start of the stream");
  std::vector<uint8_t> buffer(BUF_SIZE_BYTES);
  auto count = fileSize;
  auto toProcess = 0;
  // Copy in chunks of size BUF_SIZE_BYTES (4K)
  while (count > 0) {
    toProcess = std::min(BUF_SIZE_BYTES, count);
    if (fread(&buffer[0], 1, toProcess, file.get()) != toProcess) {
      Logger::Hidden("Failed to read from the file");
      throw exceptions::RMSLogicException(
          exceptions::RMSException::ErrorTypes::FileError,
          "Failed to read from the file");
    }
    stream->write(reinterpret_cast<const char*>(buffer.data()), toProcess);
    CheckStream(stream, "Failed to write to the stream");
    count -= toProcess;
  }
  stream->flush();
  CheckStream(stream, "Failed to flush the stream");
}

void CopyFromIstreamToFile(
    std::istream* stream,
    const std::string& fileName,
    uint64_t inputFileSize) {
  CheckStream(stream, "The stream is invalid");
  std::unique_ptr<FILE, FILE_closer> file(fopen(fileName.c_str(), "w+b"));
  if (!file) {
    Logger::Hidden("Failed to open file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to open file");
  }
  stream->seekg(0);
  CheckStream(stream, "Failed to seek to the start of the stream");
  if (fseek(file.get(), 0L, SEEK_SET) != 0) {
    Logger::Hidden("Failed to seek to the start of the file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to seek to the start of the file");
  }
  std::vector<uint8_t> buffer(BUF_SIZE_BYTES);
  auto count = inputFileSize;
  auto toProcess = 0;  
  // Copy in chunks of size BUF_SIZE_BYTES (4K)
  while (count > 0) {
    toProcess = std::min(BUF_SIZE_BYTES, count);
    stream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);
    CheckStream(stream, "Failed to read from the stream");
    if (fwrite(reinterpret_cast<const char*>(buffer.data()), 1, toProcess, file.get()) != toProcess) {
      Logger::Hidden("Failed to write to the file");
      throw exceptions::RMSLogicException(
          exceptions::RMSException::ErrorTypes::FileError,
          "Failed to write to the file");
    }
    count -= toProcess;
  }
  if (fflush(file.get()) != 0) {
    Logger::Hidden("Failed to flush the file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to flush the file");
  }
}

std::string CreateTemporaryFileName(const std::string& fileName) {
  srand(time(NULL));
  uint32_t random = rand() % 10000;
  return (fileName + std::to_string(random) + ".tmp");
}

uint64_t ValidateAndGetFileSize(std::istream* stream, uint64_t maxFileSize) {
  CheckStream(stream, "Invalid stream provided");
  stream->seekg(0, std::ios::end);
  CheckStream(stream, "Failed to seek to the end of the stream");
  uint64_t fileSize = stream->tellg();
  CheckStream(stream, "Failed to get current position in the stream");
  stream->seekg(0);
  CheckStream(stream, "Failed to seek to the start of the stream");
  if (maxFileSize < fileSize) {
    Logger::Error("Input file too large");
    throw exceptions::RMSLogicException(
          exceptions::RMSLogicException::ErrorTypes::NotSupported,
          "The file is too large. The limit is 1GB for encryption and 3GB for decryption");
  }
  return fileSize;
}

uint64_t ValidateAndGetFileSize(FILE* file, uint64_t maxFileSize) {
  if (fseek(file, 0L, SEEK_END) != 0) {
    Logger::Hidden("Failed to seek to the end of the file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to seek to the end of the file");
  }
  uint64_t fileSize = ftell(file);
  if (fileSize == -1) {
    Logger::Hidden("Failed to get current position in the file");
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::FileError,
        "Failed to get current position in the file");
  }
  rewind(file);
  if (maxFileSize < fileSize) {
    Logger::Error("Input file too large");
    throw exceptions::RMSLogicException(
          exceptions::RMSLogicException::ErrorTypes::NotSupported,
          "The file is too large. The limit is 1GB for encryption and 3GB for decryption");
  }
  return fileSize;
}

void CheckStream(std::ios* stream, std::string message) {
  if (!stream) {
    Logger::Hidden(message);
    throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::StreamError, message);
  }
}

} // namespace utils
} // namespace rmscore
