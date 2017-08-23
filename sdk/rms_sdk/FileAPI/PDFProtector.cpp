/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include "PDFProtector.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "BlockBasedProtectedStream.h"
#include "RMSExceptions.h"
#include "../Common/CommonTypes.h"
#include "../Core/ProtectionPolicy.h"
#include "../Platform/Logger/Logger.h"

using namespace rmscore::platform::logger;

namespace rmscore {
namespace fileapi {

////////////////////////////////////////////////////////////////////////////
/// class PDFProtector
PDFCryptoHandlerImpl::PDFCryptoHandlerImpl(PDFProtector *pPDFProtector)
{
    m_pPDFProtector = pPDFProtector;
    m_bProgressiveStart = false;

    m_dataToDecrypted = nullptr;
    m_objnum = 0;

    m_sharedProtectedStream = nullptr;
    m_outputIOS = nullptr;
    m_outputSharedStream = nullptr;
}

PDFCryptoHandlerImpl::~PDFCryptoHandlerImpl()
{
    m_pPDFProtector = nullptr;
}

uint32_t PDFCryptoHandlerImpl::DecryptGetSize(uint32_t src_size)
{
    return src_size;
}

void* PDFCryptoHandlerImpl::DecryptStart(uint32_t objnum, uint32_t gennum)
{
    m_objnum = objnum;
    m_dataToDecrypted = std::make_shared<std::stringstream>();
    return nullptr;
}

bool PDFCryptoHandlerImpl::DecryptStream(void* context, char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf)
{
    m_dataToDecrypted->write(src_buf, src_size);
    return true;
}

bool PDFCryptoHandlerImpl::DecryptFinish(void* context, PDFBinaryBuf* dest_buf)
{
    m_dataToDecrypted->seekg(0, std::ios::end);
    uint64_t count = m_dataToDecrypted->tellg();

    std::shared_ptr<std::iostream> protectedIOS = m_dataToDecrypted;
    auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);
    auto protectedStream = m_pPDFProtector->CreateProtectedStream(inputSharedStream, count);

    std::shared_ptr<std::stringstream> outputSS = std::make_shared<std::stringstream>();
    std::shared_ptr<std::iostream> outputIOS = outputSS;
    auto outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);

    m_pPDFProtector->DecryptStream(outputSharedStream, protectedStream, count);

    outputSharedStream->Seek(std::ios::beg);
    auto nSize = outputSharedStream->Size();
    std::vector<uint8_t> decryptedData = outputSharedStream->Read(nSize);

    char bufDestSize[4];
    memset(bufDestSize, 0, 4 * sizeof(char));
    bufDestSize[0] = decryptedData[3];
    bufDestSize[1] = decryptedData[2];
    bufDestSize[2] = decryptedData[1];
    bufDestSize[3] = decryptedData[0];
    uint64_t dwTotalDestSize = 0;
    memcpy(&dwTotalDestSize, bufDestSize, 4);

    dest_buf->AppendBlock(reinterpret_cast<const char*>(&decryptedData[0] + 4), dwTotalDestSize);

    m_objnum = 0;
    m_dataToDecrypted.reset();
    m_dataToDecrypted = nullptr;

    return true;
}

uint32_t PDFCryptoHandlerImpl::EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size)
{
    uint32_t encryptedSize = 0;
    encryptedSize = src_size;

    encryptedSize += 4; //add prefix padding
    encryptedSize += (16 - encryptedSize % 16);//add postfix padding

    return encryptedSize;
}

