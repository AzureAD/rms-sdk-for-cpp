#ifndef RMS_SDK_PDF_UNENCRYPTED_WRAPPER_H
#define RMS_SDK_PDF_UNENCRYPTED_WRAPPER_H

#include "PDFObjectModel.h"
#include "CoreInc.h"
#include "Basic.h"

namespace rmscore {
namespace pdfobjectmodel {

class PDFWrapperDocImpl : public PDFWrapperDoc
{
public:
    PDFWrapperDocImpl(rmscrypto::api::SharedStream wrapperDocStream);
    virtual ~PDFWrapperDocImpl();

	virtual uint32_t GetWrapperType() const;

	virtual bool GetCryptographicFilter(std::wstring& wsGraphicFilter, float &fVersion) const;

	virtual uint32_t GetPayLoadSize() const;

    virtual bool GetPayloadFileName(std::wstring& wsFileName) const;

    virtual bool StartGetPayload(rmscrypto::api::SharedStream outputStream);

private:
    FileStreamImpl* m_wrapperFileStream;
    CPDF_Parser m_pdfParser;
    CPDF_WrapperDoc* m_wrapperDoc;
};

class PDFUnencryptedWrapperCreatorImpl : public PDFUnencryptedWrapperCreator
{
public:
    PDFUnencryptedWrapperCreatorImpl(rmscrypto::api::SharedStream wrapperDocStream);
	virtual ~PDFUnencryptedWrapperCreatorImpl();

    virtual void SetPayloadInfo(
            const std::wstring& wsSubType,
            const std::wstring& wsFileName,
            const std::wstring& wsDescription,
            float fVersion);

    virtual void SetPayLoad(rmscrypto::api::SharedStream inputStream);

    virtual bool CreateUnencryptedWrapper(rmscrypto::api::SharedStream outputStream);

private:
    FileStreamImpl* m_wrapperFileStream;
    CPDF_Parser m_pdfParser;
    IPDF_Wrapper20Creator* m_pPDFWrapper20Creator;
    //IPDF_UnencryptedWrapperCreator* m_pPDFWrapper20Creator;
    FileStreamImpl* m_payloadFileStream;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMS_SDK_PDF_UNENCRYPTED_WRAPPER_H
