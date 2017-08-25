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
#include <vector>
#include "BlockBasedProtectedStream.h"
#include "CryptoAPI.h"
#include "RMSExceptions.h"
#include "OfficeUtils.h"
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
namespace officeprotector {

MetroOfficeProtector::MetroOfficeProtector(std::string fileName,
                                           std::shared_ptr<std::istream> inputStream)
    : m_fileName(fileName),
      m_inputStream(inputStream)
{
    gsf_init();
}

MetroOfficeProtector::~MetroOfficeProtector()
{
}

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

    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSOfficeFileException(
                    "File is already protected",
                    exceptions::RMSOfficeFileException::Reason::AlreadyProtected);
    }

    auto inputFileSize = utils::ValidateAndGetFileSize(m_inputStream.get(),
                                                       utils::MAX_FILE_SIZE_FOR_ENCRYPT);
    auto userPolicyCreationOptions = utils::ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    m_userPolicy = modernapi::UserPolicy::CreateFromTemplateDescriptor(options.templateDescriptor,
                                                                       userContext.userId,
                                                                       userContext.authenticationCallback,
                                                                       userPolicyCreationOptions,
                                                                       options.signedAppData,
                                                                       cancelState);
    std::string outputTempFileName = utils::CreateTemporaryFileName(m_fileName);
    std::unique_ptr<utils::tempFileName, utils::tempFile_deleter> outputTempFile(&outputTempFileName);
    try
    {
        //std::unique_ptr<FILE, FILE_deleter> outputTempFileStream(fopen(outputTempFileName.c_str(), "w+b"));
        ProtectInternal(outputTempFileName, inputFileSize);
        utils::CopyFromFileToOstream(outputTempFileName, outputStream.get());
    }
    catch (std::exception&)
    {
        Logger::Hidden("-MetroOfficeProtector::ProtectWithTemplate");
        throw;
    }
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

    if (IsProtected())
    {
        Logger::Error("File is already protected");
        throw exceptions::RMSOfficeFileException(
                    "File is already protected",
                    exceptions::RMSOfficeFileException::Reason::AlreadyProtected);
    }

    auto inputFileSize = utils::ValidateAndGetFileSize(m_inputStream.get(),
                                                       utils::MAX_FILE_SIZE_FOR_ENCRYPT);
    auto userPolicyCreationOptions = utils::ConvertToUserPolicyCreationOptions(
                options.allowAuditedExtraction, options.cryptoOptions);
    m_userPolicy = modernapi::UserPolicy::Create(
                const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
                userContext.userId,
                userContext.authenticationCallback,
                userPolicyCreationOptions,
                cancelState);
    std::string outputTempFileName = utils::CreateTemporaryFileName(m_fileName);
    std::unique_ptr<utils::tempFileName, utils::tempFile_deleter> outputTempFile(&outputTempFileName);
    try
    {
        //std::unique_ptr<FILE, FILE_deleter> outputTempFileStream(fopen(outputTempFileName.c_str(), "w+b"));
        ProtectInternal(outputTempFileName, inputFileSize);
        utils::CopyFromFileToOstream(outputTempFileName, outputStream.get());
    }
    catch (std::exception&)
    {
        Logger::Hidden("-MetroOfficeProtector::ProtectWithCustomRights");
        throw;
    }
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

    auto inputFileSize = utils::ValidateAndGetFileSize(m_inputStream.get(),
                                                       utils::MAX_FILE_SIZE_FOR_DECRYPT);
    auto result = UnprotectResult::NORIGHTS;
    std::string inputTempFileName = utils::CreateTemporaryFileName(m_fileName);
    std::unique_ptr<utils::tempFileName, utils::tempFile_deleter> inputTempFile(&inputTempFileName);
    try
    {
        result = UnprotectInternal(userContext, options, outputStream, inputTempFileName,
                                   inputFileSize, cancelState);
    }
    catch (std::exception&)
    {
        Logger::Hidden("-MetroOfficeProtector::UnProtect");
        throw;
    }
    Logger::Hidden("-MetroOfficeProtector::UnProtect");
    return result;
}

