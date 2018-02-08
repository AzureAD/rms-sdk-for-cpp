#include "../../include/fpdfapi/fpdf_parser.h"
#include "../../include/fpdfapi/fpdf_module.h"
#include "../../include/fpdfdoc/fpdf_doc.h"
#include "time.h"
#include "parser_int.h"
CPDF_PDFFeature CPDF_PDFVersionChecker::VersionCheck(CPDF_Parser* pParser, CPDF_PDFFeature feature)
{
    FX_INT32 featureValue = (FX_INT32)feature;
    if (featureValue < 10) {
        return (CPDF_PDFFeature)pParser->m_FileVersion;
    }
    if (pParser->m_FileVersion < featureValue) {
        pParser->m_FileVersion = featureValue;
    }
    return (CPDF_PDFFeature)pParser->m_FileVersion;
}
CPDF_Document::CPDF_Document(IPDF_DocParser* pParser) : CPDF_IndirectObjects(pParser)
{
    ASSERT(pParser != NULL);
    m_pRootDict = NULL;
    m_pInfoDict = NULL;
    m_bLinearized = FALSE;
    m_bMetaDataFlate  = FALSE;
    m_FileVersion = pParser ? ((CPDF_Parser*)pParser)->GetFileVersion() : 0;
}
FX_BOOL	CPDF_Document::SetFileVersion(int fileVersion)
{
    if (fileVersion < 10 || fileVersion > 20 || fileVersion == 18 || fileVersion == 19) {
        return FALSE;
    }
    if (fileVersion < m_FileVersion) {
        return FALSE;
    }
    m_FileVersion = fileVersion;
    return TRUE;
}
void CPDF_Document::LoadDoc()
{
    m_LastObjNum = m_pParser->GetLastObjNum();
    CPDF_Object* pRootObj = GetIndirectObject(m_pParser->GetRootObjNum());
    if (pRootObj == NULL) {
        return;
    }
    m_pRootDict = pRootObj->GetDict();
    if (m_pRootDict == NULL) {
        return;
    }
    CPDF_Object* pInfoObj = GetIndirectObject(m_pParser->GetInfoObjNum());
    if (pInfoObj) {
        m_pInfoDict = pInfoObj->GetDict();
    }
    CPDF_Array* pIDArray = m_pParser->GetIDArray();
    if (pIDArray) {
        m_ID1 = pIDArray->GetString(0);
        m_ID2 = pIDArray->GetString(1);
    }
    m_PageList.SetSize(_GetPageCount());
}
CPDF_Document::~CPDF_Document()
{
}
void CPDF_Document::SetInfoObjNum(int nInfoObjNum)
{
    m_pInfoDict = (CPDF_Dictionary*)GetIndirectObject(nInfoObjNum);
}
int CPDF_Document::GetPageCount() const
{
    return m_PageList.GetSize();
}
static int _CountPages(CPDF_Dictionary* pPages, int level)
{
    if (level > 128) {
        return 0;
    }
    CPDF_Object* pCountObj = pPages->GetElement(FX_BSTRC("Count"));
    int count = 0;
    int num = 0;
    while (pCountObj && pCountObj->GetType() == PDFOBJ_REFERENCE && num < 32) {
        pCountObj = ((CPDF_Reference*)pCountObj)->GetDirect();
        num ++;
    }
    if (num < 32) {
        count = pPages->GetInteger(FX_BSTRC("Count"));
    }
    if (count > 0 && count < FPDF_PAGE_MAX_NUM) {
        return count;
    }
    CPDF_Array* pKidList = pPages->GetArray(FX_BSTRC("Kids"));
    if (pKidList == NULL) {
        return 0;
    }
    count = 0;
    for (FX_DWORD i = 0; i < pKidList->GetCount(); i ++) {
        CPDF_Dictionary* pKid = pKidList->GetDict(i);
        if (pKid == NULL || pKid == pPages) {
            continue;
        }
        if (!pKid->KeyExist(FX_BSTRC("Kids"))) {
            count ++;
        } else {
            count += _CountPages(pKid, level + 1);
        }
    }
    pPages->SetAtInteger(FX_BSTRC("Count"), count);
    return count;
}
int CPDF_Document::_GetPageCount() const
{
    CPDF_Dictionary* pRoot = GetRoot();
    if (pRoot == NULL) {
        return 0;
    }
    CPDF_Dictionary* pPages = pRoot->GetDict(FX_BSTRC("Pages"));
    if (pPages == NULL) {
        return 0;
    }
    if (!pPages->KeyExist(FX_BSTRC("Kids"))) {
        return 1;
    }
    return _CountPages(pPages, 0);
}
FX_DWORD CPDF_Document::GetUserPermissions(FX_BOOL bCheckRevision) const
{
    if (m_pParser == NULL) {
        return (FX_DWORD) - 1;
    }
    return m_pParser->GetPermissions(bCheckRevision);
}
FX_BOOL CPDF_Document::IsOwner() const
{
    if (m_pParser == NULL) {
        return TRUE;
    }
    return m_pParser->IsOwner();
}
static CPDF_Dictionary* GetWrapper20AFDict(CPDF_Dictionary* pRoot)
{
    if (!pRoot) {
        return NULL;
    }
    CPDF_Object* pAF = pRoot->GetElementValue(FX_BSTRC("AF"));
    if (!pAF) {
        return NULL;
    }
    CPDF_Dictionary* pAFDict = NULL;
    if (pAF->GetType() == PDFOBJ_ARRAY) {
        CPDF_Array* pArray = (CPDF_Array*)pAF;
        if (pArray && pArray->GetCount() > 0) {
            pAFDict = (CPDF_Dictionary*)pArray->GetElementValue(0);
        }
    }
    return pAFDict;
}
FX_INT32 CPDF_WrapperDoc::GetWrapperType() const
{
    if(!m_pDoc) {
        return -1;
    }
    IPDF_DocParser* pParser = m_pDoc->GetParser();
    if (pParser) {
        CPDF_Dictionary* pTrailer = ((CPDF_Parser*)pParser)->GetTrailer();
        if (pTrailer && pTrailer->KeyExist(FX_BSTRC("Wrapper"))) {
            return PDF_WRAPPERTYPE_FOXIT;
        }
    }
    CPDF_Dictionary* pRoot = m_pDoc->GetRoot();
    if (pRoot) {
        CPDF_Dictionary* pAFDict = GetWrapper20AFDict(pRoot);
        if (pAFDict && pAFDict->KeyExist(FX_BSTRC("AFRelationship"))) {
            CFX_ByteString bsStr = pAFDict->GetConstString(FX_BSTRC("AFRelationship"));
            if (bsStr == "EncryptedPayload") {
                return PDF_WRAPPERTYPE_PDF2;
            }
        }
    }
    return PDF_WRAPPERTYPE_NO;
}
FX_BOOL CPDF_WrapperDoc::GetCryptographicFilter(CFX_WideString &wsGraphicFilter, FX_FLOAT &fVersion) const
{
    if(!m_pDoc) {
        return FALSE;
    }
    CPDF_Dictionary* pRoot = m_pDoc->GetRoot();
    if (!pRoot) {
        return FALSE;
    }
    CPDF_Dictionary* pAFDict = GetWrapper20AFDict(pRoot);
    if (!pAFDict) {
        return FALSE;
    }
    CPDF_Dictionary* pEPDict = pAFDict->GetDict(FX_BSTRC("EP"));
    if (!pEPDict || !pEPDict->KeyExist(FX_BSTRC("Subtype"))) {
        return FALSE;
    }
    wsGraphicFilter = ((CFX_ByteString)pEPDict->GetConstString(FX_BSTRC("Subtype"))).UTF8Decode();
    if (pEPDict->KeyExist(FX_BSTRC("Version"))) {
        fVersion = pEPDict->GetFloat(FX_BSTRC("Version"));
    } else {
        fVersion = 0.f;
    }
    return TRUE;
}
FX_FILESIZE CPDF_WrapperDoc::GetPayLoadSize() const
{
    if(!m_pDoc) {
        return -1;
    }
    CPDF_Dictionary* pRoot = m_pDoc->GetRoot();
    if (!pRoot) {
        return -1;
    }
    CPDF_Dictionary* pAFDict = GetWrapper20AFDict(pRoot);
    if (!pAFDict) {
        return -1;
    }
    CPDF_Dictionary* pEFDict = pAFDict->GetDict(FX_BSTRC("EF"));
    if (!pEFDict) {
        return -1;
    }
    CPDF_Stream* pStream = pEFDict->GetStream(FX_BSTRC("F"));
    if (!pStream || !pStream->GetDict()) {
        return -1;
    }
    CPDF_Dictionary* pParams = pStream->GetDict()->GetDict(FX_BSTRC("Params"));
    if (!pParams || !pParams->KeyExist(FX_BSTRC("Size"))) {
        return -1;
    }
    return static_cast<FX_FILESIZE>(pParams->GetInteger64(FX_BSTRC("Size")));
}
FX_BOOL GetFileNameFromAF(CPDF_Dictionary* pRoot, CFX_WideString &wsFileName)
{
    CPDF_Dictionary* pAFDict = GetWrapper20AFDict(pRoot);
    if (!pAFDict) {
        return FALSE;
    }
    CPDF_FileSpec fileSpec(pAFDict);
    return  fileSpec.GetFileName(wsFileName);
}
FX_BOOL GetFileNameFromNames(CPDF_Dictionary* pRoot, CFX_WideString &wsFileName)
{
    CPDF_Dictionary* pNames = pRoot->GetDict(FX_BSTRC("Names"));
    if (!pNames) {
        return FALSE;
    }
    CPDF_Dictionary* pEmbeddedFiles = pNames->GetDict(FX_BSTRC("EmbeddedFiles"));
    if (!pEmbeddedFiles) {
        return FALSE;
    }
    CPDF_Array* pNamesArray = pEmbeddedFiles->GetArray(FX_BSTRC("Names"));
    if (!pNamesArray || pNamesArray->GetCount() < 2) {
        return FALSE;
    }
    wsFileName = ((CFX_ByteString)pNamesArray->GetConstString(0)).UTF8Decode();
    return TRUE;
}
FX_BOOL	CPDF_WrapperDoc::GetPayloadFileName(CFX_WideString &wsFileName) const
{
    if(!m_pDoc) {
        return FALSE;
    }
    CPDF_Dictionary* pRoot = m_pDoc->GetRoot();
    if (!pRoot) {
        return FALSE;
    }
    if (GetFileNameFromAF(pRoot, wsFileName)) {
        return TRUE;
    } else {
        return GetFileNameFromNames(pRoot, wsFileName);
    }
}
FX_BOOL CPDF_WrapperDoc::StartGetPayload(IFX_FileWrite* pPayload, IFX_Pause *pPause )
{
    if (!m_pDoc || !pPayload) {
        return FALSE;
    }
    CPDF_Dictionary* pRoot = m_pDoc->GetRoot();
    if (!pRoot) {
        return FALSE;
    }
    CPDF_Dictionary* pAFDict = GetWrapper20AFDict(pRoot);
    if (!pAFDict) {
        return FALSE;
    }
    CPDF_FileSpec fileSpec(pAFDict);
    CPDF_Stream* pStream = fileSpec.GetFileStream();
    if (!pStream) {
        return FALSE;
    }
    m_pSteamFilter = pStream->GetStreamFilter(FALSE);
    if (!m_pSteamFilter) {
        return FALSE;
    }
    m_pPayload = pPayload;
    m_pBuffer = FX_Alloc(FX_BYTE, STREAM_BUFSIZE);
    if (!m_pBuffer) {
        return FALSE;
    }
    return Continue(pPause) > 0;
}
FX_INT32 CPDF_WrapperDoc::Continue(IFX_Pause *pPause )
{
    if (!m_pSteamFilter || !m_pPayload || !m_pBuffer) {
        return -1;
    }
    FX_INT32 ret = -1;
    FX_BOOL bCondition = TRUE;
    while (bCondition) {
        FXSYS_memset32(m_pBuffer, 0, STREAM_BUFSIZE);
        size_t len = m_pSteamFilter->ReadBlock(m_pBuffer, STREAM_BUFSIZE);
        if (len == 0) {
            ret = 0;
            break;
        }
        FX_BOOL l = m_pPayload->WriteBlock(m_pBuffer, len);
        if (!l) {
            return -1;
        }
        if (len < STREAM_BUFSIZE) {
            ret = 0;
            break;
        }
        if (pPause && pPause->NeedToPauseNow()) {
            ret = 1;
            break;
        }
    }
    return ret;
}
