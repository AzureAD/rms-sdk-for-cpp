/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "MetroOfficeProtector.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
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

const char metroContent[]        = "EncryptedPackage";

} // namespace

namespace rmscore {
namespace fileapi {

MetroOfficeProtector::MetroOfficeProtector(std::string fileName,
                                           std::shared_ptr<std::istream> inputStream)
    : m_fileName(fileName),
      m_inputStream(inputStream)
{
    gsf_init();
}

MetroOfficeProtector::~MetroOfficeProtector()
{
    gsf_shutdown();
}

struct FILE_deleter
{
    void operator () (FILE* obj) const
    {
        std::fclose(obj);
    }
};

void MetroOfficeProtector::ProtectWithTemplate(const UserContext& userContext,
                                               const ProtectWithTemplateOptions& options,
                                               std::shared_ptr<std::ostream> outputStream,
                                               std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+MetroOfficeProtector::ProtectWithTemplate");
    if (!outputStream->good())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    auto inputFileSize = GetFileSize(MAX_FILE_SIZE_FOR_ENCRYPT);
    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSMetroOfficeFileException(
                    "File is already protected",
                    exceptions::RMSMetroOfficeFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    m_userPolicy = modernapi::UserPolicy::CreateFromTemplateDescriptor(options.templateDescriptor,
                                                                       userContext.userId,
                                                                       userContext.authenticationCallback,
                                                                       userPolicyCreationOptions,
                                                                       options.signedAppData,
                                                                       cancelState);
    std::string tempFileName = CreateTemporaryFileName();
    try
    {
        std::unique_ptr<FILE, FILE_deleter> tempFile(fopen(tempFileName.c_str(), "w+b"));
        ProtectInternal(tempFile.get(), tempFileName, inputFileSize);
        CopyFromFileToOstream(tempFile.get(), outputStream.get());
    }
    catch (std::exception&)
    {
        remove(tempFileName.c_str());
        Logger::Hidden("-MetroOfficeProtector::ProtectWithTemplate");
        throw;
    }
    remove(tempFileName.c_str());
    Logger::Hidden("-MetroOfficeProtector::ProtectWithTemplate");
}

void MetroOfficeProtector::ProtectWithCustomRights(const UserContext& userContext,
                                                   const ProtectWithCustomRightsOptions& options,
                                                   std::shared_ptr<std::ostream> outputStream,
                                                   std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+MetroOfficeProtector::ProtectWithCustomRights");
    if (!outputStream->good())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    auto inputFileSize = GetFileSize(MAX_FILE_SIZE_FOR_ENCRYPT);
    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSMetroOfficeFileException(
                    "File is already protected",
                    exceptions::RMSMetroOfficeFileException::AlreadyProtected);
    }

    auto userPolicyCreationOptions = ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    m_userPolicy = modernapi::UserPolicy::Create(
                const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
                userContext.userId,
                userContext.authenticationCallback,
                userPolicyCreationOptions,
                cancelState);
    std::string tempFileName = CreateTemporaryFileName();
    try
    {
        std::unique_ptr<FILE, FILE_deleter> tempFile(fopen(tempFileName.c_str(), "w+b"));
        ProtectInternal(tempFile.get(), tempFileName, inputFileSize);
        CopyFromFileToOstream(tempFile.get(), outputStream.get());
    }
    catch (std::exception&)
    {
        remove(tempFileName.c_str());
        Logger::Hidden("-MetroOfficeProtector::ProtectWithCustomRights");
        throw;
    }
    remove(tempFileName.c_str());
    Logger::Hidden("-MetroOfficeProtector::ProtectWithCustomRights");
}

UnprotectResult MetroOfficeProtector::Unprotect(const UserContext& userContext,
                                                const UnprotectOptions& options,
                                                std::shared_ptr<std::ostream> outputStream,
                                                std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+MetroOfficeProtector::UnProtect");
    if (!outputStream->good())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    auto inputFileSize = GetFileSize(MAX_FILE_SIZE_FOR_DECRYPT);
    auto result = UnprotectResult::NORIGHTS;
    std::string tempFileName = CreateTemporaryFileName();
    try
    {
        result = UnprotectInternal(userContext, options, outputStream, tempFileName,
                                   inputFileSize, cancelState);
    }
    catch (std::exception&)
    {
        remove((tempFileName.c_str()));
        Logger::Hidden("-MetroOfficeProtector::UnProtect");
        throw;
    }
    remove(tempFileName.c_str());
    Logger::Hidden("-MetroOfficeProtector::UnProtect");
    return result;
}

UnprotectResult MetroOfficeProtector::UnprotectInternal(const UserContext& userContext,
                                                        const UnprotectOptions& options,
                                                        std::shared_ptr<std::ostream> outputStream,
                                                        std::string tempFileName,
                                                        uint64_t inputFileSize,
                                                        std::shared_ptr<std::atomic<bool>> cancelState)
{
    //std::unique_ptr<FILE, FILE_deleter> tempFile(fopen(tempFileName.c_str(), "rb"));
    CopyFromIstreamToFile(tempFileName, inputFileSize);
    std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> stg;
    try
    {
        std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
                    gsf_input_stdio_new(tempFileName.c_str(), nullptr));
        stg.reset(gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
    }
    catch (std::exception&)
    {
        Logger::Hidden("Failed to open file as a valid CFB");
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    ByteArray publishingLicense;
    try
    {
        auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(true, true);
        dataSpaces->ReadDataSpaces(stg.get(), publishingLicense);
    }
    catch (std::exception&)
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
    auto policyRequest = modernapi::UserPolicy::Acquire(publishingLicense,
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

    m_userPolicy = policyRequest->Policy;
    if (m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy acquisition failed");
        throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    }

    if (!m_userPolicy->DoesUseDeprecatedAlgorithms())
    {
        throw exceptions::RMSLogicException(exceptions::RMSException::ErrorTypes::NotSupported,
                                            "CBC Decryption with Office files is not yet supported");
    }

    std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> metroStream(
                gsf_infile_child_by_name(stg.get(), metroContent));   
    if (metroStream == nullptr)
    {
        Logger::Error("Stream containing encrypted data not present");
        throw exceptions::RMSMetroOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSMetroOfficeFileException::CorruptFile);
    }

    uint64_t originalFileSize = 0;
    ReadStreamHeader(metroStream.get(), originalFileSize);
    DecryptStream(outputStream, metroStream.get(), originalFileSize);
    return (UnprotectResult)policyRequest->Status;
}

bool MetroOfficeProtector::IsProtectedInternal(std::string tempFileName,
                                               uint64_t inputFileSize) const
{
    //std::unique_ptr<FILE, FILE_deleter> tempFile(fopen(tempFileName.c_str(), "rb"));
    CopyFromIstreamToFile(tempFileName, inputFileSize);
    try
    {
        std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
                        gsf_input_stdio_new(tempFileName.c_str(), nullptr));
        std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> stg(
                    gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
        auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(true);
        ByteArray publishingLicense;
        dataSpaces->ReadDataSpaces(stg.get(), publishingLicense);
    }
    catch (std::exception& e)
    {
        Logger::Hidden("-MetroOfficeProtector::IsProtected");
        if (static_cast<exceptions::RMSException&>(e).error() ==
                exceptions::RMSException::MetroOfficeFileError &&
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
    return true;
}

bool MetroOfficeProtector::IsProtected() const
{
    Logger::Hidden("+MetroOfficeProtector::IsProtected");
    auto inputFileSize = GetFileSize(MAX_FILE_SIZE_FOR_DECRYPT);
    std::string tempFileName = CreateTemporaryFileName();
    bool isProtected = IsProtectedInternal(tempFileName, inputFileSize);
    remove((tempFileName.c_str()));
    Logger::Hidden("-MetroOfficeProtector::IsProtected");
    return isProtected;
}

void MetroOfficeProtector::ProtectInternal(FILE* tempFile,
                                           std::string tempFileName,
                                           uint64_t originalFileSize)
{
    if (m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy creation failed");
        throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
    }

    std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> gsfOutputStdIO(
                gsf_output_stdio_new_FILE(tempFileName.c_str(), tempFile, true));

    std::unique_ptr<GsfOutfile, officeprotector::GsfOutfile_deleter> stg(
                gsf_outfile_msole_new(gsfOutputStdIO.get()));
    {
        //Write Dataspaces
        auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(
                    true, m_userPolicy->DoesUseDeprecatedAlgorithms());
        auto publishingLicense = m_userPolicy->SerializedPolicy();
        dataSpaces->WriteDataSpaces(stg.get(), publishingLicense);
    }

    std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> metroStream(
                gsf_outfile_new_child(stg.get(), metroContent, false));    
    WriteStreamHeader(metroStream.get(), originalFileSize);

    EncryptStream(metroStream.get(), m_userPolicy->GetImpl()->
                  GetCryptoProvider()->GetCipherTextSize(originalFileSize));
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

void MetroOfficeProtector::EncryptStream(GsfOutput* metroStream,
                                         uint64_t originalFileSize)
{
    auto cryptoProvider = m_userPolicy->GetImpl()->GetCryptoProvider();
    m_blockSize = cryptoProvider->GetBlockSize();
    std::vector<uint8_t> buffer(BUF_SIZE);
    uint64_t readPosition  = 0;
    bool isECB = m_userPolicy->DoesUseDeprecatedAlgorithms();
    uint64_t totalSize = isECB? ((originalFileSize + m_blockSize - 1) & ~(m_blockSize - 1)) :
                                originalFileSize;    
    while(totalSize - readPosition > 0)
    {
        uint64_t offsetRead  = readPosition;
        uint64_t toProcess   = std::min(BUF_SIZE, totalSize - readPosition);
        readPosition  += toProcess;

        auto sstream = std::make_shared<std::stringstream>();
        std::shared_ptr<std::iostream> iosstream = sstream;
        auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
        auto pStream = CreateProtectedStream(sharedStringStream, 0, cryptoProvider);

        m_inputStream->seekg(offsetRead);
        m_inputStream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);

        pStream->WriteAsync(
                    buffer.data(), toProcess, 0, std::launch::deferred).get();

        pStream->FlushAsync(std::launch::deferred).get();

        std::string encryptedData = sstream->str();
        auto encryptedDataLen = encryptedData.length();

        gsf_output_write(metroStream, encryptedDataLen,
                         reinterpret_cast<const uint8_t*>(encryptedData.data()));
    }
}

void MetroOfficeProtector::DecryptStream(const std::shared_ptr<std::ostream>& stdStream,
                                         GsfInput* metroStream,
                                         uint64_t originalFileSize)
{
    auto cryptoProvider = m_userPolicy->GetImpl()->GetCryptoProvider();
    m_blockSize = cryptoProvider->GetBlockSize();
    std::vector<uint8_t> buffer(BUF_SIZE);
    uint64_t readPosition  = 0;
    uint64_t writePosition = 0;
    //bool isECB = m_userPolicy->DoesUseDeprecatedAlgorithms();
    uint64_t totalSize = (uint64_t)gsf_input_size(metroStream) - sizeof(uint64_t);

    while(totalSize - readPosition > 0)
    {
        uint64_t offsetWrite = writePosition;
        uint64_t toProcess   = std::min(BUF_SIZE, totalSize - readPosition);
        uint64_t originalRemaining = std::min(BUF_SIZE, originalFileSize - readPosition);
        readPosition  += toProcess;
        writePosition += toProcess;

        gsf_input_read(metroStream, toProcess, &buffer[0]);

        std::string encryptedData(reinterpret_cast<const char *>(buffer.data()), toProcess);
        auto sstream = std::make_shared<std::stringstream>();
        sstream->str(encryptedData);
        std::shared_ptr<std::iostream> iosstream = sstream;
        auto sharedStringStream = rmscrypto::api::CreateStreamFromStdStream(iosstream);
        auto pStream = CreateProtectedStream(sharedStringStream, encryptedData.length(),
                                             cryptoProvider);

        pStream->ReadAsync(&buffer[0], toProcess, 0, std::launch::deferred).get();

        stdStream->seekp(offsetWrite);
        stdStream->write(reinterpret_cast<const char *>(buffer.data()), originalRemaining);
    }
    stdStream->flush();
}

void MetroOfficeProtector::WriteStreamHeader(GsfOutput* stm,
                                             const uint64_t &contentLength)
{
    if ( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for writing stream header");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in writing to stream", exceptions::RMSMetroOfficeFileException::Unknown);
    }
    gsf_output_seek(stm, 0, G_SEEK_SET);
    gsf_output_write(stm, sizeof(uint64_t), reinterpret_cast<const uint8_t*>(&contentLength));
}

void MetroOfficeProtector::ReadStreamHeader(GsfInput* stm,
                                            uint64_t &contentLength)
{
    if ( stm == nullptr)
    {
        Logger::Error("Invalid arguments provided for reading stream header");
        throw exceptions::RMSMetroOfficeFileException(
                    "Error in reading from stream",
                    exceptions::RMSMetroOfficeFileException::Unknown);
    }
    gsf_input_seek(stm, 0, G_SEEK_SET);
    gsf_input_read(stm, sizeof(uint64_t), reinterpret_cast<uint8_t*>(&contentLength));
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

void MetroOfficeProtector::CopyFromFileToOstream(FILE* file, std::ostream* stream) const
{
    fseek(file, 0L, SEEK_END);
    uint64_t fileSize = ftell(file);
    rewind(file);
    std::vector<uint8_t> buffer(BUF_SIZE);
    auto count = fileSize;
    while(count > BUF_SIZE)
    {
        fread(&buffer[0], BUF_SIZE, 1, file);
        stream->write(reinterpret_cast<const char*>(buffer.data()), BUF_SIZE);
        count -= BUF_SIZE;
    }

    fread((&buffer[0]), count, 1, file);
    stream->write(reinterpret_cast<const char*>(buffer.data()), count);
    stream->flush();
}

void MetroOfficeProtector::CopyFromIstreamToFile(std::string tempFileName,
                                                 uint64_t inputFileSize) const
{
    std::unique_ptr<FILE, FILE_deleter> tempFile(fopen(tempFileName.c_str(), "w+b"));

    std::vector<uint8_t> buffer(BUF_SIZE);
    auto count = inputFileSize;
    while(count > BUF_SIZE)
    {
        m_inputStream->read(reinterpret_cast<char *>(&buffer[0]), BUF_SIZE);
        fwrite(reinterpret_cast<const char*>(buffer.data()), BUF_SIZE, 1, tempFile.get());
        count -= BUF_SIZE;
    }

    m_inputStream->read(reinterpret_cast<char *>(&buffer[0]), count);
    fwrite(reinterpret_cast<const char*>(buffer.data()), count, 1, tempFile.get());
    fflush(tempFile.get());
}

std::string MetroOfficeProtector::CreateTemporaryFileName() const
{
    srand(time(NULL));
    uint32_t random = rand() % 10000;
    return (m_fileName + std::to_string(random) + ".tmp");
}

uint64_t MetroOfficeProtector::GetFileSize(uint64_t maxFileSize) const
{
    m_inputStream->seekg(0, std::ios::end);
    uint64_t fileSize = m_inputStream->tellg();
    m_inputStream->seekg(0);
    if(maxFileSize < fileSize)
    {
        Logger::Error("Input file too large");
        throw exceptions::RMSLogicException(exceptions::RMSLogicException::ErrorTypes::NotSupported,
                                            "The file is too large. The limit is 1GB for encryption"
                                            "and 3GB for decryption");
    }

    return fileSize;
}

} // namespace fileapi
} // namespace rmscore
