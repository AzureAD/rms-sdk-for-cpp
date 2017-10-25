/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef RMS_SDK_PDF_OBJECT_MODEL_H
#define RMS_SDK_PDF_OBJECT_MODEL_H

#include <string>
#include <memory>
#include "ModernAPIExport.h"
#include "CryptoAPI.h"

namespace rmscore {
namespace pdfobjectmodel {

/**
 * @brief The PDF object model is designed to parse PDF document and creat PDF document.
 * It is on the base of Foxit PDF core.
 * We can create custom encrypted PDF file(be opposed to standard encryption like
 * password encryption or certificate encryption) and decrypt it through PDFCreator
 * revelant interfaces, according to "Microsoft IRM protection for PDF Spec-v2".
 * We can wrap up the encrypted PDF file with the wrapper document through PDFUnencryptedWrapperCreator
 * relevant interfaces, according to "Microsoft IRM protection for PDF Spec-v2".
 * We can checks the encrypted PDF document wrapped up with wrapper document through PDFWrapperDoc
 * relevant interfaces.
 * The other interface classes are assistant classes, like PDFBinaryBuf, PDFCryptoHandler, PDFSecurityHandler.
 *
 * IMPORTANT !!! IMPORTANT !!! IMPORTANT !!!
 * Saying important thing three times. ^-^
 * Firstly, we MUST initialize the PDF object model through PDFModuleMgr::Initialize.
 */
class PDFModuleMgr
{
public:
    DLL_PUBLIC_RMS
    static bool Initialize();

    virtual ~PDFModuleMgr(){}

protected:
    PDFModuleMgr(){}
};

/**
 * @brief It is used in PDFCryptoHandler to deal with the binary buf.
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFBinaryBuf
{
public:
    /**
     * @brief Appends some buffer to the existing buffer.
     * @param[in] pBuf    The input buffer to be appended.
     * @param[in] size    The input buffer size.
     * @return true for success, otherwise false.
     */
    virtual bool AppendBlock(const void* pBuf, uint32_t size) = 0;
};

#define  PDFWRAPPERDOC_TYPE_UNKNOWN -1	/** Unknown. */
#define  PDFWRAPPERDOC_TYPE_NORMAL	0	/** Normal document. */
#define  PDFWRAPPERDOC_TYPE_IRMV1	1	/** For IRM V1 wrapper document. */
#define  PDFWRAPPERDOC_TYPE_IRMV2	2	/** For IRM V2 wrapper document. */

/**
 * @brief It is used to checks the encrypted PDF document wrapped up with wrapper document.
 * We can get the wrapper type, payload size, payload content and so on.
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFWrapperDoc
{
public:
    /**
     * @brief Creates the PDFWrapperDoc instance.
     * @param[in] inputStream    The input wrapper document.
     * @return The PDFWrapperDoc instance.
     */
    DLL_PUBLIC_RMS
    static std::unique_ptr<PDFWrapperDoc> Create(rmscrypto::api::SharedStream inputStream);

    /**
     * @brief Gets the wrapper type.
     * @return PDFWRAPPERDOC_TYPE_IRMV2 for PDF IRM V2 format.
     * PDFWRAPPERDOC_TYPE_IRMV1 for PDF IRM V1 format.
     */
    virtual uint32_t GetWrapperType() const = 0;

    /**
     * @brief Gets the cryptographic filter name and version.
     * @param[out] wsGraphicFilter  It receives the cryptographic filter name.
     * @param[out] fVersion         It receives the cryptographic filter version.
     * @return true for success, otherwise false.
     */
    virtual bool GetCryptographicFilter(std::wstring& wsGraphicFilter, float &fVersion) const = 0;

    /**
     * @brief Gets the payload size.
     * @return The payload size.
     */
    virtual uint32_t GetPayLoadSize() const = 0;

    /**
     * @brief Gets the payload file name.
     * @param[out] wsFileName  It receives the payload file name.
     * @return true for success, otherwise false.
     */
    virtual bool GetPayloadFileName(std::wstring& wsFileName) const = 0;

    /**
     * @brief Gets the payload content.
     * @param[out] outputStream  It receives the payload content.
     * @return true for success, otherwise false.
     */
    virtual bool StartGetPayload(rmscrypto::api::SharedStream outputStream) = 0;

    virtual ~PDFWrapperDoc(){}

protected:
    PDFWrapperDoc(){}
};

/**
 * @brief It is used to wrap up the encrypted PDF file with the wrapper document.
 * First, we have to set the payload info and the payload content. Then invoke
 * CreateUnencryptedWrapper to output the final wrapped up PDF document.
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFUnencryptedWrapperCreator
{
public:
    /**
     * @brief Creates the PDFUnencryptedWrapperCreator instance.
     * @param[in] wrapperDocStream    The input wrapper document.
     * @return The PDFUnencryptedWrapperCreator instance.
     */
    DLL_PUBLIC_RMS
    static std::unique_ptr<PDFUnencryptedWrapperCreator> Create(
            rmscrypto::api::SharedStream wrapperDocStream);

    /**
     * @brief Sets the payload info.
     * @param[in] wsSubType     The name of the cryptographic filter used to encrypt the encrypted payload document.
     * @param[in] wsFileName    The file name for encrypted payload document.
     * @param[in] wsDescription Description text for the embedded encrypted payload document.
     * @param[in] fVersion      The version number of the cryptographic filter.
     * @return void.
     */
    virtual void SetPayloadInfo(
            const std::wstring& wsSubType,
            const std::wstring& wsFileName,
            const std::wstring& wsDescription,
            float fVersion) = 0;

    /**
     * @brief Sets the payload content.
     * @param[in] inputStream   The input payload content.
     * @return void.
     */
    virtual void SetPayLoad(rmscrypto::api::SharedStream inputStream) = 0;

    /**
     * @brief Writes the wrapper doc to a custom file access.
     * @param[out] outputStream  It receives the wrapper doc.
     * @return true for success, otherwise false.
     */
    virtual bool CreateUnencryptedWrapper(rmscrypto::api::SharedStream outputStream) = 0;

    virtual ~PDFUnencryptedWrapperCreator(){}

