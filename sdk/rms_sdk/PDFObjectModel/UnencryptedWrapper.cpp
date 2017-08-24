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

    m_wrapperFileStream = new FileStreamImpl(wrapperDocStream);
    FX_DWORD parseResult = m_pdfParser.StartParse(m_wrapperFileStream);
    if(PDFPARSE_ERROR_SUCCESS == parseResult)
    {
        CPDF_Document* pDoc = m_pdfParser.GetDocument();
        m_wrapperDoc = new CPDF_WrapperDoc(pDoc);
    }
}

PDFWrapperDocImpl::~PDFWrapperDocImpl()
{
    m_pdfParser.CloseParser();

    if(m_wrapperFileStream)
    {
       delete m_wrapperFileStream;
       m_wrapperFileStream = nullptr;
    }

    if(m_wrapperDoc)
    {
        delete m_wrapperDoc;
        m_wrapperDoc = nullptr;
    }
}

uint32_t PDFWrapperDocImpl::GetWrapperType() const
{
    if(!m_wrapperDoc) return PDFWRAPPERDOC_TYPE_UNKNOWN;
    return m_wrapperDoc->GetWrapperType();
}

bool PDFWrapperDocImpl::GetCryptographicFilter(std::wstring& wsGraphicFilter, float &fVersion) const
{
    if(!m_wrapperDoc) return false;
    CFX_WideString wsGraphicFilterGet;
    FX_FLOAT fVersionGet = 0;
    bool bGet = m_wrapperDoc->GetCryptographicFilter(wsGraphicFilterGet, fVersionGet);
    if(bGet)
    {
        wsGraphicFilter = (wchar_t*)(FX_LPCWSTR)wsGraphicFilterGet;
        fVersion = fVersionGet;
        return true;
    }
    return false;
}

uint32_t PDFWrapperDocImpl::GetPayLoadSize() const
{
    if(!m_wrapperDoc) return 0;
    return m_wrapperDoc->GetPayLoadSize();
}

bool PDFWrapperDocImpl::GetPayloadFileName(std::wstring& wsFileName) const
{
    if(!m_wrapperDoc) return false;
    CFX_WideString wsFileNameGet;
    bool bGet = m_wrapperDoc->GetPayloadFileName(wsFileNameGet);
    if(bGet)
    {
        wsFileName = (wchar_t*)(FX_LPCWSTR)wsFileNameGet;
        return true;
    }
    return false;
}

bool PDFWrapperDocImpl::StartGetPayload(rmscrypto::api::SharedStream outputStream)
{
    if(!m_wrapperDoc) return false;
    FileStreamImpl outputFileStream(outputStream);
    m_wrapperDoc->StartGetPayload(&outputFileStream);
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

    m_wrapperFileStream = new FileStreamImpl(wrapperDocStream);
    FX_DWORD parseResult = m_pdfParser.StartParse(m_wrapperFileStream);
    if(PDFPARSE_ERROR_SUCCESS == parseResult)
    {
        CPDF_Document* pDoc = m_pdfParser.GetDocument();
        m_pPDFWrapper20Creator = FPDF_Wrapper20Creator_Create(pDoc);
        //m_pPDFWrapper20Creator = FPDF_UnencryptedWrapperCreator_Create(pDoc);
    }
}

PDFUnencryptedWrapperCreatorImpl::~PDFUnencryptedWrapperCreatorImpl()
{
     m_pdfParser.CloseParser();

    if(m_wrapperFileStream)
    {
        delete m_wrapperFileStream;
        m_wrapperFileStream = nullptr;
    }

    if(m_payloadFileStream)
    {
        delete m_payloadFileStream;
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
        delete m_payloadFileStream;
        m_payloadFileStream = nullptr;
    }
    m_payloadFileStream = new FileStreamImpl(inputStream);
    m_pPDFWrapper20Creator->SetPayLoad(m_payloadFileStream);
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
