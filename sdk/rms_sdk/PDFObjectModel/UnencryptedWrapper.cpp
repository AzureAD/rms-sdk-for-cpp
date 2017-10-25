#include "UnencryptedWrapper.h"
#include "Basic.h"

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
// class PDFWrapperDoc

std::unique_ptr<PDFWrapperDoc> PDFWrapperDoc::Create(rmscrypto::api::SharedStream inputStream)
{
    std::unique_ptr<PDFWrapperDoc> pdfWrapperDoc(new PDFWrapperDocImpl(inputStream));
    return pdfWrapperDoc;
}

PDFWrapperDocImpl::PDFWrapperDocImpl(rmscrypto::api::SharedStream wrapperDocStream)
{
    m_wrapperDoc = nullptr;
    m_wrapperType = PDFWRAPPERDOC_TYPE_NORMAL;
    m_wsGraphicFilter = L"";
    m_fVersion = 0;
    m_payloadSize = 0;
    m_wsFileName = L"";

    m_wrapperFileStream = std::make_shared<FileStreamImpl>(wrapperDocStream);
    FX_DWORD parseResult = m_pdfParser.StartParse(m_wrapperFileStream.get());
    if(PDFPARSE_ERROR_SUCCESS == parseResult)
    {
        CPDF_Document* pDoc = m_pdfParser.GetDocument();
        m_wrapperDoc = std::make_shared<CPDF_WrapperDoc>(pDoc);

        m_wrapperType = m_wrapperDoc->GetWrapperType();
        if(m_wrapperType == PDFWRAPPERDOC_TYPE_IRMV1)
        {
            CPDF_Dictionary* trailerDict = m_pdfParser.GetTrailer();
            CPDF_Dictionary* wrapperDict = trailerDict->GetDict(IRMV1_WRAPPER_DICTIONARY);
            if(wrapperDict)
            {
                CFX_ByteString bsType = wrapperDict->GetString(IRMV1_WRAPPER_DICTIONARY_TYPE);
                CFX_WideString wsType = bsType.UTF8Decode();
                m_wsGraphicFilter = (wchar_t*)(FX_LPCWSTR)wsType;
                m_fVersion = 1;
                m_payloadSize = trailerDict->GetInteger(IRMV1_WRAPPER_DICTIONARY_OFFSET);
            }
        }
        else if(m_wrapperType == PDFWRAPPERDOC_TYPE_IRMV2)
        {
            CFX_WideString wsGraphicFilterGet;
            m_wrapperDoc->GetCryptographicFilter(wsGraphicFilterGet, m_fVersion);
            m_wsGraphicFilter = (wchar_t*)(FX_LPCWSTR)wsGraphicFilterGet;

            m_payloadSize = m_wrapperDoc->GetPayLoadSize();

            CFX_WideString wsFileName;
            m_wrapperDoc->GetPayloadFileName(wsFileName);
            m_wsFileName = (wchar_t*)(FX_LPCWSTR)wsFileName;
        }
    }
}

PDFWrapperDocImpl::~PDFWrapperDocImpl()
{
    m_pdfParser.CloseParser();

    m_wrapperFileStream.reset();
    m_wrapperFileStream = nullptr;

    m_wrapperDoc.reset();
    m_wrapperDoc = nullptr;
}

uint32_t PDFWrapperDocImpl::GetWrapperType() const
{
    return m_wrapperType;
}

bool PDFWrapperDocImpl::GetCryptographicFilter(std::wstring& wsGraphicFilter, float &fVersion) const
{
    wsGraphicFilter = m_wsGraphicFilter;
    fVersion = m_fVersion;
    return true;
}

uint32_t PDFWrapperDocImpl::GetPayLoadSize() const
{
    return m_payloadSize;
}

bool PDFWrapperDocImpl::GetPayloadFileName(std::wstring& wsFileName) const
{
    wsFileName = m_wsFileName;
    return true;
}