bool PDFCryptoHandlerImpl::EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size)
{
    if(!m_pPDFProtector) return false;

    uint64_t contentSizeAddPre = src_size + 4;
    char* contentAddPre = new char[contentSizeAddPre];
    contentAddPre[3] = ((char*)&src_size)[0];
    contentAddPre[2] = ((char*)&src_size)[1];
    contentAddPre[1] = ((char*)&src_size)[2];
    contentAddPre[0] = ((char*)&src_size)[3];
    memcpy(contentAddPre + 4, src_buf, src_size);

    auto sstream = std::make_shared<std::stringstream>();
    std::shared_ptr<std::iostream> outputIOS = sstream;

    auto outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(outputIOS);
    auto protectedStream = m_pPDFProtector->CreateProtectedStream(outputSharedStream, contentSizeAddPre);
    m_pPDFProtector->EncryptStream(contentAddPre, contentSizeAddPre, protectedStream, true);

    outputSharedStream->Seek(std::ios::beg);
    auto nSize = outputSharedStream->Size();
    int64_t dataRead = outputSharedStream->Read(reinterpret_cast<uint8_t*>(dest_buf), nSize);
    *dest_size = nSize;

    delete [] contentAddPre;
    return true;
}

bool PDFCryptoHandlerImpl::ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size)
{
    if (raw_size > MIN_RAW_SIZE)
    {
        m_bProgressiveStart = true;
        return true;
    }
    return false;
}

bool PDFCryptoHandlerImpl::ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf)
{
    uint64_t contentSizeAddPre = 0;
    char* contentAddPre = nullptr;

    if(m_bProgressiveStart)
    {
        contentSizeAddPre = src_size + 4;
        contentAddPre = new char[contentSizeAddPre];
        contentAddPre[3] = ((char*)&src_size)[0];
        contentAddPre[2] = ((char*)&src_size)[1];
        contentAddPre[1] = ((char*)&src_size)[2];
        contentAddPre[0] = ((char*)&src_size)[3];
        memcpy(contentAddPre + 4, src_buf, src_size);

        auto sstream = std::make_shared<std::stringstream>();
        m_outputIOS = sstream;

        m_outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(m_outputIOS);
        m_sharedProtectedStream = m_pPDFProtector->CreateProtectedStream(m_outputSharedStream, contentSizeAddPre);
    }
    else
    {
        contentSizeAddPre = src_size;
        contentAddPre = src_buf;
    }

    m_pPDFProtector->EncryptStream(contentAddPre, contentSizeAddPre, m_sharedProtectedStream, false);

    if(m_bProgressiveStart)
    {
        delete [] contentAddPre;
    }

    m_bProgressiveStart = false;

    return true;
}

bool PDFCryptoHandlerImpl::ProgressiveEncryptFinish(PDFBinaryBuf* dest_buf)
{
    m_pPDFProtector->EncryptStream(nullptr, 0, m_sharedProtectedStream, true);

    m_outputSharedStream->Seek(std::ios::beg);
    auto nSize = m_outputSharedStream->Size();

    char* pDataRead = new char[nSize];
    int64_t dataRead = m_outputSharedStream->Read(reinterpret_cast<uint8_t*>(pDataRead), nSize);

    dest_buf->AppendBlock(pDataRead, nSize);

    delete [] pDataRead;

    m_sharedProtectedStream.reset();
    m_outputIOS.reset();
    m_outputSharedStream.reset();

    m_sharedProtectedStream = nullptr;
    m_outputIOS = nullptr;
    m_outputSharedStream = nullptr;

    return true;
}

////////////////////////////////////////////////////////////////////////////
/// class PDFSecurityHandlerImpl

PDFSecurityHandlerImpl::PDFSecurityHandlerImpl(
        PDFProtector* pPDFProtector,
        const UserContext &userContext,
        const UnprotectOptions &options,
        std::shared_ptr<std::atomic<bool> > cancelState)
    : m_userContext(userContext),
      m_options(options),
      m_cancelState(cancelState)
{
    m_pPDFProtector = pPDFProtector;
    m_pCryptoHandler = nullptr;
}

PDFSecurityHandlerImpl::~PDFSecurityHandlerImpl()
{
    if(m_pCryptoHandler)
    {
        delete m_pCryptoHandler;
        m_pCryptoHandler = nullptr;
    }
}