protected:
    PDFUnencryptedWrapperCreator(){}
};

/**
 * @brief This class is responsible for PDF data encryption and decryption.
 * It is implemented by PDF protector, and set it to CreateCustomEncryptedFile
 * or returned by PDFSecurityHandler::CreateCryptoHandler.
 * It will be invoked by PDF object model to pass the PDF data to PDF protector,
 * then PDF protector process the PDF data(encryption or decryption), and then
 * PDF protector feeds back the data to PDF object model.
 */
class PDFCryptoHandler
{
public:

    /**
     * @brief The PDF object model passes the encrypted data size to PDF protector and the
     * PDF protector feeds back the decrypted data size so that the PDF object model can prepare
     * the memory to receive the decrypted data.
     */
    virtual uint32_t DecryptGetSize(uint32_t src_size) = 0;

    virtual void DecryptStart(uint32_t objnum, uint32_t gennum) = 0;

    virtual bool DecryptStream(char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf) = 0;

    virtual bool DecryptFinish(PDFBinaryBuf* dest_buf) = 0;

    /**
     * @brief The PDF object model passes the plain data size to PDF protector and the
     * PDF protector feeds back the encrypted data size so that the PDF object model can prepare
     * the memory to receive the encrypted data.
     *
     * IMPORTANT !!! IMPORTANT !!! IMPORTANT !!!
     * EncryptGetSize and EncryptContent are one group dealing with the one time encryption.
     * ProgressiveEncryptStart, ProgressiveEncryptContent and ProgressiveEncryptFinish are one
     * group dealing with the progressive encryption if the data size is large.
     * The return value of ProgressiveEncryptStart determines which mode is adopted. If it returns
     * true, the progressive mode is adopted, otherwise the one time mode is adopted.
     */
    virtual uint32_t EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size) = 0;

    virtual bool EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size) = 0;

    virtual bool ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size) = 0;

    virtual bool ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf) = 0;

    virtual bool ProgressiveEncryptFinish(PDFBinaryBuf* dest_buf) = 0;
};

/**
 * @brief This class is responsible for PDF security handler.
 * It is implemented by PDF protector, and set it to UnprotectCustomEncryptedFile.
 * It will be invoked by PDF object model to pass the publishing license to PDF protector
 * to check the validity and to create crypto hander for PDF data decryption.
 */
class PDFSecurityHandler
{
public:
    /**
     * @brief The PDF protector checks the publishing license validity here. If false
     * is returned, UnprotectCustomEncryptedFile will fail.
     */
    virtual bool OnInit(unsigned char* publishingLicense, uint32_t plSize) = 0;

    virtual std::shared_ptr<PDFCryptoHandler> CreateCryptoHandler() = 0;
};

#define PDFCREATOR_ERR_SUCCESS				0
#define PDFCREATOR_ERR_FILE					1
#define PDFCREATOR_ERR_FORMAT				2
#define PDFCREATOR_ERR_SECURITY				3
#define PDFCREATOR_ERR_CREATOR				4
#define PDFCREATOR_ERR_UNKNOWN				5

/**
 * @brief It is used to create custom encrypted PDF file(be opposed to standard encryption like
 * password encryption or certificate encryption) and decrypt it, according to
 * "Microsoft IRM protection for PDF Spec-v2".
 * It is implemented by PDF object model, and it is invoked by PDF protector.
 */
class PDFCreator
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<PDFCreator> Create();

    /**
     * @brief Creates custom encrypted PDF file(be opposed to standard encryption like
     * password encryption or certificate encryption).
     * @param[in] inputFilePath     The input file path to be encrypted.
     * @param[in] filterName        The filter name, according to "Microsoft IRM protection for PDF Spec-v2".
     * @param[in] publishingLicense The publishing license stored in PDF file, according to "Microsoft IRM protection for PDF Spec-v2".
     * @param[in] cryptoHander      The crypto handler that is responsible for PDF data encryption.
     * @param[out] outputIOS        It receives the encrypted PDF documnet.
     * @return PDFCREATOR_ERR_SUCCESS for success, otherwise the other error code.
     */
    virtual uint32_t CreateCustomEncryptedFile(
            const std::string& inputFilePath,
            const std::string& filterName,
            const std::vector<unsigned char>& publishingLicense,
            std::shared_ptr<PDFCryptoHandler> cryptoHander,
            rmscrypto::api::SharedStream outputIOS) = 0;

    /**
     * @brief Decrypts the custom encrypted PDF file(be opposed to standard encryption like
     * password encryption or certificate encryption).
     * @param[in] inputIOS          The input custom encrypted PDF document.
     * @param[in] filterName        The filter name, according to "Microsoft IRM protection for PDF Spec-v2".
     * @param[in] securityHander    The securty handler used to check the validity and to create crypto hander for PDF data decryption.
     * @param[out] outputIOS        It receives the decrypted PDF documnet.
     * @return PDFCREATOR_ERR_SUCCESS for success, otherwise the other error code.
     */
    virtual uint32_t UnprotectCustomEncryptedFile(
            rmscrypto::api::SharedStream inputIOS,
            const std::string& filterName,
            std::shared_ptr<PDFSecurityHandler> securityHander,
            rmscrypto::api::SharedStream outputIOS) = 0;

    virtual ~PDFCreator(){}

protected:
    PDFCreator(){}
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMS_SDK_PDF_OBJECT_MODEL_H

