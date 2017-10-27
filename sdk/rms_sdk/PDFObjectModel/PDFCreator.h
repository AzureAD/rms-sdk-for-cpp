#ifndef RMSSDK_PDFOBJECTMODEL_PDFCREATOR_H_
#define RMSSDK_PDFOBJECTMODEL_PDFCREATOR_H_

#include "PDFObjectModel.h"
#include "Core.inc"

namespace rmscore {
namespace pdfobjectmodel {

#define PROGRESSIVE_ENCRYPT_TEMP_FILE L".RMS.PE.temp"

/**
 * @brief The implementaion class of interface class CPDF_CryptoHandler of Foxit core.
 * CPDF_CryptoHandler is the abstract class for PDF cryptographic operations (encryption and decryption).
 * This class works with security handler which provides algorithm and key info.
 * Please refer to comments of CPDF_CryptoHandler.
 */
class CustomCryptoHandler : public CPDF_CryptoHandler
{
public:
    explicit CustomCryptoHandler(std::shared_ptr<PDFCryptoHandler> pPDFCryptoHandler);
	virtual ~CustomCryptoHandler();

public:
	virtual FX_BOOL		Init(CPDF_Dictionary* pEncryptDict, CPDF_SecurityHandler* pSecurityHandler);

	virtual FX_DWORD	DecryptGetSize(FX_DWORD src_size);

	virtual FX_LPVOID	DecryptStart(FX_DWORD objnum, FX_DWORD gennum);

	virtual FX_BOOL		DecryptStream(FX_LPVOID context, FX_LPCBYTE src_buf, FX_DWORD src_size, CFX_BinaryBuf& dest_buf);

	virtual FX_BOOL		DecryptFinish(FX_LPVOID context, CFX_BinaryBuf& dest_buf);

	virtual FX_DWORD	EncryptGetSize(FX_DWORD objnum, FX_DWORD version, FX_LPCBYTE src_buf, FX_DWORD src_size);

	virtual FX_BOOL		EncryptContent(FX_DWORD objnum, FX_DWORD version, FX_LPCBYTE src_buf, FX_DWORD src_size,
		FX_LPBYTE dest_buf, FX_DWORD& dest_size);

	virtual FX_BOOL		ProgressiveEncryptStart(FX_DWORD objnum, FX_DWORD version, FX_DWORD raw_size, FX_BOOL bFlateEncode);

	virtual FX_BOOL     ProgressiveEncryptContent(FX_INT32 objnum, FX_DWORD version, FX_LPCBYTE src_buf, FX_DWORD src_size, CFX_BinaryBuf& dest_buf);

	virtual FX_BOOL     ProgressiveEncryptFinish(CFX_BinaryBuf& dest_buf);

protected:
    std::shared_ptr<PDFCryptoHandler> m_pPDFCryptoHandler;
};

/**
 * @brief The implementaion class of interface class CPDF_ProgressiveEncryptHandler of Foxit core.
 * CPDF_ProgressiveEncryptHandler is set to deal with progressive encryption.
 * Please refer to comments of CPDF_ProgressiveEncryptHandler.
 */
class CustomProgressiveEncryptHandler : public CPDF_ProgressiveEncryptHandler
{
public:
    explicit CustomProgressiveEncryptHandler(CFX_WideString wsTempPath);

    virtual ~CustomProgressiveEncryptHandler();

    virtual IFX_FileStream* GetTempFile();

    virtual void ReleaseTempFile(IFX_FileStream* pFile);

    virtual FX_BOOL SetCryptoHandler(CPDF_CryptoHandler* pCryptoHandler);

    virtual FX_DWORD EncryptGetSize(FX_DWORD objnum, FX_DWORD gennum, FX_LPCBYTE src_buf, FX_DWORD src_size);

    virtual FX_LPVOID EncryptStart(FX_DWORD objnum, FX_DWORD gennum, FX_DWORD raw_size, FX_BOOL bFlateEncode);

    virtual FX_BOOL EncryptStream(FX_LPVOID context, FX_LPCBYTE src_buf, FX_DWORD src_size, IFX_FileStream* pDest);

    virtual FX_BOOL EncryptFinish(FX_LPVOID context, IFX_FileStream* pDest);

    virtual FX_BOOL UpdateFilter(CPDF_Dictionary* pDict);

 private:
    CFX_WideString m_wsTempPath;
    CustomCryptoHandler* crypto_handler_;
    FX_DWORD m_dwObjNum;
    FX_DWORD m_dwVersion;
};

/**
 * @brief The implementaion class of interface class CPDF_SecurityHandler of Foxit core.
 * CPDF_SecurityHandler is the abstract Abstract class for all security handlers, contains
 * virtual functions to be implemented by derived classes.
 * Please refer to comments of CPDF_SecurityHandler.
 */
class CustomSecurityHandler : public CPDF_SecurityHandler
{
public:
    explicit CustomSecurityHandler(std::shared_ptr<PDFSecurityHandler> pPDFSecHandler);

    virtual ~CustomSecurityHandler();

    virtual FX_BOOL OnInit(CPDF_Parser* pParser, CPDF_Dictionary* pEncryptDict);

    virtual FX_DWORD GetPermissions();

    virtual FX_BOOL IsOwner();

    virtual FX_BOOL GetCryptInfo(int& cipher, FX_LPCBYTE& buffer, int& keylen);

    virtual FX_BOOL IsMetadataEncrypted();

    virtual CPDF_CryptoHandler* CreateCryptoHandler();

private:
    std::shared_ptr<PDFSecurityHandler> m_pPDFSecHandler;
    bool m_bEncryptMetadata;
};

/**
 * @brief The implementaion class of interface class PDFCreator defined in pdf object model layer.
 * Please refer to comments of PDFCreator.
 */
class PDFCreatorImpl : public PDFCreator
{
public:
    explicit PDFCreatorImpl();
	virtual ~PDFCreatorImpl();

    virtual uint32_t CreateCustomEncryptedFile(const std::string& inputFilePath,
            const std::string& filter_name,
            const std::vector<unsigned char>& publishing_license,
            std::shared_ptr<PDFCryptoHandler> crypto_hander,
            rmscrypto::api::SharedStream outputIOS);

    virtual uint32_t UnprotectCustomEncryptedFile(rmscrypto::api::SharedStream inputIOS,
            const std::string& filter_name,
            std::shared_ptr<PDFSecurityHandler> security_hander,
            rmscrypto::api::SharedStream outputIOS);
protected:
    //if the document is protected by password, or is signed, or is dynamic XFA, it cannot be encrypted.
    bool IsProtectedByPassword(CPDF_Parser *pPDFParser);
    bool IsSigned(CPDF_Parser *pPDFParser);
    bool IsDynamicXFA(CPDF_Parser *pPDFParser);

    uint32_t ParsePDFFile(CPDF_Parser* pPDFParser);
    uint32_t CreatePDFFile(CPDF_Parser* pPDFParser,
                           CPDF_Dictionary *pEncryptionDic,
                           std::shared_ptr<PDFCryptoHandler> crypto_hander,
                           rmscrypto::api::SharedStream outputIOS);

    CPDF_Dictionary* CreateEncryptionDict(const std::string& filter_name, const std::vector<unsigned char>& publishing_license);
    uint32_t ConvertParsingErrCode(FX_DWORD parseResult);
private:
    std::string m_filePath;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_PDFCREATOR_H_