bool PDFSecurityHandlerImpl::OnInit(unsigned char *publishingLicense, uint32_t plSize)
{
    std::vector<uint8_t> vecPL(plSize);
    memcpy(reinterpret_cast<uint8_t *>(&vecPL[0]), publishingLicense, plSize);

    modernapi::PolicyAcquisitionOptions policyAcquisitionOptions = m_options.offlineOnly?
                modernapi::PolicyAcquisitionOptions::POL_OfflineOnly :
                modernapi::PolicyAcquisitionOptions::POL_None;
    auto cacheMask = modernapi::RESPONSE_CACHE_NOCACHE;
    if (m_options.useCache)
    {
        cacheMask = static_cast<modernapi::ResponseCacheFlags>(modernapi::RESPONSE_CACHE_INMEMORY|
                                                               modernapi::RESPONSE_CACHE_ONDISK |
                                                               modernapi::RESPONSE_CACHE_CRYPTED);
    }

    auto policyRequest = modernapi::UserPolicy::Acquire(vecPL,
                                                        m_userContext.userId,
                                                        m_userContext.authenticationCallback,
                                                        &m_userContext.consentCallback,
                                                        policyAcquisitionOptions,
                                                        cacheMask,
                                                        m_cancelState);
    if (policyRequest->Status != modernapi::GetUserPolicyResultStatus::Success)
    {
        Logger::Error("UserPolicy::Acquire unsuccessful", policyRequest->Status);
        throw exceptions::RMSPFileException("The file is corrupt",
                                            exceptions::RMSPFileException::CorruptFile);
        return false;
    }

    std::shared_ptr<modernapi::UserPolicy> userPolicy = policyRequest->Policy;
    if (userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy acquisition failed");
        throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
        return false;
    }

    m_pPDFProtector->SetUserPolicy(userPolicy);

    return true;
}

PDFCryptoHandler* PDFSecurityHandlerImpl::CreateCryptoHandler()
{
    m_pCryptoHandler = new PDFCryptoHandlerImpl(m_pPDFProtector);
    return m_pCryptoHandler;
}

////////////////////////////////////////////////////////////////////////////
/// class PDFProtector

PDFProtector::PDFProtector(const std::string& originalFilePath,
                           const std::string& originalFileExtension,
                           std::shared_ptr<std::fstream> inputStream)
    : m_originalFileExtension(originalFileExtension),
      m_inputStream(inputStream),
      m_originalFilePath(originalFilePath)
{
    PDFModuleMgr::Initialize();

    m_pdfCreator = PDFCreator::Create();
}

PDFProtector::~PDFProtector()
{
}

void PDFProtector::ProtectWithTemplate(const UserContext& userContext,
                                         const ProtectWithTemplateOptions& options,
                                         std::shared_ptr<std::fstream> outputStream,
                                         std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PDFProtector::ProtectWithTemplate");
    if (!outputStream->is_open())
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
    m_userPolicy = modernapi::UserPolicy::CreateFromTemplateDescriptor(options.templateDescriptor,
                                                                       userContext.userId,
                                                                       userContext.authenticationCallback,
                                                                       userPolicyCreationOptions,
                                                                       options.signedAppData,
                                                                       cancelState);
    Protect(outputStream);
    Logger::Hidden("-PDFProtector::ProtectWithTemplate");
}

void PDFProtector::ProtectWithCustomRights(const UserContext& userContext,
                                             const ProtectWithCustomRightsOptions& options,
                                             std::shared_ptr<std::fstream> outputStream,
                                             std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PDFProtector::ProtectWithCustomRights");
    if (!outputStream->is_open())
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
    m_userPolicy = modernapi::UserPolicy::Create(
                const_cast<modernapi::PolicyDescriptor&>(options.policyDescriptor),
                userContext.userId,
                userContext.authenticationCallback,
                userPolicyCreationOptions,
                cancelState);
    Protect(outputStream);
    Logger::Hidden("-PDFProtector::ProtectWithCustomRights");
}

