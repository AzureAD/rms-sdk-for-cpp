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

class PDFModuleMgr
{
public:
    DLL_PUBLIC_RMS
    static bool Initialize();

    virtual ~PDFModuleMgr(){}

protected:
    PDFModuleMgr(){}
};

class PDFBinaryBuf
{
public:
    virtual bool AppendBlock(const void* pBuf, uint32_t size) = 0;
};

#define  PDFWRAPPERDOC_TYPE_UNKNOWN -1	/** Unknown. */
#define  PDFWRAPPERDOC_TYPE_NORMAL	0	/** Normal document. */
#define  PDFWRAPPERDOC_TYPE_IRMV1	1	/** For IRM V1 wrapper document. */
#define  PDFWRAPPERDOC_TYPE_IRMV2	2	/** For IRM V2 wrapper document. */

class PDFWrapperDoc
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<PDFWrapperDoc> Create(rmscrypto::api::SharedStream inputStream);

    virtual uint32_t GetWrapperType() const = 0;

    virtual bool GetCryptographicFilter(std::wstring& wsGraphicFilter, float &fVersion) const = 0;

    virtual uint32_t GetPayLoadSize() const = 0;

    virtual bool GetPayloadFileName(std::wstring& wsFileName) const = 0;

    virtual bool StartGetPayload(rmscrypto::api::SharedStream outputStream) = 0;

    virtual ~PDFWrapperDoc(){}

protected:
    PDFWrapperDoc(){}
};

class PDFUnencryptedWrapperCreator
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<PDFUnencryptedWrapperCreator> Create(
            rmscrypto::api::SharedStream wrapperDocStream);

    virtual void SetPayloadInfo(
            const std::wstring& wsSubType,
            const std::wstring& wsFileName,
            const std::wstring& wsDescription,
            float fVersion) = 0;

    virtual void SetPayLoad(rmscrypto::api::SharedStream inputStream) = 0;

    virtual bool CreateUnencryptedWrapper(rmscrypto::api::SharedStream outputStream) = 0;

    virtual ~PDFUnencryptedWrapperCreator(){}

protected:
    PDFUnencryptedWrapperCreator(){}
};

class PDFCryptoHandler
{
public:

    virtual uint32_t DecryptGetSize(uint32_t src_size) = 0;

    virtual void* DecryptStart(uint32_t objnum, uint32_t gennum) = 0;

    virtual bool DecryptStream(void* context, char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf) = 0;

    virtual bool DecryptFinish(void* context, PDFBinaryBuf* dest_buf) = 0;

    virtual uint32_t EncryptGetSize(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size) = 0;

    virtual bool EncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, char* dest_buf, uint32_t* dest_size) = 0;

    virtual bool ProgressiveEncryptStart(uint32_t objnum, uint32_t version, uint32_t raw_size) = 0;

    virtual bool ProgressiveEncryptContent(uint32_t objnum, uint32_t version, char* src_buf, uint32_t src_size, PDFBinaryBuf* dest_buf) = 0;

    virtual bool ProgressiveEncryptFinish(PDFBinaryBuf* dest_buf) = 0;
};

class PDFSecurityHandler
{
public:
    virtual bool OnInit(unsigned char* publishingLicense, uint32_t plSize) = 0;

    virtual PDFCryptoHandler* CreateCryptoHandler() = 0;
};

#define PDFCREATOR_ERR_SUCCESS				0
#define PDFCREATOR_ERR_FILE					1
#define PDFCREATOR_ERR_FORMAT				2
#define PDFCREATOR_ERR_SECURITY				3
#define PDFCREATOR_ERR_CREATOR				4
#define PDFCREATOR_ERR_UNKNOWN				5

class PDFCreator
{
public:
    DLL_PUBLIC_RMS
    static std::unique_ptr<PDFCreator> Create();

    virtual uint32_t CreateCustomEncryptedFile(
            const std::string& inputFilePath,
            const std::string& filterName,
            const std::vector<unsigned char>& publishingLicense,
            PDFCryptoHandler* cryptoHander,
            rmscrypto::api::SharedStream outputIOS) = 0;

    virtual uint32_t UnprotectCustomEncryptedFile(
            rmscrypto::api::SharedStream inputIOS,
            const std::string& filterName,
            PDFSecurityHandler* securityHander,
            rmscrypto::api::SharedStream outputIOS) = 0;

    virtual ~PDFCreator(){}

protected:
    PDFCreator(){}
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMS_SDK_PDF_OBJECT_MODEL_H

