/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "metro_office_protector.h"
#include <sstream>
#include <vector>
#include "CryptoAPI.h"
#include "RMSExceptions.h"
#include "CommonTypes.h"
#include "ProtectionPolicy.h"
#include "utils.h"
#include "Logger.h"
#include "data_spaces.h"
#include "office_utils.h"

using namespace rmscore::platform::logger;
using namespace rmscore::common;

namespace{
const char kMetroContent[] = "EncryptedPackage";
} // namespace

namespace rmscore {
namespace officeprotector {

MetroOfficeProtector::MetroOfficeProtector(
    std::string fileName,
    std::shared_ptr<std::istream> inputStream)
  : mFileName(fileName),
    mInputStream(inputStream) {
  utils::CheckStream(inputStream.get(), "Invalid input stream");
  gsf_init();
}

MetroOfficeProtector::~MetroOfficeProtector() {
}

void MetroOfficeProtector::ProtectWithTemplate(
    const UserContext& userContext,
    const ProtectWithTemplateOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+MetroOfficeProtector::ProtectWithTemplate");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  // Check if the file is protected first. This is done by checking presence of license in the file.
  if (IsProtected()) {
    Logger::Error("File is already protected");
    throw exceptions::RMSOfficeFileException(
        "File is already protected",
        exceptions::RMSOfficeFileException::Reason::AlreadyProtected);
  }
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_ENCRYPT);
  auto userPolicyCreationOptions = officeutils::ConvertToUserPolicyCreationOptionsForOffice(
      options.allowAuditedExtraction,
      options.cryptoOptions);
  // Create User Policy
  mUserPolicy = modernapi::UserPolicy::CreateFromTemplateDescriptor(
      options.templateDescriptor,
      userContext.userId,
      userContext.authenticationCallback,
      userPolicyCreationOptions,
      options.signedAppData,
      cancelState);
  // Creating and using temporary file to store encrypted content and license. This is done since
  // LibGsf doesn't support C++ iostreams.
  std::string outputTempFileName = utils::CreateTemporaryFileName(mFileName);
  // The custom deleter deletes the temporary file once the unique ptr to its name goes out of scope.
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> outputTempFile(&outputTempFileName);
  try {
    ProtectInternal(outputTempFileName, inputFileSize);
    // Copy contents of the temporary file to the output stream provided.
    utils::CopyFromFileToOstream(outputTempFileName, outputStream.get());
  } catch (std::exception&) {
    Logger::Hidden("-MetroOfficeProtector::ProtectWithTemplate");
    throw;
  }
  Logger::Hidden("-MetroOfficeProtector::ProtectWithTemplate");
}

void MetroOfficeProtector::ProtectWithCustomRights(
    const UserContext& userContext,
    const ProtectWithCustomRightsOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+MetroOfficeProtector::ProtectWithCustomRights");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  // Check if the file is protected first. This is done by checking presence of license in the file.
  if (IsProtected()) {
    Logger::Error("File is already protected");
    throw exceptions::RMSOfficeFileException(
        "File is already protected",
        exceptions::RMSOfficeFileException::Reason::AlreadyProtected);
  }
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_ENCRYPT);
  auto userPolicyCreationOptions = officeutils::ConvertToUserPolicyCreationOptionsForOffice(
      options.allowAuditedExtraction,
      options.cryptoOptions);
  // Create User Policy
  mUserPolicy = modernapi::UserPolicy::Create(
      const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
      userContext.userId,
      userContext.authenticationCallback,
      userPolicyCreationOptions,
      cancelState);
  // Creating and using temporary file to store encrypted content and license. This is done since
  // LibGsf doesn't support C++ iostreams.
  std::string outputTempFileName = utils::CreateTemporaryFileName(mFileName);
  // The custom deleter deletes the temporary file once the unique ptr to its name goes out of scope.
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> outputTempFile(&outputTempFileName);
  try {
    ProtectInternal(outputTempFileName, inputFileSize);
    // Copy contents of the temporary file to the output stream provided.
    utils::CopyFromFileToOstream(outputTempFileName, outputStream.get());
  } catch (std::exception&) {
    Logger::Hidden("-MetroOfficeProtector::ProtectWithCustomRights");
    throw;
  }
  Logger::Hidden("-MetroOfficeProtector::ProtectWithCustomRights");
}

