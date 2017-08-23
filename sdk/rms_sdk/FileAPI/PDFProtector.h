/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_FILE_API_PDFPROTECTOR_H
#define RMS_SDK_FILE_API_PDFPROTECTOR_H

#include "Protector.h"
#include "BlockBasedProtectedStream.h"
#include "FileAPIStructures.h"
#include "UserPolicy.h"
#include "PDFObjectModel.h"
#include "CryptoAPI.h"

using namespace rmscore::pdfobjectmodel;

namespace rmscore {
namespace fileapi {

class MemoryWriteStreamBuf: public std::streambuf
{
public:
    MemoryWriteStreamBuf(uint32_t bufferSize)
    {
        m_pBuffer = new char[bufferSize];
        m_bufferSize = bufferSize;
        setg(m_pBuffer, m_pBuffer, m_pBuffer + m_bufferSize);
        setp(m_pBuffer, m_pBuffer + m_bufferSize);
    }

    virtual ~MemoryWriteStreamBuf()
    {
        if(nullptr != m_pBuffer)
        {
            delete [] m_pBuffer;
            m_pBuffer = nullptr;
        }
    }

public:
    char* m_pBuffer;
    uint32_t m_bufferSize;
};

class MemoryReadStreamBuf: public std::streambuf
{
public:
    MemoryReadStreamBuf(std::shared_ptr<char> pBuffer, uint32_t bufferSize)
        : m_bufferSize(bufferSize)
    {
        m_pBuffer = pBuffer.get();
        setg(m_pBuffer, m_pBuffer, m_pBuffer + m_bufferSize);
        setp(m_pBuffer, m_pBuffer + m_bufferSize);
    }

    virtual ~MemoryReadStreamBuf()
    {

    }

public:
    char* m_pBuffer;
    uint32_t m_bufferSize;
};

#define MIN_RAW_SIZE 1//64 * 1024 * 1024

class PDFProtector;
class PDFCryptoHandlerImpl : public PDFCryptoHandler
{
public:
    PDFCryptoHandlerImpl(PDFProtector* pPDFProtector);
    virtual ~PDFCryptoHandlerImpl();

    virtual uint32_t DecryptGetSize(uint32_t src_size);

    virtual void* DecryptStart(uint32_t objnum, uint32_t gennum);

    virtual bool DecryptStream(void* context, char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf);

    virtual bool DecryptFinish(void* context, PDFBinaryBuf* dest_buf);

    virtual uint32_t EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size);

    virtual bool EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size);

    virtual bool ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size);

    virtual bool ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf);

    virtual bool ProgressiveEncryptFinish(PDFBinaryBuf* dest_buf);

private:
    PDFProtector* m_pPDFProtector;

    //for decryption
    std::shared_ptr<std::stringstream> m_dataToDecrypted;
    uint32_t m_objnum;

    //for encryption
    bool m_bProgressiveStart;
    std::shared_ptr<std::iostream> m_outputIOS;
    rmscrypto::api::SharedStream m_outputSharedStream;
    std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> m_sharedProtectedStream;
};

class PDFSecurityHandlerImpl : public PDFSecurityHandler
{
public:
    PDFSecurityHandlerImpl(PDFProtector* pPDFProtector,
                         const UserContext& userContext,
                         const UnprotectOptions& options,
                         std::shared_ptr<std::atomic<bool>> cancelState);
    virtual ~PDFSecurityHandlerImpl();

    virtual bool OnInit(unsigned char* publishingLicense, uint32_t plSize);

    virtual PDFCryptoHandler* CreateCryptoHandler();

private:
    PDFProtector* m_pPDFProtector;
    PDFCryptoHandlerImpl* m_pCryptoHandler;
    UserContext m_userContext;
    UnprotectOptions m_options;
    std::shared_ptr<std::atomic<bool>> m_cancelState;
};

class PDFProtector : public Protector
{
public:
    PDFProtector(const std::string& originalFilePath,
                 const std::string& originalFileExtension,
                 std::shared_ptr<std::fstream> inputStream);

    ~PDFProtector();

    void ProtectWithTemplate(const UserContext& userContext,
                             const ProtectWithTemplateOptions& options,
                             std::shared_ptr<std::fstream> outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState) override;

    void ProtectWithCustomRights(const UserContext& userContext,
                                 const ProtectWithCustomRightsOptions& options,
                                 std::shared_ptr<std::fstream> outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState) override;

    UnprotectResult Unprotect(const UserContext& userContext,
                              const UnprotectOptions& options,
                              std::shared_ptr<std::fstream> outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState) override;

    bool IsProtected() const override;

    std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream> CreateProtectedStream(
            const rmscrypto::api::SharedStream& stream,
            uint64_t contentSize);

    void EncryptStream(char* pBuffer,
                       uint32_t bufferSize,
                       const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
                       bool bFinish);

    void DecryptStream(const rmscrypto::api::SharedStream& outputIOS,
                       const std::shared_ptr<rmscrypto::api::BlockBasedProtectedStream>& pStream,
                       uint64_t originalContentSize);

    void SetUserPolicy(std::shared_ptr<modernapi::UserPolicy> userPolicy);

private:
    void Protect(const std::shared_ptr<std::fstream>& outputStream);
    modernapi::UserPolicyCreationOptions ConvertToUserPolicyCreationOptions(
            const bool& allowAuditedExtraction,
            CryptoOptions cryptoOptions);

    std::string m_originalFileExtension;
    std::string m_originalFilePath;
    std::shared_ptr<std::fstream> m_inputStream;
    uint32_t m_blockSize;
    std::shared_ptr<modernapi::UserPolicy> m_userPolicy;

    std::unique_ptr<PDFCreator> m_pdfCreator;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PDFPROTECTOR_H

