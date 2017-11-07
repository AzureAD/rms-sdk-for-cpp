/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "pfile_protector.h"
#include <vector>
#include "CryptoAPI.h"
#include "RMSExceptions.h"
#include "PfileHeaderReader.h"
#include "PfileHeaderWriter.h"
#include "CommonTypes.h"
#include "ProtectionPolicy.h"
#include "Logger.h"
#include "utils.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace pfile {

PFileProtector::PFileProtector(const std::string& originalFileExtension,
                               std::shared_ptr<std::istream> inputStream)
    : mOriginalFileExtension(originalFileExtension),
      mInputStream(inputStream) {
}

PFileProtector::~PFileProtector() {
}

void PFileProtector::ProtectWithTemplate(
    const UserContext& userContext,
    const ProtectWithTemplateOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+PFileProtector::ProtectWithTemplate");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  // Check if the file is protected first. This is done by checking presence of license in the file.
  if (IsProtected()) {
    Logger::Error("File is already protected");
    throw exceptions::RMSPFileException(
        "File is already protected",
        exceptions::RMSPFileException::Reason::AlreadyProtected);
  }
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_ENCRYPT);
  auto userPolicyCreationOptions = utils::ConvertToUserPolicyCreationOptionsForPfile(
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
  ProtectInternal(outputStream, inputFileSize);
  Logger::Hidden("-PFileProtector::ProtectWithTemplate");
}

void PFileProtector::ProtectWithCustomRights(
    const UserContext& userContext,
    const ProtectWithCustomRightsOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+PFileProtector::ProtectWithCustomRights");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }

  // Check if the file is protected first. This is done by checking presence of license in the file.
  if (IsProtected()) {
    Logger::Error("File is already protected");
    throw exceptions::RMSPFileException(
        "File is already protected",
        exceptions::RMSPFileException::Reason::AlreadyProtected);
  }
  auto inputFileSize = utils::ValidateAndGetFileSize(
      mInputStream.get(),
      utils::MAX_FILE_SIZE_FOR_ENCRYPT);
  auto userPolicyCreationOptions = utils::ConvertToUserPolicyCreationOptionsForPfile(
      options.allowAuditedExtraction,
      options.cryptoOptions);
  // Create User Policy
  mUserPolicy = modernapi::UserPolicy::Create(
      const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
      userContext.userId,
      userContext.authenticationCallback,
      userPolicyCreationOptions,
      cancelState);
  ProtectInternal(outputStream, inputFileSize);
  Logger::Hidden("-PFileProtector::ProtectWithCustomRights");
}

UnprotectResult PFileProtector::Unprotect(
    const UserContext& userContext,
    const UnprotectOptions& options,
    std::shared_ptr<std::ostream> outputStream,
    std::shared_ptr<std::atomic<bool>> cancelState) {
  Logger::Hidden("+PFileProtector::UnProtect");
  if (!outputStream->good()) {
    Logger::Error("Output stream invalid");
    throw exceptions::RMSStreamException("Output stream invalid");
  }
  utils::ValidateAndGetFileSize(mInputStream.get(), utils::MAX_FILE_SIZE_FOR_ENCRYPT);
  // Wraps a std stream into rmscrypto::api::IStream. This is later used to create protected
  // stream and read protected data from that stream.
  auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(mInputStream);
  std::shared_ptr<pfile::PfileHeader> header = nullptr;
  try {
    // Read header from PFile. If this operation fails, then file is either not protected or has
    // been corrupted.
    header =  ReadHeader(inputSharedStream);
  } catch (exceptions::RMSException&) {
    Logger::Hidden("Failed to read header");
    Logger::Hidden("-PFileProtector::UnProtect");
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
      header->GetPublishingLicense(),
      userContext.userId,
      userContext.authenticationCallback,
      &userContext.consentCallback,
      policyAcquisitionOptions,
      cacheMask,
      cancelState);
  if (policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success) {
    Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
    throw exceptions::RMSPFileException(
        "The file is corrupt",
        exceptions::RMSPFileException::Reason::CorruptFile);
  }
  mUserPolicy = policyRequest->Policy;
  if (!mUserPolicy) {
    Logger::Error("User Policy acquisition failed");
    throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
  }
  // Create protected stream from rmscrypto::api::SharedStream to read encrypted data
  auto protectedStream = CreateProtectedStream(inputSharedStream, header);
  DecryptStream(outputStream, protectedStream, header->GetOriginalFileSize());
  Logger::Hidden("+PFileProtector::UnProtect");
  return (UnprotectResult)policyRequest->Status;
}

bool PFileProtector::IsProtected() const {
  Logger::Hidden("+PFileProtector::IsProtected");
  auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(mInputStream);
  try {
    // If header is successfully read, then it means the file is protected. Else it is not protected
    // or has been corrupted .
    ReadHeader(inputSharedStream);
  } catch (exceptions::RMSException&) {
    Logger::Hidden("-PFileProtector::IsProtected");
    return false;
  }
  Logger::Hidden("-PFileProtector::IsProtected");
  return true;
}

