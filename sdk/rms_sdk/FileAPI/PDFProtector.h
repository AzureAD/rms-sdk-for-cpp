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

#define MIN_RAW_SIZE 64 * 1024 * 1024

class PDFProtector;

/**
 * @brief The implementaion class of interface class PDFCryptoHandler defined by PDF object model.
 * Please refer to comments of PDFCryptoHandler.
 * The interface header file of PDF object model is PDFObjectModel.h.
 */
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

/**
 * @brief The implementaion class of interface class PDFSecurityHandler defined by PDF object model.
 * Please refer to comments of PDFSecurityHandler.
 * The interface header file of PDF object model is PDFObjectModel.h.
 */
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

/**
 * @brief The wrapper info used to create the unencrypted wrapper document
 * according to "Microsoft IRM protection for PDF Spec-v2".
 */
#define PDF_PROTECTOR_FILTER_NAME       "MicrosoftIRMServices"
#define PDF_PROTECTOR_WRAPPER_SUBTYPE   L"MicrosoftIRMServices"
#define PDF_PROTECTOR_WRAPPER_FILENAME  L"MicrosoftIRMServices Protected PDF.pdf"
#define PDF_PROTECTOR_WRAPPER_DES       L"This embedded file is encrypted using MicrosoftIRMServices filter"
#define PDF_PROTECTOR_WRAPPER_VERSION   2

/**
 * @brief The PDF protector supports the protection of PDF IRM V2 and the un-protection of
 * PDF IRM V2 and PDF IRM V1, according to "Microsoft IRM protection for PDF Spec-v2".
 * The PDF protector is on the base of PDF object model.
 * The interface header file of PDF object model is PDFObjectModel.h.
 * We have to invoke pdfobjectmodel::PDFModuleMgr::Initialize to initialize the PDF object model first.
 */
class PDFProtector : public ProtectorWithWrapper
{
public:
    /**
     * @brief Constructs the protector to protect or unprotect a PDF document.
     * We have to invoke pdfobjectmodel::PDFModuleMgr::Initialize to initialize the PDF object model first.
     * @param[in] originalFilePath      The input PDF file path to be protected.
     * @param[in] originalFileExtension The input file extension name.
     * @param[in] inputStream           The protector will read the original PDF document data from the input file stream.
     * @return void.
     */
    PDFProtector(const std::string& originalFilePath,
                 const std::string& originalFileExtension,
                 std::shared_ptr<std::fstream> inputStream);

    ~PDFProtector();

    /**
     * @brief Sets the PDF wrapper document to wrap up the encrypted PDF document.
     * If the PDF wrapper document is invalid, ProtectWithTemplate will fail and throw an exception.
     * @param[in] inputWrapperStream      The protector will read the PDF wrapper document data from the input file stream.
     * @return void.
     */
    void SetWrapper(std::shared_ptr<std::fstream> inputWrapperStream) override;

    /**
     * @brief The protector protects the PDF document with the official rights policy template.
     * First, the protector invokes pdfobjectmodel::PDFCreator::CreateCustomEncryptedFile to create
     * custom encrypted PDF document(be opposed to standard encryption like password encryption or certificate encryption).
     * Second, the protector invokes pdfobjectmodel::PDFUnencryptedWrapperCreator relevant interfaces to create final
     * PDF IRM V2 document wrapped up with unencrypted wrapper document.
     * @param[in] userContext    The input user context.
     * @param[in] options        The input options.
     * @param[in] outputStream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
     * @param[in] cancelState    It is used to check the process abandon.
     * @return void.
     */
    void ProtectWithTemplate(const UserContext& userContext,
                             const ProtectWithTemplateOptions& options,
                             std::shared_ptr<std::fstream> outputStream,
                             std::shared_ptr<std::atomic<bool>> cancelState) override;

    /**
     * @brief The protector protects the PDF document with the custom rights policy.
     * First, the protector invokes pdfobjectmodel::PDFCreator::CreateCustomEncryptedFile to create
     * custom encrypted PDF document(be opposed to standard encryption like password encryption or certificate encryption).
     * Second, the protector invokes pdfobjectmodel::PDFUnencryptedWrapperCreator relevant interfaces to create final
     * PDF IRM V2 document wrapped up with unencrypted wrapper document.
     * @param[in] userContext    The input user context.
     * @param[in] options        The input options.
     * @param[in] outputStream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
     * @param[in] cancelState    It is used to check the process abandon.
     * @return void.
     */
    void ProtectWithCustomRights(const UserContext& userContext,
                                 const ProtectWithCustomRightsOptions& options,
                                 std::shared_ptr<std::fstream> outputStream,
                                 std::shared_ptr<std::atomic<bool>> cancelState) override;

    /**
     * @brief The protector unprotects the PDF document. It supports PDF IRM V2&V1 format.
     * First, the protector invokes pdfobjectmodel::PDFWrapperDoc relevant interfaces to check whether
     * the input document is rms-protected.
     * Second, the protector invokes pdfobjectmodel::PDFWrapperDoc::StartGetPayload
     * to obtain the encrypted PDF document.
     * Third, the protector invokes pdfobjectmodel::PDFCreator::UnprotectCustomEncryptedFile to
     * decrypt the encrypted PDF document.
     * @param[in] userContext    The input user context.
     * @param[in] options        The input options.
     * @param[in] outputStream   It receives the final PDF IRM V2 document wrapped up with unencrypted wrapper document.
     * @param[in] cancelState    It is used to check the process abandon.
     * @return The unprotection result.
     */
    UnprotectResult Unprotect(const UserContext& userContext,
                              const UnprotectOptions& options,
                              std::shared_ptr<std::fstream> outputStream,
                              std::shared_ptr<std::atomic<bool>> cancelState) override;

    /**
     * @brief The protector invokes pdfobjectmodel::PDFWrapperDoc relevant interfaces to
     * check whether the input PDF document is protected or not.
     * @return true if the document is protected, otherwise false.
     */
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

    std::shared_ptr<std::fstream> m_inputWrapperStream;
    std::unique_ptr<PDFCreator> m_pdfCreator;
    std::unique_ptr<PDFUnencryptedWrapperCreator> m_pdfWrapperCreator;
};

} // namespace fileapi
} // namespace rmscore

#endif // RMS_SDK_FILE_API_PDFPROTECTOR_H

