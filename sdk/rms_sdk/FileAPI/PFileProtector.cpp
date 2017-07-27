/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PFileProtector.h"
#include <algorithm>
#include <fstream>
#include <vector>
#include "BlockBasedProtectedStream.h"
#include "CryptoAPI.h"
#include "RMSExceptions.h"
#include "PfileHeader.h"
#include "PfileHeaderReader.h"
#include "PfileHeaderWriter.h"
#include "OfficeUtils.h"
#include "../Common//CommonTypes.h"
#include "../Core/ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

PFileProtector::PFileProtector(const std::string& originalFileExtension,
                               std::shared_ptr<std::istream> inputStream)
    : m_originalFileExtension(originalFileExtension),
      m_inputStream(inputStream)
{
}

PFileProtector::~PFileProtector()
{
}

void PFileProtector::ProtectWithTemplate(const UserContext& userContext,
                                         const ProtectWithTemplateOptions& options,
                                         std::shared_ptr<std::ostream> outputStream,
                                         std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::ProtectWithTemplate");
    if (!outputStream->good())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    auto inputFileSize = GetFileSize(m_inputStream.get(), MAX_FILE_SIZE_FOR_ENCRYPT);
    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSPFileException("File is already protected",
                                            exceptions::RMSPFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    m_userPolicy = modernapi::UserPolicy::CreateFromTemplateDescriptor(options.templateDescriptor,
                                                                       userContext.userId,
                                                                       userContext.authenticationCallback,
                                                                       userPolicyCreationOptions,
                                                                       options.signedAppData,
                                                                       cancelState);
    ProtectInternal(outputStream, inputFileSize);
    Logger::Hidden("-PFileProtector::ProtectWithTemplate");
}

void PFileProtector::ProtectWithCustomRights(const UserContext& userContext,
                                             const ProtectWithCustomRightsOptions& options,
                                             std::shared_ptr<std::ostream> outputStream,
                                             std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::ProtectWithCustomRights");
    if (!outputStream->good())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    auto inputFileSize = GetFileSize(m_inputStream.get(), MAX_FILE_SIZE_FOR_ENCRYPT);
    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSPFileException("File is already protected",
                                            exceptions::RMSPFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    m_userPolicy = modernapi::UserPolicy::Create(
                const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
                userContext.userId,
                userContext.authenticationCallback,
                userPolicyCreationOptions,
                cancelState);
    ProtectInternal(outputStream, inputFileSize);
    Logger::Hidden("-PFileProtector::ProtectWithCustomRights");
}

UnprotectResult PFileProtector::Unprotect(const UserContext& userContext,
                                          const UnprotectOptions& options,
                                          std::shared_ptr<std::ostream> outputStream,
                                          std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::UnProtect");
    if (!outputStream->good())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    GetFileSize(m_inputStream.get(), MAX_FILE_SIZE_FOR_ENCRYPT);
    auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(m_inputStream);
    std::shared_ptr<pfile::PfileHeader> header = nullptr;
    try
    {
      header =  ReadHeader(inputSharedStream);
    }
    catch (exceptions::RMSException&)
    {
      Logger::Hidden("Failed to read header");
      Logger::Hidden("-PFileProtector::UnProtect");
      throw;
    }

    modernapi::PolicyAcquisitionOptions policyAcquisitionOptions = options.offlineOnly?
                modernapi::PolicyAcquisitionOptions::POL_OfflineOnly :
                modernapi::PolicyAcquisitionOptions::POL_None;
    auto cacheMask = modernapi::RESPONSE_CACHE_NOCACHE;
    if (options.useCache)
    {
        cacheMask = static_cast<modernapi::ResponseCacheFlags>(modernapi::RESPONSE_CACHE_INMEMORY|
                                                               modernapi::RESPONSE_CACHE_ONDISK |
                                                               modernapi::RESPONSE_CACHE_CRYPTED);
    }
    auto policyRequest = modernapi::UserPolicy::Acquire(header->GetPublishingLicense(),
                                                        userContext.userId,
                                                        userContext.authenticationCallback,
                                                        &userContext.consentCallback,
                                                        policyAcquisitionOptions,
                                                        cacheMask,
                                                        cancelState);
    if (policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success)
    {
        Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
        throw exceptions::RMSPFileException("The file is corrupt",
                                            exceptions::RMSPFileException::CorruptFile);
    }

    m_userPolicy = policyRequest->Policy;
    if (m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy acquisition failed");
        throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    }

    auto protectedStream = CreateProtectedStream(inputSharedStream, header);
    DecryptStream(outputStream, protectedStream, header->GetOriginalFileSize());

    Logger::Hidden("+PFileProtector::UnProtect");
    return (UnprotectResult)policyRequest->Status;
}

bool PFileProtector::IsProtected() const
{
    Logger::Hidden("+PFileProtector::IsProtected");
    auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(m_inputStream);
    try
    {
      ReadHeader(inputSharedStream);
    }
    catch (exceptions::RMSException&)
    {
      Logger::Hidden("-PFileProtector::IsProtected");
      return false;
    }

    Logger::Hidden("-PFileProtector::IsProtected");
    return true;
}

void PFileProtector::ProtectInternal(const std::shared_ptr<std::ostream>& outputStream,
                                     uint64_t inputFileSize)
{
    if (m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy creation failed");
        throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
    }

    auto outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(outputStream);

    //Write Header
    auto header = WriteHeader(outputSharedStream, inputFileSize);
    auto protectedStream = CreateProtectedStream(outputSharedStream, header);
    EncryptStream(protectedStream, inputFileSize);
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> PFileProtector::CreateProtectedStream(
        const rmscrypto::api::SharedStream& stream,
        const std::shared_ptr<pfile::PfileHeader>& header)
{
    auto protectionPolicy = m_userPolicy->GetImpl();
    if ((protectionPolicy->GetCipherMode() == rmscrypto::api::CipherMode::CIPHER_MODE_ECB  ) &&
            (header->GetMajorVersion() <= rmscore::pfile::MaxMajorVerionsCBC4KIsForced))
    {
        // Older versions of the SDK ignored ECB cipher mode when encrypting pfile format.
        protectionPolicy->ReinitilizeCryptoProvider(rmscrypto::api::CipherMode::CIPHER_MODE_CBC4K);
    }

    auto cryptoProvider = m_userPolicy->GetImpl()->GetCryptoProvider();
    m_blockSize = cryptoProvider->GetBlockSize();
    // Cache block size to be 512 for cbc512, 4096 for cbc4k and ecb
    uint64_t protectedStreamBlockSize = m_blockSize == 512 ? 512 : 4096;
    auto contentStartPosition = header->GetContentStartPosition();
    auto backingStreamImpl = stream->Clone();
    return rmscrypto::api::BlockBasedProtectedStream::Create(cryptoProvider,
                                                             backingStreamImpl,
                                                             contentStartPosition,
                                                             stream->Size() - contentStartPosition,
                                                             protectedStreamBlockSize);
}

void PFileProtector::EncryptStream(
        const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
        uint64_t inputFileSize)
{
    std::vector<uint8_t> buffer(BUF_SIZE);
    uint64_t readPosition  = 0;
    uint64_t writePosition = 0;
    bool isECB = m_userPolicy->DoesUseDeprecatedAlgorithms();
    uint64_t totalSize = isECB? ((inputFileSize + m_blockSize - 1) & ~(m_blockSize - 1)) :
                                inputFileSize;
    while(totalSize - readPosition > 0)
    {
        uint64_t offsetRead  = readPosition;
        uint64_t offsetWrite = writePosition;
        uint64_t toProcess   = std::min(BUF_SIZE, totalSize - readPosition);
        readPosition  += toProcess;
        writePosition += toProcess;

        m_inputStream->seekg(offsetRead);
        m_inputStream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);

        auto written = pStream->WriteAsync(
                    buffer.data(), toProcess, offsetWrite, std::launch::deferred).get();

        if (written != toProcess)
        {
          throw exceptions::RMSStreamException("Error while writing data");
        }
    }    
    pStream->Flush();
}

void PFileProtector::DecryptStream(
        const std::shared_ptr<std::ostream>& stdStream,
        const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
        uint64_t originalFileSize)
{
    std::vector<uint8_t> buffer(BUF_SIZE);
    uint64_t readPosition  = 0;
    uint64_t writePosition = 0;
    uint64_t totalSize = pStream->Size();
    while(totalSize - readPosition > 0)
    {
        uint64_t offsetRead  = readPosition;
        uint64_t offsetWrite = writePosition;
        uint64_t toProcess   = std::min(BUF_SIZE, totalSize - readPosition);
        uint64_t originalRemaining = std::min(BUF_SIZE, originalFileSize - readPosition);
        readPosition  += toProcess;
        writePosition += toProcess;

        auto read = pStream->ReadAsync(
                    &buffer[0], toProcess, offsetRead, std::launch::deferred).get();
        if (read == 0)
        {
          break;
        }

        stdStream->seekp(offsetWrite);
        stdStream->write(reinterpret_cast<const char *>(buffer.data()), originalRemaining);
    }
    stdStream->flush();
}

std::shared_ptr<pfile::PfileHeader> PFileProtector::WriteHeader(
        const rmscrypto::api::SharedStream& stream,
        uint64_t originalFileSize)
{
    std::shared_ptr<pfile::PfileHeader> pHeader;
    auto headerWriter = pfile::IPfileHeaderWriter::Create();
    auto publishingLicense = m_userPolicy->SerializedPolicy();
    common::ByteArray metadata; // No metadata as of now.

    // calculate content size
    uint32_t contentStartPosition = static_cast<uint32_t>(m_originalFileExtension.size() +
                                                          publishingLicense.size() +
                                                          metadata.size() + 454);
    pHeader = std::make_shared<pfile::PfileHeader>(move(publishingLicense),
                                                   m_originalFileExtension,
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

modernapi::UserPolicyCreationOptions PFileProtector::ConvertToUserPolicyCreationOptions(
        const bool& allowAuditedExtraction,
        CryptoOptions cryptoOptions)
{
    auto userPolicyCreationOptions = allowAuditedExtraction ?
                modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
                modernapi::UserPolicyCreationOptions::USER_None;
    if (cryptoOptions == CryptoOptions::AES128_ECB )
    {
        userPolicyCreationOptions = static_cast<modernapi::UserPolicyCreationOptions>(
                    userPolicyCreationOptions |
                    modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
    }
    return userPolicyCreationOptions;
}

} // namespace fileapi
} // namespace rmscore