bool PDFWrapperDocImpl::StartGetPayload(rmscrypto::api::SharedStream outputStream)
{
    if(m_wrapperType == PDFWRAPPERDOC_TYPE_IRMV1)
    {
        uint8_t* pBuf = new uint8_t[m_payloadSize];
        m_wrapperFileStream->ReadBlock(pBuf, 0, m_payloadSize);
        outputStream->Write(pBuf, m_payloadSize);
    }
    else if(m_wrapperType == PDFWRAPPERDOC_TYPE_IRMV2)
    {
        if(!m_wrapperDoc) return false;
        FileStreamImpl outputFileStream(outputStream);
        m_wrapperDoc->StartGetPayload(&outputFileStream);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// class PDFUnencryptedWrapperCreatorImpl
std::unique_ptr<PDFUnencryptedWrapperCreator> PDFUnencryptedWrapperCreator::Create(rmscrypto::api::SharedStream wrapperDocStream)
{
    std::unique_ptr<PDFUnencryptedWrapperCreator> pdfUnencryptedWrapperCreator(new PDFUnencryptedWrapperCreatorImpl(wrapperDocStream));
    return pdfUnencryptedWrapperCreator;
}

PDFUnencryptedWrapperCreatorImpl::PDFUnencryptedWrapperCreatorImpl(rmscrypto::api::SharedStream wrapperDocStream)
{
    m_pPDFWrapper20Creator = nullptr;
    m_payloadFileStream = nullptr;

    m_wrapperFileStream = std::make_shared<FileStreamImpl>(wrapperDocStream);
    FX_DWORD parseResult = m_pdfParser.StartParse(m_wrapperFileStream.get());
    if(PDFPARSE_ERROR_SUCCESS == parseResult)
    {
        CPDF_Document* pDoc = m_pdfParser.GetDocument();
        m_pPDFWrapper20Creator = FPDF_UnencryptedWrapperCreator_Create(pDoc);
    }
}

PDFUnencryptedWrapperCreatorImpl::~PDFUnencryptedWrapperCreatorImpl()
{
     m_pdfParser.CloseParser();

    m_wrapperFileStream.reset();
    m_wrapperFileStream = nullptr;

    if(m_payloadFileStream)
    {
        m_payloadFileStream.reset();
        m_payloadFileStream = nullptr;
    }

    if(m_pPDFWrapper20Creator)
    {
        m_pPDFWrapper20Creator->Release();
        m_pPDFWrapper20Creator = nullptr;
    }
}

void PDFUnencryptedWrapperCreatorImpl::SetPayloadInfo(
        const std::wstring& wsSubType,
        const std::wstring& wsFileName,
        const std::wstring& wsDescription,
        float fVersion)
{
    if(!m_pPDFWrapper20Creator) return;
    CFX_WideString subType = wsSubType.c_str();
    CFX_WideString fileName = wsFileName.c_str();
    CFX_WideString description = wsDescription.c_str();
    m_pPDFWrapper20Creator->SetPayloadInfo(subType, fileName, description, fVersion);
}

void PDFUnencryptedWrapperCreatorImpl::SetPayLoad(rmscrypto::api::SharedStream inputStream)
{
    if(!m_pPDFWrapper20Creator) return;

    if(m_payloadFileStream)
    {
        m_payloadFileStream.reset();
        m_payloadFileStream = nullptr;
    }
    m_payloadFileStream = std::make_shared<FileStreamImpl>(inputStream);
    m_pPDFWrapper20Creator->SetPayLoad(m_payloadFileStream.get());
}

bool PDFUnencryptedWrapperCreatorImpl::CreateUnencryptedWrapper(rmscrypto::api::SharedStream outputStream)
{
    if(!m_pPDFWrapper20Creator) return false;
    FileStreamImpl outputFileStream(outputStream);
    m_pPDFWrapper20Creator->Create(&outputFileStream);
    return true;
}

} // namespace pdfobjectmodel
} // namespace rmscore
