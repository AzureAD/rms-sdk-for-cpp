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
#include "../Common//CommonTypes.h"
#include "../Core/ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

PFileProtector::PFileProtector(const std::string& originalFileExtension,
                               std::shared_ptr<std::fstream> input_stream)
    : original_file_extension_(originalFileExtension),
      input_stream_(input_stream)
{
}

PFileProtector::~PFileProtector()
{
}

void PFileProtector::ProtectWithTemplate(const UserContext& userContext,
                                         const ProtectWithTemplateOptions& options,
                                         std::shared_ptr<std::fstream> output_stream,
                                         std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::ProtectWithTemplate");
    if (!output_stream->is_open())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSPFileException("File is already protected",
                                            exceptions::RMSPFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    user_policy_ = modernapi::UserPolicy::CreateFromTemplateDescriptor(options.templateDescriptor,
                                                                       userContext.userId,
                                                                       userContext.authenticationCallback,
                                                                       userPolicyCreationOptions,
                                                                       options.signedAppData,
                                                                       cancelState);
    Protect(output_stream);
    Logger::Hidden("-PFileProtector::ProtectWithTemplate");
}

void PFileProtector::ProtectWithCustomRights(const UserContext& userContext,
                                             const ProtectWithCustomRightsOptions& options,
                                             std::shared_ptr<std::fstream> output_stream,
                                             std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::ProtectWithCustomRights");
    if (!output_stream->is_open())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSPFileException("File is already protected",
                                            exceptions::RMSPFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    user_policy_ = modernapi::UserPolicy::Create(
                const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
                userContext.userId,
                userContext.authenticationCallback,
                userPolicyCreationOptions,
                cancelState);
    Protect(output_stream);
    Logger::Hidden("-PFileProtector::ProtectWithCustomRights");
}

UnprotectResult PFileProtector::Unprotect(const UserContext& userContext,
                                          const UnprotectOptions& options,
                                          std::shared_ptr<std::fstream> output_stream,
                                          std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PFileProtector::UnProtect");
    if (!output_stream->is_open())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    std::shared_ptr<std::iostream> inputIO = input_stream_;
    auto input_shared_stream = rmscrypto::api::CreateStreamFromStdStream(inputIO);    
    std::shared_ptr<pfile::PfileHeader> header = nullptr;
    try
    {
      header =  ReadHeader(input_shared_stream);
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

    user_policy_ = policyRequest->Policy;
    if (user_policy_.get() == nullptr)
    {
        Logger::Error("User Policy acquisition failed");
        throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    }

    auto protected_stream = CreateProtectedStream(input_shared_stream, header);
    DecryptStream(output_stream, protected_stream, header->GetOriginalFileSize());

    Logger::Hidden("+PFileProtector::UnProtect");
    return (UnprotectResult)policyRequest->Status;
}

bool PFileProtector::IsProtected() const
{
    Logger::Hidden("+PFileProtector::IsProtected");
    std::shared_ptr<std::iostream> inputIO = input_stream_;
    auto input_shared_stream = rmscrypto::api::CreateStreamFromStdStream(inputIO);
    try
    {
      ReadHeader(input_shared_stream);
    }
    catch (exceptions::RMSException&)
    {
      Logger::Hidden("-PFileProtector::IsProtected");
      return false;
    }

    Logger::Hidden("-PFileProtector::IsProtected");
    return true;
}

void PFileProtector::Protect(const std::shared_ptr<std::fstream>& output_stream)
{
    if (user_policy_.get() == nullptr)
    {
        Logger::Error("User Policy creation failed");
        throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
    }

    std::shared_ptr<std::iostream> outputIO = output_stream;
    auto output_shared_stream = rmscrypto::api::CreateStreamFromStdStream(outputIO);
    //std::string ext = original_file_extension_.empty() ? ".pfile" : original_file_extension_;

    input_stream_->seekg(0, std::ios::end);
    uint64_t originalFileSize = input_stream_->tellg();
    //Write Header
    auto header = WriteHeader(output_shared_stream, originalFileSize);
    auto protected_stream = CreateProtectedStream(output_shared_stream, header);
    EncryptStream(input_stream_, protected_stream, originalFileSize);
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> PFileProtector::CreateProtectedStream(
        const rmscrypto::api::SharedStream& stream,
        const std::shared_ptr<pfile::PfileHeader>& header)
{
    auto protectionPolicy = user_policy_->GetImpl();
    if ((protectionPolicy->GetCipherMode() == rmscrypto::api::CipherMode::CIPHER_MODE_ECB  ) &&
            (header->GetMajorVersion() <= rmscore::pfile::MaxMajorVerionsCBC4KIsForced))
    {
        // Older versions of the SDK ignored ECB cipher mode when encrypting pfile format.
        protectionPolicy->ReinitilizeCryptoProvider(rmscrypto::api::CipherMode::CIPHER_MODE_CBC4K);
    }

    auto cryptoProvider = user_policy_->GetImpl()->GetCryptoProvider();
    block_size_ = cryptoProvider->GetBlockSize();
    // Cache block size to be 512 for cbc512, 4096 for cbc4k and ecb
    uint64_t protectedStreamBlockSize = block_size_ == 512 ? 512 : 4096;
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
    uint64_t buf_size_temp = 4096;    //should be a multiple of AES block size (16)
    std::vector<uint8_t> buffer(buf_size_temp);
    uint64_t read_position  = 0;
    uint64_t write_position = 0;
    bool isECB = user_policy_->DoesUseDeprecatedAlgorithms();
    uint64_t total_size = isECB? ((originalFileSize + block_size_ - 1) & ~(block_size_ - 1)) :
                                originalFileSize;
    while(total_size - read_position > 0)
    {
        uint64_t offset_read  = read_position;
        uint64_t offset_write = write_position;
        uint64_t to_process   = std::min(buf_size_temp, total_size - read_position);
        read_position  += to_process;
        write_position += to_process;

        stdStream->seekg(offset_read);
        stdStream->read(reinterpret_cast<char *>(&buffer[0]), to_process);

        auto written = pStream->WriteAsync(
                    buffer.data(), to_process, offset_write, std::launch::deferred).get();

        if (written != to_process)
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
    uint64_t buf_size_temp = 4096;    //should be a multiple of AES block size (16)
    std::vector<uint8_t> buffer(buf_size_temp);
    uint64_t read_position  = 0;
    uint64_t write_position = 0;
    uint64_t total_size = pStream->Size();
    while(total_size - read_position > 0)
    {
        uint64_t offset_read  = read_position;
        uint64_t offset_write = write_position;
        uint64_t to_process   = std::min(buf_size_temp, total_size - read_position);
        uint64_t original_remaining = std::min(buf_size_temp, originalFileSize - read_position);
        read_position  += to_process;
        write_position += to_process;

        auto read = pStream->ReadAsync(
                    &buffer[0], to_process, offset_read, std::launch::deferred).get();
        if (read == 0)
        {
          break;
        }

        stdStream->seekp(offset_write);
        stdStream->write(reinterpret_cast<const char *>(buffer.data()), original_remaining);
    }
    stdStream->flush();
}

std::shared_ptr<pfile::PfileHeader> PFileProtector::WriteHeader(
        const rmscrypto::api::SharedStream& stream,
        uint64_t originalFileSize)
{
    std::shared_ptr<pfile::PfileHeader> pHeader;
    auto headerWriter = pfile::IPfileHeaderWriter::Create();
    auto publishing_license = user_policy_->SerializedPolicy();
    common::ByteArray metadata; // No metadata as of now.

    // calculate content size
    uint32_t contentStartPosition = static_cast<uint32_t>(original_file_extension_.size() +
                                                          publishing_license.size() +
                                                          metadata.size() + 454);
    pHeader = std::make_shared<pfile::PfileHeader>(move(publishing_license),
                                                   original_file_extension_,
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
