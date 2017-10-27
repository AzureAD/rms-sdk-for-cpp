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
    explicit CustomCryptoHandler(std::shared_ptr<PDFCryptoHandler> pdf_crypto_handler);
	virtual ~CustomCryptoHandler();

public:
    virtual FX_BOOL		Init(CPDF_Dictionary* encryption_dictionary,
                             CPDF_SecurityHandler* security_handler);

	virtual FX_DWORD	DecryptGetSize(FX_DWORD src_size);

	virtual FX_LPVOID	DecryptStart(FX_DWORD objnum, FX_DWORD gennum);

    virtual FX_BOOL		DecryptStream(FX_LPVOID context,
                                      FX_LPCBYTE src_buf,
                                      FX_DWORD src_size,
                                      CFX_BinaryBuf& dest_buf);

	virtual FX_BOOL		DecryptFinish(FX_LPVOID context, CFX_BinaryBuf& dest_buf);

    virtual FX_DWORD	EncryptGetSize(FX_DWORD objnum,
                                       FX_DWORD version,
                                       FX_LPCBYTE src_buf,
                                       FX_DWORD src_size);

    virtual FX_BOOL		EncryptContent(FX_DWORD objnum,
                                       FX_DWORD version,
                                       FX_LPCBYTE src_buf,
                                       FX_DWORD src_size,
		FX_LPBYTE dest_buf, FX_DWORD& dest_size);

    virtual FX_BOOL		ProgressiveEncryptStart(FX_DWORD objnum,
                                                FX_DWORD version,
                                                FX_DWORD raw_size,
                                                FX_BOOL flate_encode);

    virtual FX_BOOL     ProgressiveEncryptContent(FX_INT32 objnum,
                                                  FX_DWORD version,
                                                  FX_LPCBYTE src_buf,
                                                  FX_DWORD src_size,
                                                  CFX_BinaryBuf& dest_buf);

	virtual FX_BOOL     ProgressiveEncryptFinish(CFX_BinaryBuf& dest_buf);

protected:
    std::shared_ptr<PDFCryptoHandler> pdf_crypto_handler_;
};

/**
 * @brief The implementaion class of interface class CPDF_ProgressiveEncryptHandler of Foxit core.
 * CPDF_ProgressiveEncryptHandler is set to deal with progressive encryption.
 * Please refer to comments of CPDF_ProgressiveEncryptHandler.
 */
class CustomProgressiveEncryptHandler : public CPDF_ProgressiveEncryptHandler
{
public:
    explicit CustomProgressiveEncryptHandler(CFX_WideString temp_path);

    virtual ~CustomProgressiveEncryptHandler();

    virtual IFX_FileStream* GetTempFile();

    virtual void ReleaseTempFile(IFX_FileStream* file_stream);

    virtual FX_BOOL SetCryptoHandler(CPDF_CryptoHandler* crypto_handler);

    virtual FX_DWORD EncryptGetSize(FX_DWORD objnum,
                                    FX_DWORD gennum,
                                    FX_LPCBYTE src_buf,
                                    FX_DWORD src_size);

    virtual FX_LPVOID EncryptStart(FX_DWORD objnum,
                                   FX_DWORD gennum,
                                   FX_DWORD raw_size,
                                   FX_BOOL flate_encode);

    virtual FX_BOOL EncryptStream(FX_LPVOID context,
                                  FX_LPCBYTE src_buf,
                                  FX_DWORD src_size,
                                  IFX_FileStream* dest_file_stream);

    virtual FX_BOOL EncryptFinish(FX_LPVOID context,
                                  IFX_FileStream* dest_file_stream);

    virtual FX_BOOL UpdateFilter(CPDF_Dictionary* pdf_dictionary);

 private:
    CFX_WideString temp_path_;
    CustomCryptoHandler* crypto_handler_;
    FX_DWORD obj_number_;
    FX_DWORD version_number_;
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
    explicit CustomSecurityHandler(std::shared_ptr<PDFSecurityHandler> pdf_security_handler);

    virtual ~CustomSecurityHandler();

    virtual FX_BOOL OnInit(CPDF_Parser* pdf_parser, CPDF_Dictionary* encryption_dictionary);

    virtual FX_DWORD GetPermissions();

    virtual FX_BOOL IsOwner();

    virtual FX_BOOL GetCryptInfo(int& cipher, FX_LPCBYTE& buffer, int& keylen);

    virtual FX_BOOL IsMetadataEncrypted();

    virtual CPDF_CryptoHandler* CreateCryptoHandler();

private:
    std::shared_ptr<PDFSecurityHandler> pdf_security_handler_;
    bool encrypt_metadata_;
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

    virtual uint32_t CreateCustomEncryptedFile(const std::string& input_file_path,
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
    bool IsProtectedByPassword(CPDF_Parser *pdf_parser);
    bool IsSigned(CPDF_Parser *pdf_parser);
    bool IsDynamicXFA(CPDF_Parser *pdf_parser);

    uint32_t ParsePDFFile(CPDF_Parser* pdf_parser);
    uint32_t CreatePDFFile(CPDF_Parser* pdf_parser,
                           CPDF_Dictionary *encryption_dictionary,
                           std::shared_ptr<PDFCryptoHandler> crypto_hander,
                           rmscrypto::api::SharedStream outputIOS);

    CPDF_Dictionary* CreateEncryptionDict(const std::string& filter_name,
                                          const std::vector<unsigned char>& publishing_license);
    uint32_t ConvertParsingErrCode(FX_DWORD parse_result);
private:
    std::string file_path_;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_PDFCREATOR_H_
