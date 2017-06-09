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
#include "../common/CommonTypes.h"
#include "../../Core/ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

PFileProtector::PFileProtector(const std::string& originalFileExtension)
{
    m_originalFileExtension = originalFileExtension;
}

PFileProtector::~PFileProtector()
{
}

void PFileProtector::ProtectWithTemplate(const std::shared_ptr<std::fstream>& inputStream,
                                         const modernapi::TemplateDescriptor& templateDescriptor,
                                         const std::string& userId,
                                         modernapi::IAuthenticationCallback &authenticationCallback,
                                         modernapi::UserPolicyCreationOptions options,
                                         const modernapi::AppDataHashMap& signedAppData,
                                         const std::shared_ptr<std::fstream>& outputStream,
                                         std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::ProtectWithTemplate");
    if(!inputStream->is_open() || !outputStream->is_open())
    {
        Logger::Error("Input/Output stream invalid");
        throw exceptions::RMSStreamException("Input/Output stream invalid");
    }

    if(IsProtected(inputStream))
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSPFileException("File is already protected",
                                            exceptions::RMSPFileException::AlreadyProtected);
    }

    m_userPolicy = modernapi::UserPolicy::CreateFromTemplateDescriptor(templateDescriptor,
                                                                       userId,
                                                                       authenticationCallback,
                                                                       options,
                                                                       signedAppData,
                                                                       cancelState);
    Protect(inputStream, outputStream);
    Logger::Hidden("-PFileProtector::ProtectWithTemplate");
}

void PFileProtector::ProtectWithCustomRights(const std::shared_ptr<std::fstream>& inputStream,
                                             const modernapi::PolicyDescriptor &policyDescriptor,
                                             const std::string& userId,
                                             modernapi::IAuthenticationCallback& authenticationCallback,
                                             modernapi::UserPolicyCreationOptions options,
                                             const std::shared_ptr<std::fstream>& outputStream,
                                             std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::ProtectWithCustomRights");
    if(!inputStream->is_open() || !outputStream->is_open())
    {
        Logger::Error("Input/Output stream invalid");
        throw exceptions::RMSStreamException("Input/Output stream invalid");
    }

    if(IsProtected(inputStream))
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSPFileException("File is already protected",
                                            exceptions::RMSPFileException::AlreadyProtected);
    }

    m_userPolicy = modernapi::UserPolicy::Create(
                const_cast<modernapi::PolicyDescriptor&>(policyDescriptor),
                userId,
                authenticationCallback,
                options,
                cancelState);
    Protect(inputStream, outputStream);
    Logger::Hidden("-PFileProtector::ProtectWithCustomRights");
}

UnprotectStatus PFileProtector::Unprotect(const std::shared_ptr<std::fstream>& inputStream,
                                          const std::string& userId,
                                          modernapi::IAuthenticationCallback& authenticationCallBack,
                                          modernapi::IConsentCallback& consentCallBack,
                                          const bool& isOffline,
                                          const bool& useCache,
                                          const std::shared_ptr<std::fstream>& outputStream,
                                          std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::UnProtect");
    if(!inputStream->is_open() || !outputStream->is_open())
    {
        Logger::Error("Input/Output stream invalid");
        throw exceptions::RMSStreamException("Input/Output stream invalid");
    }

    std::shared_ptr<std::iostream> inputIO = inputStream;
    auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(inputIO);

    modernapi::PolicyAcquisitionOptions policyAcquisitionOptions = isOffline?
                modernapi::PolicyAcquisitionOptions::POL_OfflineOnly :
                modernapi::PolicyAcquisitionOptions::POL_None;
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

    auto cacheMask = modernapi::RESPONSE_CACHE_NOCACHE;
    if(useCache)
    {
        cacheMask = static_cast<modernapi::ResponseCacheFlags>(modernapi::RESPONSE_CACHE_INMEMORY|
                                                               modernapi::RESPONSE_CACHE_ONDISK |
                                                               modernapi::RESPONSE_CACHE_CRYPTED);
    }
    auto policyRequest = modernapi::UserPolicy::Acquire(header->GetPublishingLicense(),
                                                        userId,
                                                        authenticationCallBack,
                                                        &consentCallBack,
                                                        policyAcquisitionOptions,
                                                        cacheMask,
                                                        cancelState);
    if(policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success)
    {
        Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
        throw exceptions::RMSPFileException("The file is corrupt",
                                            exceptions::RMSPFileException::CorruptFile);
    }

    m_userPolicy = policyRequest->Policy;
    if(m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy acquisition failed");
        throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    }

    auto protectedStream = CreateProtectedStream(inputSharedStream, header);
    DecryptStream(outputStream, protectedStream, header->GetOriginalFileSize());

    Logger::Hidden("+PFileProtector::UnProtect");
    return (UnprotectStatus)policyRequest->Status;
}