UnprotectResult PDFProtector::Unprotect(const UserContext& userContext,
                                          const UnprotectOptions& options,
                                          std::shared_ptr<std::fstream> outputStream,
                                          std::shared_ptr<std::atomic<bool>> cancelState)
{
    Logger::Hidden("+PDFProtector::UnProtect");
    if (!outputStream->is_open())
    {
        Logger::Error("Output stream invalid");
        throw exceptions::RMSStreamException("Output stream invalid");
    }

    std::shared_ptr<std::iostream> inputEncryptedIO = m_inputStream;
    auto inputEncrypted = rmscrypto::api::CreateStreamFromStdStream(inputEncryptedIO);

    std::shared_ptr<std::iostream> outputDecryptedIO = outputStream;
    auto outputDecrypted = rmscrypto::api::CreateStreamFromStdStream(outputDecryptedIO);

    std::string filterName = "MicrosoftIRMServices";
    PDFSecurityHandlerImpl securityHander(this, userContext, options, cancelState);
    m_pdfCreator->UnprotectCustomEncryptedFile(
                inputEncrypted,
                filterName,
                &securityHander,
                outputDecrypted);
    return rmscore::fileapi::UnprotectResult::SUCCESS;
    //end test
    //end test
    //end test

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

    //get publishing license
    std::string plPath = "C:\\Users\\foxit-dev\\Desktop\\PL.txt";
    auto inPLFile = std::make_shared<std::fstream>(
      plPath, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    inPLFile->seekg(0, std::ios::end);
    uint64_t plSize = inPLFile->tellg();
    inPLFile->seekg(0, std::ios::beg);
    std::vector<unsigned char> publishingLicense(plSize);
    inPLFile->read(reinterpret_cast<char*>(&publishingLicense[0]), plSize);
    inPLFile->close();

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
        throw exceptions::RMSPFileException("The file is corrupt",
                                            exceptions::RMSPFileException::CorruptFile);
    }

    m_userPolicy = policyRequest->Policy;
    if (m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy acquisition failed");
        throw exceptions::RMSInvalidArgumentException("User Policy acquisition failed.");
    }

    //std::shared_ptr<std::iostream> inputIO = m_inputStream;
    //auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(inputIO);

    m_inputStream->seekg(0, std::ios::end);
    uint64_t originalFileSize = m_inputStream->tellg();

    //just test. read content from original file.
    uint64_t contentSize = originalFileSize;
    char* fileContent = new char[contentSize];
    m_inputStream->seekg(std::ios::beg);
    m_inputStream->read(fileContent, contentSize);

    auto sstream = std::make_shared<std::stringstream>();
    sstream->write(fileContent, contentSize);
    std::shared_ptr<std::iostream> protectedIOS = sstream;

    auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);
    auto protectedStream = CreateProtectedStream(inputSharedStream, contentSize);

    std::shared_ptr<std::stringstream> outputSS = std::make_shared<std::stringstream>();
    std::shared_ptr<std::iostream> outputIOS = outputSS;
    auto outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(protectedIOS);

    DecryptStream(outputSharedStream, protectedStream, originalFileSize);

    outputSharedStream->Seek(std::ios::beg);
    auto nSize = outputSharedStream->Size();
    std::vector<uint8_t> decryptedData = outputSharedStream->Read(nSize);

    char bufDestSize[4];
    memset(bufDestSize, 0, 4 * sizeof(char));
    bufDestSize[0] = decryptedData[3];
    bufDestSize[1] = decryptedData[2];
    bufDestSize[2] = decryptedData[1];
    bufDestSize[3] = decryptedData[0];
    uint64_t dwTotalDestSize = 0;
    memcpy(&dwTotalDestSize, bufDestSize, 4);

    outputStream->seekp(std::ios::beg);
    outputStream->write(reinterpret_cast<const char*>(&decryptedData[0] + 4), dwTotalDestSize);

    delete [] fileContent;

    Logger::Hidden("+PDFProtector::UnProtect");
    return (UnprotectResult)policyRequest->Status;
}

bool PDFProtector::IsProtected() const
{
    Logger::Hidden("+PDFProtector::IsProtected");
    std::shared_ptr<std::iostream> inputIO = m_inputStream;
    auto inputSharedStream = rmscrypto::api::CreateStreamFromStdStream(inputIO);
    try
    {
        return false;
    }
    catch (exceptions::RMSException&)
    {
      Logger::Hidden("-PDFProtector::IsProtected");
      return false;
    }

    Logger::Hidden("-PDFProtector::IsProtected");
    return true;
}

