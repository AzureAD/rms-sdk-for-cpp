/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "MetroOfficeProtector.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "BlockBasedProtectedStream.h"
#include "CryptoAPI.h"
#include "RMSExceptions.h"
#include "DataSpaces.h"
#include "../Common/CommonTypes.h"
#include "../Core/ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;
using namespace rmscore::common;

namespace{

const char metroContent[]        = "/EncryptedPackage";

} // namespace

namespace rmscore {
namespace fileapi {

MetroOfficeProtector::MetroOfficeProtector(std::shared_ptr<std::fstream> input_stream)
    : input_stream_(input_stream)
{    
}

MetroOfficeProtector::~MetroOfficeProtector()
{
}

void MetroOfficeProtector::ProtectWithTemplate(const UserContext& userContext,
                                               const ProtectWithTemplateOptions& options,
                                               std::shared_ptr<std::fstream> output_stream,
                                               std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+MetroOfficeProtector::ProtectWithTemplate");
    if (!output_stream->is_open())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSMetroOfficeFileException(
                    "File is already protected",
                    exceptions::RMSMetroOfficeFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    user_policy_ = modernapi::UserPolicy::CreateFromTemplateDescriptor(options.templateDescriptor,
                                                                       userContext.userId,
                                                                       userContext.authenticationCallback,
                                                                       userPolicyCreationOptions,
                                                                       options.signedAppData,
                                                                       cancelState);
    m_storage = std::make_shared<pole::Storage>(output_stream);
    m_storage->open(true, true);
    Protect(output_stream);
    //Write Dataspaces
    auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(
                true, user_policy_->DoesUseDeprecatedAlgorithms());
    auto publishing_license = user_policy_->SerializedPolicy();
    dataSpaces->WriteDataspaces(m_storage, publishing_license);
    Logger::Hidden("-MetroOfficeProtector::ProtectWithTemplate");
}

void MetroOfficeProtector::ProtectWithCustomRights(const UserContext& userContext,
                                                   const ProtectWithCustomRightsOptions& options,
                                                   std::shared_ptr<std::fstream> output_stream,
                                                   std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+MetroOfficeProtector::ProtectWithCustomRights");
    if (!output_stream->is_open())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSMetroOfficeFileException(
                    "File is already protected",
                    exceptions::RMSMetroOfficeFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    user_policy_ = modernapi::UserPolicy::Create(
                const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
                userContext.userId,
                userContext.authenticationCallback,
                userPolicyCreationOptions,
                cancelState);
    m_storage = std::make_shared<pole::Storage>(output_stream);
    m_storage->open(true, true);
    //Write Dataspaces
    auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(
                true, user_policy_->DoesUseDeprecatedAlgorithms());
    auto publishing_license = user_policy_->SerializedPolicy();
    dataSpaces->WriteDataspaces(m_storage, publishing_license);
    Protect(output_stream);
    Logger::Hidden("-MetroOfficeProtector::ProtectWithCustomRights");
}

UnprotectResult MetroOfficeProtector::Unprotect(const UserContext& userContext,
                                                const UnprotectOptions& options,
                                                std::shared_ptr<std::fstream> output_stream,
                                                std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+MetroOfficeProtector::UnProtect");
    if (!output_stream->is_open())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    std::shared_ptr<rmscore::pole::Storage> storage;
    try
    {
        storage = std::make_shared<rmscore::pole::Storage>(input_stream_);
        storage->open();
    }
    catch(std::exception&)
    {
        Logger::Hidden("Failed to open file as a valid CFB");
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    ByteArray publishing_license;
    try
    {
        auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(true, true);
        dataSpaces->ReadDataspaces(storage, publishing_license);
    }
    catch (exceptions::RMSException&)
    {
      Logger::Hidden("Failed to read dataspaces");
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
    auto policyRequest = modernapi::UserPolicy::Acquire(publishing_license,
                                                        userContext.userId,
                                                        userContext.authenticationCallback,
                                                        &userContext.consentCallback,
                                                        policyAcquisitionOptions,
                                                        cacheMask,
                                                        cancelState);
    if (policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success)
    {
        Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    user_policy_ = policyRequest->Policy;
    if (user_policy_.get() == nullptr)
    {
        Logger::Error("User Policy acquisition failed");
        throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    }

    if (!user_policy_->DoesUseDeprecatedAlgorithms())
    {
        throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                            "CBC Decryption with Office files is not yet supported");
    }

    if (!storage->exists(metroContent))
    {
        Logger::Error("Stream containing encrypted data not present");
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    auto metroStream = std::make_shared<pole::Stream>(storage.get(), metroContent, false);
    uint64_t originalFileSize = 0;
    ReadStreamHeader(metroStream, originalFileSize);
    DecryptStream(output_stream, metroStream, originalFileSize);

    Logger::Hidden("-MetroOfficeProtector::UnProtect");
    return (UnprotectResult)policyRequest->Status;
}

bool MetroOfficeProtector::IsProtected() const
{
    Logger::Hidden("+MetroOfficeProtector::IsProtected");

    try
    {
      auto storage = std::make_shared<rmscore::pole::Storage>(input_stream_);
      storage->open();
      auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(true);
      ByteArray publishing_license;
      dataSpaces->ReadDataspaces(storage, publishing_license);
    }
    catch (exceptions::RMSException& e)
    {
        Logger::Hidden("-MetroOfficeProtector::IsProtected");
        if (e.error() == exceptions::RMSException::MetroOfficeFileError &&
                (static_cast<exceptions::RMSMetroOfficeFileException&>(e).reason() ==
                 exceptions::RMSMetroOfficeFileException::NonRMSProtected))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    Logger::Hidden("-MetroOfficeProtector::IsProtected");
    return true;
}

void MetroOfficeProtector::Protect(const std::shared_ptr<std::fstream>& output_stream)
{
    FILEAPI_UNREFERENCED_PARAMETER(output_stream);

    if (user_policy_.get() == nullptr)
    {
        Logger::Error("User Policy creation failed");
        throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
    }

    std::shared_ptr<pole::Stream> metroStream = std::make_shared<pole::Stream>(m_storage.get(),
                                                                               metroContent, true);
    input_stream_->seekg(0, std::ios::end);
    uint64_t originalFileSize = input_stream_->tellg();
    WriteStreamHeader(metroStream, originalFileSize);
    input_stream_->seekg(0);

    EncryptStream(input_stream_, metroStream,
                  user_policy_->GetImpl()->GetCryptoProvider()->GetCipherTextSize(originalFileSize));
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> MetroOfficeProtector::CreateProtectedStream(
        const rmscrypto::api::SharedStream& stream,
        uint64_t streamSize,
        std::shared_ptr<rmscrypto::api::ICryptoProvider> cryptoProvider)
{
    // Cache block size to be 512 for cbc512, 4096 for cbc4k and ecb
    uint64_t protectedStreamBlockSize = cryptoProvider->GetBlockSize() == 512 ? 512 : 4096;
    return rmscrypto::api::BlockBasedProtectedStream::Create(cryptoProvider,
                                                             stream,
                                                             0,
                                                             streamSize,
                                                             protectedStreamBlockSize);
}

void MetroOfficeProtector::EncryptStream(const std::shared_ptr<std::fstream>& stdStream,
                                         const std::shared_ptr<pole::Stream>& metroStream,
                                         uint64_t originalFileSize)
{
    auto cryptoProvider = user_policy_->GetImpl()->GetCryptoProvider();
    block_size_ = cryptoProvider->GetBlockSize();
    uint64_t buf_size_temp = 4096;    //should be a multiple of AES block size (16)
    std::vector<uint8_t> buffer(buf_size_temp);
    uint64_t read_position  = 0;
    bool isECB = user_policy_->DoesUseDeprecatedAlgorithms();
    uint64_t total_size = isECB? ((originalFileSize + block_size_ - 1) & ~(block_size_ - 1)) :
                                originalFileSize;
    uint64_t len = 0;
    while(total_size - read_position > 0)
    {
        uint64_t offset_read  = read_position;
        uint64_t to_process   = std::min(buf_size_temp, total_size - read_position);
        read_position  += to_process;

        auto sstream = std::make_shared<std::stringstream>();
        std::shared_ptr<std::iostream> iosstream = sstream;
        auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
        auto pStream = CreateProtectedStream(sharedStringStream, 0, cryptoProvider);

        stdStream->seekg(offset_read);
        stdStream->read(reinterpret_cast<char *>(&buffer[0]), to_process);

        auto written = pStream->WriteAsync(
                    buffer.data(), to_process, 0, std::launch::deferred).get();
        FILEAPI_UNREFERENCED_PARAMETER(written);

        pStream->FlushAsync(std::launch::deferred).get();

        std::string encryptedData = sstream->str();
        auto encryptedDataLen = encryptedData.length();

        len += metroStream->write(reinterpret_cast<unsigned char*>(const_cast<char*>(encryptedData.data())),
                           encryptedDataLen);
    }
    metroStream->flush();
}

void MetroOfficeProtector::DecryptStream(const std::shared_ptr<std::iostream>& stdStream,
                                         const std::shared_ptr<pole::Stream>& metroStream,
                                         uint64_t originalFileSize)
{
    auto cryptoProvider = user_policy_->GetImpl()->GetCryptoProvider();
    block_size_ = cryptoProvider->GetBlockSize();
    uint64_t buf_size_temp = 4096;    //should be a multiple of AES block size (16)
    std::vector<uint8_t> buffer(buf_size_temp);
    uint64_t read_position  = 0;
    uint64_t write_position = 0;
    //bool isECB = user_policy_->DoesUseDeprecatedAlgorithms();
    uint64_t total_size = metroStream->size() - sizeof(uint64_t);

    while(total_size - read_position > 0)
    {
        uint64_t offset_write = write_position;
        uint64_t to_process   = std::min(buf_size_temp, total_size - read_position);
        uint64_t original_remaining = std::min(buf_size_temp, originalFileSize - read_position);
        read_position  += to_process;
        write_position += to_process;

        auto read = metroStream->read(&buffer[0], to_process);
        if (read == 0)
        {
          break;
        }

        std::string encryptedData(reinterpret_cast<const char *>(buffer.data()), read);
        auto sstream = std::make_shared<std::stringstream>();
        sstream->str(encryptedData);
        std::shared_ptr<std::iostream> iosstream = sstream;
        auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
        auto pStream = CreateProtectedStream(sharedStringStream, encryptedData.length(),
                                             cryptoProvider);

        read = pStream->ReadAsync(&buffer[0], read, 0, std::launch::deferred).get();

        stdStream->seekg(offset_write);
        stdStream->write(reinterpret_cast<const char *>(buffer.data()), original_remaining);
    }
    stdStream->flush();
}

void MetroOfficeProtector::WriteStreamHeader(const std::shared_ptr<pole::Stream>& stm,
                                             const uint64_t &contentLength)
{
    if ( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing stream header");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }
    stm->seek(0);
    stm->write(reinterpret_cast<unsigned char*>(const_cast<uint64_t*>(&contentLength)),
               sizeof(uint64_t));
}

void MetroOfficeProtector::ReadStreamHeader(const std::shared_ptr<pole::Stream>& stm,
                                            uint64_t &contentLength)
{
    if ( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading stream header");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }
    stm->seek(0);
    stm->read(reinterpret_cast<unsigned char*>(&contentLength), sizeof(uint64_t));
}

modernapi::UserPolicyCreationOptions MetroOfficeProtector::ConvertToUserPolicyCreationOptions(
        const bool& allowAuditedExtraction,
        CryptoOptions cryptoOptions)
{
    auto userPolicyCreationOptions = allowAuditedExtraction ?
                modernapi::UserPolicyCreationOptions::USER_AllowAuditedExtraction :
                modernapi::UserPolicyCreationOptions::USER_None;
    if (cryptoOptions == CryptoOptions::AUTO ||
            cryptoOptions == CryptoOptions::AES128_ECB )
    {
        userPolicyCreationOptions = static_cast<modernapi::UserPolicyCreationOptions>(
                    userPolicyCreationOptions |
                    modernapi::UserPolicyCreationOptions::USER_PreferDeprecatedAlgorithms);
    }
    else    //temporary until we have CBC for office files
    {
        throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                            "CBC Encryption with Office files is not yet"
                                            "supported");
    }
    return userPolicyCreationOptions;
}

} // namespace fileapi
} // namespace rmscore