UnprotectResult MetroOfficeProtector::Unprotect(
    const UserContext& userContext,
    const UnprotectOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+MetroOfficeProtector::UnProtect");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_DECRYPT);
  auto result = UnprotectResult::NORIGHTS;
  // Creating and using temporary file to store the input encrypted file. This is done since
  // LibGsf doesn't support C++ iostreams.
  std::string inputTempFileName = utils::CreateTemporaryFileName(mFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> inputTempFile(&inputTempFileName);
  try {
    result = UnprotectInternal(
        userContext,
        options,
        outputStream,
        inputTempFileName,
        inputFileSize,
        cancelState);
  } catch (std::exception&) {
    Logger::Hidden("-MetroOfficeProtector::UnProtect");
    throw;
  }
  Logger::Hidden("-MetroOfficeProtector::UnProtect");
  return result;
}

UnprotectResult MetroOfficeProtector::UnprotectInternal(
    const UserContext& userContext,
    const UnprotectOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::string inputTempFileName,
    uint64_t inputFileSize,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  // Copy from input file (which is encrypted) to a temporary file. This file will then be opened as
  // a Compound File through LibGsf
  utils::CopyFromIstreamToFile(mInputStream.get(), inputTempFileName, inputFileSize);
  std::unique_ptr<GsfInfile, GsfInfile_deleter> stg;
  try {
    std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
        gsf_input_stdio_new(inputTempFileName.c_str(), nullptr));
    officeutils::CheckGsfInput(gsfInputStdIO.get());
    stg.reset(gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
  } catch (std::exception&) {
    Logger::Hidden("Failed to open file as a valid Compound File");
    throw;
  }
  ByteArray publishingLicense;
  try {
    // Read the license from the encrypted file.
    auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(true, true);
    dataSpaces->ReadDataSpaces(stg.get(), publishingLicense);
  } catch (std::exception&) {
    Logger::Hidden("Failed to read dataspaces");
    throw;
  }
  modernapi::PolicyAcquisitionOptions policyAcquisitionOptions = options.offlineOnly?
      modernapi::PolicyAcquisitionOptions::POL_OfflineOnly :
      modernapi::PolicyAcquisitionOptions::POL_None;
  auto cacheMask = modernapi::RESPONSE_CACHE_NOCACHE;
  if (options.useCache) {
    cacheMask = static_cast<modernapi::ResponseCacheFlags>(
        modernapi::RESPONSE_CACHE_INMEMORY|
        modernapi::RESPONSE_CACHE_ONDISK |
        modernapi::RESPONSE_CACHE_CRYPTED);
  }
  // Acquire User Policy
  auto policyRequest = modernapi::UserPolicy::Acquire(
      publishingLicense,
      userContext.userId,
      userContext.authenticationCallback,
      &userContext.consentCallback,
      policyAcquisitionOptions,
      cacheMask,
      cancelState);
  if (policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success) {
    Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
    throw exceptions::RMSOfficeFileException(
        "The file has been corrupted",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  mUserPolicy = policyRequest->Policy;
  if (mUserPolicy.get() == nullptr) {
    Logger::Error("User Policy acquisition failed");
    throw exceptions::RMSOfficeFileException(
        "User Policy acquisition failed.",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }

  if (!mUserPolicy->DoesUseDeprecatedAlgorithms()) {
    throw exceptions::RMSLogicException(
        exceptions::RMSException::ErrorTypes::NotSupported,
        "CBC Decryption with Office files is not yet supported");
  }
  // This stream contains the entire file encrypted as binary.
  std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> metroStream(
      gsf_infile_child_by_name(stg.get(), kMetroContent));
  officeutils::CheckGsfInput(metroStream.get());
  uint64_t originalFileSize = 0;
  officeutils::ReadStreamHeader(metroStream.get(), originalFileSize);
  DecryptStream(outputStream, metroStream.get(), originalFileSize);
  return (UnprotectResult)policyRequest->Status;
}

bool MetroOfficeProtector::IsProtected() const {
  Logger::Hidden("+MetroOfficeProtector::IsProtected");
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_DECRYPT);
  std::string inputTempFileName = utils::CreateTemporaryFileName(mFileName);
  std::unique_ptr<utils::TempFileName, utils::TempFile_deleter> inputTempFile(&inputTempFileName);
  bool isProtected = officeutils::IsProtectedInternal(
      mInputStream.get(),
      inputTempFileName,
      inputFileSize,
      true);
  Logger::Hidden("-MetroOfficeProtector::IsProtected");
  return isProtected;
}

void MetroOfficeProtector::ProtectInternal(
    std::string outputTempFileName,
    uint64_t originalFileSize) {
  if (mUserPolicy.get() == nullptr) {
    Logger::Error("User Policy creation failed");
    throw exceptions::RMSOfficeFileException(
        "User Policy creation failed.",
        exceptions::RMSOfficeFileException::Reason::BadArguments);
  }
  std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> gsfOutputStdIO(
      gsf_output_stdio_new(outputTempFileName.c_str(), nullptr));
  officeutils::CheckGsfOutput(gsfOutputStdIO.get());
  std::unique_ptr<GsfOutfile, officeprotector::GsfOutfile_deleter> stg(
      gsf_outfile_msole_new(gsfOutputStdIO.get()));
  {
    // Write Dataspaces
    auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(
        true,
        mUserPolicy->DoesUseDeprecatedAlgorithms());
    auto publishingLicense = mUserPolicy->SerializedPolicy();
    dataSpaces->WriteDataSpaces(stg.get(), publishingLicense);
  }
  std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> metroStream(
        gsf_outfile_new_child(stg.get(), kMetroContent, false));
  // Stream header currently contains only the size of the unencrypted content.
  officeutils::WriteStreamHeader(metroStream.get(), originalFileSize);  
  EncryptStream(metroStream.get(), originalFileSize);
}

void MetroOfficeProtector::EncryptStream(GsfOutput* metroStream, uint64_t inputFileSize) {
  officeutils::CheckGsfOutput(metroStream);
  auto cryptoProvider = mUserPolicy->GetImpl()->GetCryptoProvider();
  mBlockSize = cryptoProvider->GetBlockSize();
  std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
  uint64_t readPosition  = 0;
  bool isECB = mUserPolicy->DoesUseDeprecatedAlgorithms();
  // Total size represents the size of the encrypted content. in case of ECB, padding is added in
  // the end to round the size up to the block size using the bitmask.
  uint64_t totalSize = isECB? ((inputFileSize + mBlockSize - 1) & ~(mBlockSize - 1)) : inputFileSize;
  // Encrypt data in chunks of 4K bytes.
  while (totalSize - readPosition > 0) {
    uint64_t offsetRead  = readPosition;
    uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
    readPosition  += toProcess;

    // Using String Streams since the underlying protection APIs use streams instead of buffers.
    // Hence we create string streams and create protected stream from it.
    // TODO: modify this logic once we have buffer based APIs.
    auto sstream = std::make_shared<std::stringstream>();
    std::shared_ptr<std::iostream> iosstream = sstream;
    auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
    auto pStream = officeutils::CreateProtectedStream(sharedStringStream, 0, cryptoProvider);

    mInputStream->seekg(offsetRead);
    mInputStream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);
    pStream->WriteAsync(buffer.data(), toProcess, 0, std::launch::deferred).get();    
    pStream->Flush();
    // get the encrypted content as a string from the string stream.
    std::string encryptedData = sstream->str();
    auto encryptedDataLen = encryptedData.length();
    officeutils::GsfWrite(
        metroStream,
        encryptedDataLen,
        reinterpret_cast<const uint8_t*>(encryptedData.data()));
  }
}

void MetroOfficeProtector::DecryptStream(
    const std::shared_ptr<std::ostream>& stdStream,
    GsfInput* metroStream,
    uint64_t originalFileSize) {
  officeutils::CheckGsfInput(metroStream);
  auto cryptoProvider = mUserPolicy->GetImpl()->GetCryptoProvider();
  mBlockSize = cryptoProvider->GetBlockSize();
  std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
  uint64_t readPosition  = 0;
  uint64_t writePosition = 0;
  // Total size is the size of the encrypted content. The size of the unencrypted content is
  // written in the stream header.
  uint64_t totalSize = (uint64_t)gsf_input_size(metroStream) - sizeof(uint64_t);
  // Decrypt data in chunks of 4K bytes.
  while (totalSize - readPosition > 0) {
    uint64_t offsetWrite = writePosition;
    uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
    uint64_t originalRemaining = std::min(utils::BUF_SIZE_BYTES, originalFileSize - readPosition);
    readPosition  += toProcess;
    writePosition += toProcess;

    officeutils::GsfRead(metroStream, toProcess, &buffer[0]);
    std::string encryptedData(reinterpret_cast<const char *>(buffer.data()), toProcess);
    // Using String Streams since the underlying protection APIs use streams instead of buffers.
    // Hence we create string stream with encrypted content and create protected stream from it
    // to read.
    // TODO: modify this logic once we have buffer based APIs.
    auto sstream = std::make_shared<std::stringstream>();
    sstream->str(encryptedData);
    std::shared_ptr<std::iostream> iosstream = sstream;
    auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
    auto pStream = officeutils::CreateProtectedStream(
        sharedStringStream,
        encryptedData.length(),
        cryptoProvider);
    pStream->ReadAsync(&buffer[0], toProcess, 0, std::launch::deferred).get();

    stdStream->seekp(offsetWrite);
    // Write to standard output stream.
    stdStream->write(reinterpret_cast<const char *>(buffer.data()), originalRemaining);
  }
  stdStream->flush();
}

} // namespace officeprotector
} // namespace rmscore