UnprotectResult MetroOfficeProtector::UnprotectInternal(const UserContext& userContext,
                                                        const UnprotectOptions& options,
                                                        std::shared_ptr<std::ostream> outputStream,
                                                        std::string inputTempFileName,
                                                        uint64_t inputFileSize,
                                                        std::shared_ptr<std::atomic<bool>> cancelState)
{
    utils::CopyFromIstreamToFile(m_inputStream.get(), inputTempFileName, inputFileSize);
    std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> stg;
    try
    {
        std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
                    gsf_input_stdio_new(inputTempFileName.c_str(), nullptr));
        stg.reset(gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
    }
    catch (std::exception&)
    {
        Logger::Hidden("Failed to open file as a valid CFB");
        throw exceptions::RMSOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSOfficeFileException::Reason::CorruptFile);
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
        throw exceptions::RMSOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSOfficeFileException::Reason::CorruptFile);
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
        throw exceptions::RMSOfficeFileException(
                    "The file has been corrupted",
                    exceptions::RMSOfficeFileException::Reason::CorruptFile);
    }

    uint64_t originalFileSize = 0;
    officeutils::ReadStreamHeader(metroStream.get(), originalFileSize);
    DecryptStream(outputStream, metroStream.get(), originalFileSize);
    return (UnprotectResult)policyRequest->Status;
}

bool MetroOfficeProtector::IsProtectedInternal(std::string inputTempFileName,
                                               uint64_t inputFileSize) const
{
    utils::CopyFromIstreamToFile(m_inputStream.get(), inputTempFileName, inputFileSize);
    try
    {
        std::unique_ptr<GsfInput, officeprotector::GsfInput_deleter> gsfInputStdIO(
                        gsf_input_stdio_new(inputTempFileName.c_str(), nullptr));
        std::unique_ptr<GsfInfile, officeprotector::GsfInfile_deleter> stg(
                    gsf_infile_msole_new(gsfInputStdIO.get(), nullptr));
        auto dataSpaces = std::make_shared<officeprotector::DataSpaces>(true);
        ByteArray publishingLicense;
        dataSpaces->ReadDataSpaces(stg.get(), publishingLicense);
    }
    catch (std::exception& e)
    {
        if (static_cast<exceptions::RMSException&>(e).error() ==
                static_cast<int>(exceptions::RMSException::ErrorTypes::OfficeFileError) &&
                (static_cast<exceptions::RMSOfficeFileException&>(e).reason() ==
                 exceptions::RMSOfficeFileException::Reason::NonRMSProtected))
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
    auto inputFileSize = utils::ValidateAndGetFileSize(m_inputStream.get(),
                                                       utils::MAX_FILE_SIZE_FOR_DECRYPT);
    std::string inputTempFileName = utils::CreateTemporaryFileName(m_fileName);
    std::unique_ptr<utils::tempFileName, utils::tempFile_deleter> inputTempFile(&inputTempFileName);
    bool isProtected = IsProtectedInternal(inputTempFileName, inputFileSize);
    Logger::Hidden("-MetroOfficeProtector::IsProtected");
    return isProtected;
}

void MetroOfficeProtector::ProtectInternal(std::string outputTempFileName,
                                           uint64_t originalFileSize)
{
    if (m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy creation failed");
        throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
    }

    std::unique_ptr<GsfOutput, officeprotector::GsfOutput_deleter> gsfOutputStdIO(
                gsf_output_stdio_new(outputTempFileName.c_str(), nullptr));

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
    officeutils::WriteStreamHeader(metroStream.get(), originalFileSize);

    EncryptStream(metroStream.get(), originalFileSize);
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
                                         uint64_t inputFileSize)
{
    auto cryptoProvider = m_userPolicy->GetImpl()->GetCryptoProvider();
    m_blockSize = cryptoProvider->GetBlockSize();
    std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
    uint64_t readPosition  = 0;
    bool isECB = m_userPolicy->DoesUseDeprecatedAlgorithms();
    uint64_t totalSize = isECB? ((inputFileSize + m_blockSize - 1) & ~(m_blockSize - 1)) :
                                inputFileSize;
    while(totalSize - readPosition > 0)
    {
        uint64_t offsetRead  = readPosition;
        uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
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
    std::vector<uint8_t> buffer(utils::BUF_SIZE_BYTES);
    uint64_t readPosition  = 0;
    uint64_t writePosition = 0;
    uint64_t totalSize = (uint64_t)gsf_input_size(metroStream) - sizeof(uint64_t);

    while(totalSize - readPosition > 0)
    {
        uint64_t offsetWrite = writePosition;
        uint64_t toProcess   = std::min(utils::BUF_SIZE_BYTES, totalSize - readPosition);
        uint64_t originalRemaining = std::min(utils::BUF_SIZE_BYTES, originalFileSize - readPosition);
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

} // namespace officeprotector
} // namespace rmscore