void PFileProtector::ProtectInternal(
    const std::shared_ptr<std::ostream>& outputStream,
    uint64_t inputFileSize) {
  if (mUserPolicy.get() == nullptr) {
    Logger::Error("User Policy creation failed");
    throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
  }
  // Wraps a std stream into rmscrypto::api::IStream. This is later used to create protected
  // stream and write protected data into that stream.
  auto outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(outputStream);
  // Write Header
  auto header = WriteHeader(outputSharedStream, inputFileSize);
  // Create protected stream from rmscrypto::api::SharedStream to write encrypted data
  auto protectedStream = CreateProtectedStream(outputSharedStream, header);
  EncryptStream(protectedStream, inputFileSize);
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> PFileProtector::CreateProtectedStream(
    const rmscrypto::api::SharedStream& stream,
    const std::shared_ptr<pfile::PfileHeader>& header) {
  auto protectionPolicy = mUserPolicy->GetImpl();
  if ((protectionPolicy->GetCipherMode() == rmscrypto::api::CipherMode::CIPHER_MODE_ECB) &&
      (header->GetMajorVersion() <= rmscore::pfile::MaxMajorVerionsCBC4KIsForced)) {
    // Older versions of the SDK ignored ECB cipher mode when encrypting pfile format.
    protectionPolicy->ReinitilizeCryptoProvider(rmscrypto::api::CipherMode::CIPHER_MODE_CBC4K);
  }
  auto cryptoProvider = mUserPolicy->GetImpl()->GetCryptoProvider();
  mBlockSize = cryptoProvider->GetBlockSize();
  // Cache block size to be 512 for CBC512, 4096 for CBC4K and ECB.
  uint64_t protectedStreamBlockSize = mBlockSize == 512 ? 512 : 4096;
  auto contentStartPosition = header->GetContentStartPosition();
  auto backingStreamImpl = stream->Clone();
  return rmscrypto::api::BlockBasedProtectedStream::Create(
      cryptoProvider,
      backingStreamImpl,
      contentStartPosition,
      stream->Size() - contentStartPosition,
      protectedStreamBlockSize);
}

void PFileProtector::EncryptStream(
    const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
    uint64_t inputFileSize) {
  std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
  uint64_t readPosition  = 0;
  uint64_t writePosition = 0;
  bool isECB = mUserPolicy->DoesUseDeprecatedAlgorithms();
  uint64_t totalSize = isECB? ((inputFileSize + mBlockSize - 1) & ~(mBlockSize - 1)) :
                              inputFileSize;
  // Encrypt data in chunks of 4K bytes.
  while (totalSize - readPosition > 0) {
    uint64_t offsetRead  = readPosition;
    uint64_t offsetWrite = writePosition;
    uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
    readPosition  += toProcess;
    writePosition += toProcess;

    mInputStream->seekg(offsetRead);
    mInputStream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);
    auto written = pStream->WriteAsync(
        buffer.data(),
        toProcess,
        offsetWrite,
        std::launch::deferred).get();
    if (written != toProcess) {
      throw exceptions::RMSStreamException("Error while writing data");
    }
  }
  pStream->Flush();
}

void PFileProtector::DecryptStream(
    const std::shared_ptr<std::ostream>& stdStream,
    const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
    uint64_t originalFileSize) {
  std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
  uint64_t readPosition  = 0;
  uint64_t writePosition = 0;
  uint64_t totalSize = pStream->Size();
  // Decrypt data in chunks of 4K bytes.
  while (totalSize - readPosition > 0) {
    uint64_t offsetRead  = readPosition;
    uint64_t offsetWrite = writePosition;
    uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
    uint64_t originalRemaining = std::min(utils::BUF_SIZE_BYTES, originalFileSize - readPosition);
    readPosition  += toProcess;
    writePosition += toProcess;

    auto read = pStream->ReadAsync(&buffer[0], toProcess, offsetRead, std::launch::deferred).get();
    if (read == 0) {
      break;
    }
    stdStream->seekp(offsetWrite);
    stdStream->write(reinterpret_cast<const char *>(buffer.data()), originalRemaining);
  }
  stdStream->flush();
}

std::shared_ptr<pfile::PfileHeader> PFileProtector::WriteHeader(
    const rmscrypto::api::SharedStream& stream,
    uint64_t originalFileSize) {
  std::shared_ptr<pfile::PfileHeader> pHeader;
  auto headerWriter = pfile::IPfileHeaderWriter::Create();
  auto publishingLicense = mUserPolicy->SerializedPolicy();
  common::ByteArray metadata; // No metadata as of now.

  // calculate content size
  uint32_t contentStartPosition = static_cast<uint32_t>(
      mOriginalFileExtension.size() + publishingLicense.size() + metadata.size() + 454);
  pHeader = std::make_shared<pfile::PfileHeader>(
      move(publishingLicense),
      mOriginalFileExtension,
      contentStartPosition,
      originalFileSize, //
      move(metadata),
      static_cast<uint32_t>(rmscore::pfile::MJVERSION_FOR_WRITING),
      static_cast<uint32_t>(rmscore::pfile::MNVERSION_FOR_WRITING),
      pfile::CleartextRedirectHeader);
  headerWriter->Write(stream, pHeader);
  stream->Flush();
  return pHeader;
}

std::shared_ptr<pfile::PfileHeader> PFileProtector::ReadHeader(
    const rmscrypto::api::SharedStream& stream) const
{
  std::shared_ptr<pfile::PfileHeader> header;
  auto headerReader = pfile::IPfileHeaderReader::Create();
  header  = headerReader->Read(stream);
  Logger::Hidden(
      "PFileProtector: ReadHeader. Major version: %d, minor version: %d, "
      "file extension: '%s', content start position: %d, original file size: %I64d",
      header->GetMajorVersion(),
      header->GetMinorVersion(),
      header->GetFileExtension().c_str(),
      header->GetContentStartPosition(),
      header->GetOriginalFileSize());
  return header;
}

} // namespace pfile
} // namespace rmscore