void PDFProtector::Protect(const std::shared_ptr<std::fstream>& outputStream)
{
    if (m_userPolicy.get() == nullptr)
    {
        Logger::Error("User Policy creation failed");
        throw exceptions::RMSInvalidArgumentException("User Policy creation failed.");
    }

    std::shared_ptr<std::iostream> outputIO = outputStream;

    auto publishingLicense = m_userPolicy->SerializedPolicy();

    std::string filterName = "MicrosoftIRMServices";
    auto outputEncrypted = rmscrypto::api::CreateStreamFromStdStream(outputIO);
    PDFCryptoHandlerImpl cryptoHander(this);
    m_pdfCreator->CreateCustomEncryptedFile(
                m_originalFilePath,
                filterName,
                publishingLicense,
                &cryptoHander,
                outputEncrypted);
    return;
    //end test
    //end test
    //end test

    m_inputStream->seekg(0, std::ios::end);
    uint64_t originalFileSize = m_inputStream->tellg();

    //just test. read content from original file.
    uint64_t contentSize = originalFileSize;
    char* fileContent = new char[contentSize];
    m_inputStream->seekg(std::ios::beg);
    m_inputStream->read(fileContent, contentSize);

    uint64_t contentSizeAddPre = originalFileSize + 4;
    char* fileContentAddPre = new char[contentSizeAddPre];
    fileContentAddPre[3] = ((char*)&originalFileSize)[0];
    fileContentAddPre[2] = ((char*)&originalFileSize)[1];
    fileContentAddPre[1] = ((char*)&originalFileSize)[2];
    fileContentAddPre[0] = ((char*)&originalFileSize)[3];
    memcpy(fileContentAddPre + 4, fileContent, originalFileSize);

    /*MemoryReadStreamBuf inputStreamBuf(fileContent, contentSize);
    std::shared_ptr<std::istream> inputIStream =
            std::make_shared<std::istream>(&inputStreamBuf);
            */

    /*MemoryWriteStreamBuf outputStreamBuf(contentSize);
    std::shared_ptr<std::iostream> outputIStream =
            std::make_shared<std::iostream>(&outputStreamBuf);
            */

    //write publishing license
    std::string plPath = "C:\\Users\\foxit-dev\\Desktop\\PL.txt";
    auto outPLFile = std::make_shared<std::fstream>(
      plPath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    outPLFile->write(reinterpret_cast<const char*>(&publishingLicense[0]), publishingLicense.size());
    outPLFile->close();

    auto sstream = std::make_shared<std::stringstream>();
    std::shared_ptr<std::iostream> outputIOS = sstream;

    auto outputSharedStream = rmscrypto::api::CreateStreamFromStdStream(outputIOS);
    auto protectedStream = CreateProtectedStream(outputSharedStream, contentSize);
    EncryptStream(fileContentAddPre, contentSizeAddPre, protectedStream, true);

    outputSharedStream->Seek(std::ios::beg);
    auto nSize = outputSharedStream->Size();
    std::vector<uint8_t> encryptedData = outputSharedStream->Read(nSize);

    //std::string encryptedData = sstream->str();
    //auto encryptedDataLen = encryptedData.length();
    outputIO->seekp(std::ios::beg);
    //outputIO->write(encryptedData.c_str(), encryptedDataLen);
    outputIO->write(reinterpret_cast<const char*>(&encryptedData[0]), nSize);

    /*uint8_t *pbBuffer = nullptr;
    int64_t cbBuffer = protectedStream->Size();
    pbBuffer = new uint8_t[cbBuffer];
    memset(pbBuffer, 0, cbBuffer);
    protectedStream->Seek(std::ios::beg);
    protectedStream->Read(pbBuffer, cbBuffer);
    outputIO->write(reinterpret_cast<const char*>(pbBuffer), cbBuffer);
    delete [] pbBuffer;*/

    delete [] fileContent;
    delete [] fileContentAddPre;
}

std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> PDFProtector::CreateProtectedStream(
        const rmscrypto::api::SharedStream& stream,
        uint64_t contentSize)
{
    auto protectionPolicy = m_userPolicy->GetImpl();
    if (protectionPolicy->GetCipherMode() == rmscrypto::api::CipherMode::CIPHER_MODE_ECB)
    {
        // Older versions of the SDK ignored ECB cipher mode when encrypting pfile format.
        protectionPolicy->ReinitilizeCryptoProvider(rmscrypto::api::CipherMode::CIPHER_MODE_CBC4K);
    }

    auto cryptoProvider = m_userPolicy->GetImpl()->GetCryptoProvider();
    m_blockSize = cryptoProvider->GetBlockSize();
    // Cache block size to be 512 for cbc512, 4096 for cbc4k and ecb
    uint64_t protectedStreamBlockSize = m_blockSize == 512 ? 512 : 4096;
    auto backingStreamImpl = stream->Clone();
    uint64_t contentStartPosition = 0;
    return rmscrypto::api::BlockBasedProtectedStream::Create(cryptoProvider,
                                                             backingStreamImpl,
                                                             contentStartPosition,
                                                             contentSize,
                                                             protectedStreamBlockSize);
}

void PDFProtector::EncryptStream(
        char* pBuffer,
        uint32_t bufferSize,
        const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
        bool bFinish)
{
    if(nullptr == pBuffer || 0 == bufferSize)
    {
        if(bFinish)
        {
            pStream->Flush();
        }
        return;
    }

    uint32_t contentSize = bufferSize;

    uint64_t bufSize = 4096;    //should be a multiple of AES block size (16)
    std::vector<uint8_t> buffer(bufSize);
    uint64_t readPosition  = 0;
    uint64_t writePosition = 0;
    bool isECB = m_userPolicy->DoesUseDeprecatedAlgorithms();
    uint64_t totalSize = isECB? ((contentSize + m_blockSize - 1) & ~(m_blockSize - 1)) :
                                contentSize;

    //int index = 0;
    while(totalSize - readPosition > 0)
    {
        uint64_t offsetRead  = readPosition;
        uint64_t offsetWrite = writePosition;

        /*uint64_t toProcess = 9;
        if(index == 1)
        {
            toProcess = 10;
        }
        index ++;*/
        uint64_t toProcess   = std::min(bufSize, totalSize - readPosition);
        readPosition  += toProcess;
        writePosition += toProcess;

        //stdInputStream->seekg(offsetRead);
        //stdInputStream->read(reinterpret_cast<char *>(&buffer[0]), toProcess);
        memcpy(reinterpret_cast<char *>(&buffer[0]), pBuffer + offsetRead, toProcess);

        auto written = pStream->WriteAsync(
                    buffer.data(), toProcess, offsetWrite, std::launch::deferred).get();

        uint64_t size1 = pStream->Size();
        if (written != toProcess)
        {
          throw exceptions::RMSStreamException("Error while writing data");
        }
    }

    if(bFinish)
    {
        pStream->Flush();
        uint64_t size2 = pStream->Size();
    }
}

void PDFProtector::DecryptStream(const rmscrypto::api::SharedStream& outputIOS,
        const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
        uint64_t originalContentSize)
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
        uint64_t originalRemaining = std::min(bufSize, originalContentSize - readPosition);
        readPosition  += toProcess;
        writePosition += toProcess;

        auto read = pStream->ReadAsync(
                    &buffer[0], toProcess, offsetRead, std::launch::deferred).get();
        if (read == 0)
        {
          break;
        }

        outputIOS->Seek(offsetWrite);
        outputIOS->Write(buffer.data(), originalRemaining);
    }
    outputIOS->Flush();
}

modernapi::UserPolicyCreationOptions PDFProtector::ConvertToUserPolicyCreationOptions(
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

void PDFProtector::SetUserPolicy(std::shared_ptr<modernapi::UserPolicy> userPolicy)
{
    m_userPolicy = userPolicy;
}

} // namespace fileapi
} // namespace rmscore
