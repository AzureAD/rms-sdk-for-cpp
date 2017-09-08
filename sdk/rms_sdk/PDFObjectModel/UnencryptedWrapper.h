#ifndef RMS_SDK_PDF_UNENCRYPTED_WRAPPER_H
#define RMS_SDK_PDF_UNENCRYPTED_WRAPPER_H

#include "PDFObjectModel.h"
#include "CoreInc.h"
#include "Basic.h"

namespace rmscore {
namespace pdfobjectmodel {

#define IRMV1_WRAPPER_DICTIONARY            "Wrapper"
#define IRMV1_WRAPPER_DICTIONARY_TYPE       "Type"
#define IRMV1_WRAPPER_DICTIONARY_VERSION    "Version"
#define IRMV1_WRAPPER_DICTIONARY_OFFSET     "WrapperOffset"

/**
 * @brief The implementaion class of interface class PDFWrapperDoc defined in pdf object model layer.
 * The implementation is on the basic of Foxit core.
 * Please refer to comments of PDFWrapperDoc.
 */
class PDFWrapperDocImpl : public PDFWrapperDoc
{
public:
    PDFWrapperDocImpl(rmscrypto::api::SharedStream wrapperDocStream);
    virtual ~PDFWrapperDocImpl();

	virtual uint32_t GetWrapperType() const;

	virtual bool GetCryptographicFilter(std::wstring& wsGraphicFilter, float &fVersion) const;

	virtual uint32_t GetPayLoadSize() const;

    //this is just for IRM V2.
    virtual bool GetPayloadFileName(std::wstring& wsFileName) const;

    virtual bool StartGetPayload(rmscrypto::api::SharedStream outputStream);

private:
    FileStreamImpl* m_wrapperFileStream;
    CPDF_Parser m_pdfParser;
    //this is just for IRM V2. we have to parse IRM V1 self.
    CPDF_WrapperDoc* m_wrapperDoc;

    uint32_t m_wrapperType;
    std::wstring m_wsGraphicFilter;
    float m_fVersion;
    uint32_t m_payloadSize;
    std::wstring m_wsFileName;
};

/**
 * @brief The implementaion class of interface class PDFUnencryptedWrapperCreator defined in pdf object model layer.
 * The implementation is on the basic of Foxit core.
 * Please refer to comments of PDFUnencryptedWrapperCreator.
 */
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
    IPDF_UnencryptedWrapperCreator* m_pPDFWrapper20Creator;
    FileStreamImpl* m_payloadFileStream;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMS_SDK_PDF_UNENCRYPTED_WRAPPER_H