bool PFileProtector::IsProtected(const std::shared_ptr<std::fstream>& inputStream)
{
    Logger::Hidden("+PFileProtector::IsProtected");
    if(!inputStream->is_open())
    {
        Logger::Error("Input stream invalid");
        throw exceptions::RMSStreamException("Input stream invalid");
    }

    std::shared_ptr<std::iostream> inputIO = inputStream;
    auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(inputIO);
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

void PFileProtector::Protect(const std::shared_ptr<std::fstream>& inputStream,
                             const std::shared_ptr<std::fstream>& outputStream)
{
    if(m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy creation failed");
        throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
    }

    std::shared_ptr<std::iostream> outputIO = outputStream;
    auto outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(outputIO);
    //std::string ext = m_originalFileExtension.empty() ? ".pfile" : m_originalFileExtension;

    inputStream->seekg(0, std::ios::end);
    uint64_t originalFileSize = inputStream->tellg();
    //Write Header
    auto header = WriteHeader(outputSharedStream, originalFileSize);
    auto protectedStream = CreateProtectedStream(outputSharedStream, header);
    EncryptStream(inputStream, protectedStream, originalFileSize);
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
    uint64_t protectedStreamBlockSize = cryptoProvider->GetBlockSize() == 512 ? 512 : 4096;
    auto contentStartPosition = header->GetContentStartPosition();
    auto backingStreamImpl = stream->Clone();
    return rmscrypto::api::BlockBasedProtectedStream::Create(cryptoProvider,
                                                             backingStreamImpl,
                                                             contentStartPosition,
                                                             stream->Size() - contentStartPosition,
                                                             protectedStreamBlockSize);
}

void PFileProtector::EncryptStream(
        const std::shared_ptr<std::fstream>& stdStream,
        const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
        uint64_t originalFileSize)
{
    uint64_t bufSize = 4096;    //should be a multiple of AES block size (16)
    std::vector<uint8_t> buffer(bufSize);
    uint64_t readPosition  = 0;
    uint64_t writePosition = 0;
    bool isECB = m_userPolicy->DoesUseDeprecatedAlgorithms();
    uint64_t totalSize = isECB? ((originalFileSize + m_blockSize - 1) & ~(m_blockSize - 1)) :
                                originalFileSize;
    while(totalSize - readPosition > 0)
    {
        uint64_t offsetRead  = readPosition;
        uint64_t offsetWrite = writePosition;
        uint64_t toProcess   = std::min(bufSize, totalSize - readPosition);
        readPosition  += toProcess;
        writePosition += toProcess;

        stdStream->seekg(offsetRead);
        stdStream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);

        auto written = pStream->WriteAsync(
                    buffer.data(), toProcess, offsetWrite, std::launch::deferred).get();

        if (written != toProcess)
        {
          throw exceptions::RMSStreamException("Error while writing data");
        }
    }    
    pStream->Flush();
}

void PFileProtector::DecryptStream(const std::shared_ptr<std::fstream> &stdStream,
        const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
        uint64_t originalFileSize)
{
    uint64_t bufSize = 4096;    //should be a multiple of AES block size (16)
    std::vector<uint8_t> buffer(bufSize);
    uint64_t readPosition  = 0;
    uint64_t writePosition = 0;
    uint64_t totalSize = pStream->Size();
    while(totalSize - readPosition > 0)
    {
        uint64_t offsetRead  = readPosition;
        uint64_t offsetWrite = writePosition;
        uint64_t toProcess   = std::min(bufSize, totalSize - readPosition);
        uint64_t originalRemaining = std::min(bufSize, originalFileSize - readPosition);
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
        const rmscrypto::api::SharedStream& stream)
{
    std::shared_ptr<pfile::PfileHeader> header = nullptr;
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

} // namespace fileapi
} // namespace rmscore
