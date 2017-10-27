/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include "../../../include/fpdfapi/fpdf_parser.h"
#include "../../../include/fpdfapi/fpdf_module.h"
#include "parser_int.h"
#include <limits.h>
#define _PARSER_OBJECT_LEVLE_		64
extern const FX_LPCSTR _PDF_CharType;
FX_BOOL IsSignatureDict(CPDF_Dictionary* pDict, FX_DWORD objnum,  CPDF_Object* pCurContainer)
{
    if (!pDict) {
        return FALSE;
    }
    CPDF_Object* pType = pDict->GetElementValue(FX_BSTRC("Type"));
    if (!pType) {
        pType = pDict->GetElementValue(FX_BSTRC("FT"));
        if (!pType) {
            if(objnum && pCurContainer && pCurContainer->GetType() == PDFOBJ_DICTIONARY) {
                CPDF_Dictionary* pDictContainer = (CPDF_Dictionary*)pCurContainer;
                if(pDictContainer->GetString(FX_BSTRC("FT")) == FX_BSTRC("Sig")) {
                    CPDF_Object* pObj = pDictContainer->GetElement(FX_BSTRC("V"));
                    if(pObj && pObj->GetType() == PDFOBJ_REFERENCE && ((CPDF_Reference*)pObj)->GetRefObjNum() == objnum) {
                        if(pDict->GetString(FX_BSTRC("SubFilter")) == FX_BSTRC("ETSI.RFC3161")) {
                            pDict->SetAtName(FX_BSTRC("Type"), FX_BSTRC("DocTimeStamp"));
                        } else {
                            pDict->SetAtName(FX_BSTRC("Type"), FX_BSTRC("Sig"));
                        }
                        return TRUE;
                    }
                }
            }
            return FALSE;
        }
    }
    if (pType->GetString() == FX_BSTRC("Sig") || pType->GetString() == FX_BSTRC("DocTimeStamp")) {
        return TRUE;
    }
    return FALSE;
}
static FX_INT32 _CompareDWord(const void* p1, const void* p2)
{
    return (*(FX_DWORD*)p1) - (*(FX_DWORD*)p2);
}
static int _CompareFileSize(const void* p1, const void* p2)
{
    FX_FILESIZE ret = (*(FX_FILESIZE*)p1) - (*(FX_FILESIZE*)p2);
    if (ret > 0) {
        return 1;
    }
    if (ret < 0) {
        return -1;
    }
    return 0;
}
CPDF_Parser::CPDF_Parser()
{
    m_pDocument = NULL;
    m_pTrailer = NULL;
    m_pEncryptDict = NULL;
    m_pSecurityHandler = NULL;
    m_LastXRefOffset = 0;
    m_pLinearized = NULL;
    m_dwFirstPageNo = 0;
    m_FileVersion = 0;
    m_dwXrefStartObjNum = 0;
    m_bOwnFileRead = TRUE;
    m_bForceUseSecurityHandler = FALSE;
    m_bRebuildXRef = FALSE;
    m_bLinearized = FALSE;
    m_LinearizedFirstPageXRefOffset = -1;
    m_bInitObjectStreamMap = FALSE;
    m_nTimeCount = 0;
    m_bXRefStream = FALSE;
    m_bVersionUpdated = FALSE;
    m_nLevel = 0;
}
CPDF_Parser::~CPDF_Parser()
{
    CloseParser(FALSE);
}
FX_DWORD CPDF_Parser::GetLastObjNum()
{
    FX_DWORD dwSize = m_CrossRef.GetSize();
    return dwSize ? dwSize - 1 : 0;
}
void CPDF_Parser::SetEncryptDictionary(CPDF_Dictionary* pDict)
{
    m_pEncryptDict = pDict;
}
void CPDF_Parser::CloseParser(FX_BOOL bReParse)
{
    m_bVersionUpdated = FALSE;
    m_bLinearized = FALSE;
    if (m_pDocument && !bReParse) {
        delete m_pDocument;
        m_pDocument = NULL;
    }
    if (m_pTrailer) {
        m_pTrailer->Release();
        m_pTrailer = NULL;
    }
    ReleaseEncryptHandler();
    SetEncryptDictionary(NULL);
    if (m_bOwnFileRead && m_Syntax.m_pFileAccess != NULL) {
        m_Syntax.m_pFileAccess->Release();
        m_Syntax.m_pFileAccess = NULL;
    }
    if(bReParse) {
        for(int i = 0; i < m_RefStreamObjNums.GetSize(); i++) {
            m_pDocument->ReleaseIndirectObject(m_RefStreamObjNums[i]);
        }
    }
    m_RefStreamObjNums.RemoveAll();
    FX_POSITION pos = m_ObjectStreamMap.GetStartPosition();
    while (pos) {
        FX_LPVOID objnum;
        CPDF_StreamAcc* pStream;
        m_ObjectStreamMap.GetNextAssoc(pos, objnum, (void*&)pStream);
        delete pStream;
        if(bReParse) {
            m_pDocument->ReleaseIndirectObject((FX_DWORD)(FX_INTPTR)objnum);
        }
    }
    m_bInitObjectStreamMap = FALSE;
    m_ObjectStreamMap.RemoveAll();
    m_ObjStmTimeCountMap.RemoveAll();
    m_SortedOffset.RemoveAll();
    m_CrossRef.RemoveAll();
    m_V5Type.RemoveAll();
    m_ObjVersion.RemoveAll();
    FX_INT32 iLen = m_Trailers.GetSize();
    for (FX_INT32 i = 0; i < iLen; ++i) {
        CPDF_Dictionary* pTrailer = m_Trailers.GetAt(i);
        if (pTrailer) {
            pTrailer->Release();
        }
    }
    m_Trailers.RemoveAll();
    if (m_pLinearized) {
        m_pLinearized->Release();
        m_pLinearized = NULL;
    }
    m_nLevel = 0;
}
static FX_INT32 GetHeaderOffset(IFX_FileRead* pFile)
{
    if (!pFile) {
        return -1;
    }
    FX_DWORD tag = FXDWORD_FROM_LSBFIRST(0x46445025);
    FX_BYTE buf[4];
    FX_INT32 offset = 0;
    while (1) {
        if (!pFile->ReadBlock(buf, offset, 4)) {
            return -1;
        }
        if (*(FX_DWORD*)buf == tag) {
            return offset;
        }
        offset ++;
        if (offset > 1024) {
            return -1;
        }
    }
    return -1;
}
FX_DWORD CPDF_Parser::StartParse(FX_LPCSTR filename, FX_BOOL bReParse)
{
    IFX_FileRead* pFileAccess = FX_CreateFileRead(filename);
    if (!pFileAccess) {
        return PDFPARSE_ERROR_FILE;
    }
    return StartParse(pFileAccess, bReParse);
}
FX_DWORD CPDF_Parser::StartParse(FX_LPCWSTR filename, FX_BOOL bReParse)
{
    IFX_FileRead* pFileAccess = FX_CreateFileRead(filename);
    if (!pFileAccess) {
        return PDFPARSE_ERROR_FILE;
    }
    return StartParse(pFileAccess, bReParse);
}
CPDF_SecurityHandler* FPDF_CreateStandardSecurityHandler();
CPDF_SecurityHandler* FPDF_CreatePubKeyHandler(void*);
FX_DWORD CPDF_Parser::StartParse(IFX_FileRead* pFileAccess, FX_BOOL bReParse, FX_BOOL bOwnFileRead)
{
    CloseParser(bReParse);
    m_bXRefStream = FALSE;
    m_LastXRefOffset = 0;
    m_bOwnFileRead = bOwnFileRead;
    FX_INT32 offset = GetHeaderOffset(pFileAccess);
    if (offset == -1) {
        if (bOwnFileRead && pFileAccess) {
            pFileAccess->Release();
        }
        return PDFPARSE_ERROR_FORMAT;
    }
    m_Syntax.InitParser(pFileAccess, offset);
    FX_BYTE ch;
    if (!m_Syntax.GetCharAt(5, ch)) {
        return PDFPARSE_ERROR_FORMAT;
    }
    if (ch >= '0' && ch <= '9') {
        m_FileVersion = (ch - '0') * 10;
    }
    if (!m_Syntax.GetCharAt(7, ch)) {
        return PDFPARSE_ERROR_FORMAT;
    }
    if (ch >= '0' && ch <= '9') {
        m_FileVersion += ch - '0';
    }
    if (m_Syntax.m_FileLen <  m_Syntax.m_HeaderOffset + 9) {
        return PDFPARSE_ERROR_FORMAT;
    }
    IsLinearizedFile(pFileAccess, offset);
    m_Syntax.RestorePos(m_Syntax.m_FileLen - m_Syntax.m_HeaderOffset - 1);
    FX_FILESIZE savePos = m_Syntax.SavePos();
    if (!m_Syntax.SearchWord(FX_BSTRC("%%EOF"), TRUE, FALSE, 1024)) {
        m_Syntax.RestorePos(savePos);
    }
    if (!bReParse) {
        m_pDocument = FX_NEW CPDF_Document(this);
    }
    FX_BOOL bXRefRebuilt = FALSE;
    if (m_Syntax.SearchWord(FX_BSTRC("startxref"), TRUE, FALSE, 4096)) {
        FX_FILESIZE startxref_offset = m_Syntax.SavePos();
        FX_LPVOID pResult = FXSYS_bsearch(&startxref_offset, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
        if (pResult == NULL) {
            m_SortedOffset.Add(startxref_offset);
        }
        m_Syntax.GetKeyword();
        FX_BOOL bNumber;
        CFX_ByteString xrefpos_str = m_Syntax.GetNextWord(bNumber);
        m_LastXRefOffset = (FX_FILESIZE)FXSYS_atoi64(xrefpos_str);
        if (!LoadAllCrossRefV4(m_LastXRefOffset) && !LoadAllCrossRefV5(m_LastXRefOffset)) {
            if (!RebuildCrossRef()) {
                return PDFPARSE_ERROR_FORMAT;
            }
            bXRefRebuilt = TRUE;
            m_LastXRefOffset = 0;
        }
    } else {
        if (!RebuildCrossRef()) {
            return PDFPARSE_ERROR_FORMAT;
        }
        bXRefRebuilt = TRUE;
    }
    FX_DWORD dwRet = SetEncryptHandler();
    if (dwRet != PDFPARSE_ERROR_SUCCESS) {
        return dwRet;
    }
    m_pDocument->LoadDoc();
    if (m_pDocument->GetRoot() == NULL || m_pDocument->GetPageCount() == 0) {
        if (bXRefRebuilt) {
            return PDFPARSE_ERROR_FORMAT;
        }
        ReleaseEncryptHandler();
        if (!RebuildCrossRef()) {
            return PDFPARSE_ERROR_FORMAT;
        }
        dwRet = SetEncryptHandler();
        if (dwRet != PDFPARSE_ERROR_SUCCESS) {
            return dwRet;
        }
        m_pDocument->LoadDoc();
        if (m_pDocument->GetRoot() == NULL || m_pDocument->GetPageCount() == 0) {
            return PDFPARSE_ERROR_FORMAT;
        }
    }
    FXSYS_qsort(m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
    FX_DWORD RootObjNum = GetRootObjNum();
    if (RootObjNum == 0) {
        ReleaseEncryptHandler();
        RebuildCrossRef();
        RootObjNum = GetRootObjNum();
        if (RootObjNum == 0) {
            return PDFPARSE_ERROR_FORMAT;
        }
        dwRet = SetEncryptHandler();
        if (dwRet != PDFPARSE_ERROR_SUCCESS) {
            return dwRet;
        }
    }
    if (m_pSecurityHandler && !m_pSecurityHandler->IsMetadataEncrypted()) {
        CPDF_Reference* pMetadata = (CPDF_Reference*)m_pDocument->GetRoot()->GetElement(FX_BSTRC("Metadata"));
        if (pMetadata && pMetadata->GetType() == PDFOBJ_REFERENCE) {
            m_Syntax.m_MetadataObjnum = pMetadata->GetRefObjNum();
        }
    }
    if(!m_pSecurityHandler || !m_pSecurityHandler->IsMetadataEncrypted()) {
        CPDF_Stream* pXMLStream = m_pDocument->GetRoot()->GetStream("Metadata");
        if(!pXMLStream || pXMLStream->GetDict() == NULL || !pXMLStream->GetDict()->KeyExist("Filter")) {
            return PDFPARSE_ERROR_SUCCESS;
        }
        CPDF_Object* pDecoder = pXMLStream->GetDict()->GetElementValue(FX_BSTRC("Filter"));
        if (!pDecoder) {
            return PDFPARSE_ERROR_SUCCESS;
        }
        if (pDecoder->GetType() == PDFOBJ_ARRAY) {
            CPDF_Array* pDecoders = (CPDF_Array*)pDecoder;
            for (FX_DWORD i = 0; i < pDecoders->GetCount(); i ++) {
                CFX_ByteStringC str = pDecoders->GetConstString(i);
                if(str == FX_BSTRC("FlateDecode") || str == FX_BSTRC("Fl") ||
                        str == FX_BSTRC("LZWDecode") || str == FX_BSTRC("LZW") ||
                        str == FX_BSTRC("RunLengthDecode") || str == FX_BSTRC("RL")) {
                    m_pDocument->m_bMetaDataFlate = TRUE;
                    break;
                }
            }
        } else if(pDecoder->GetType() == PDFOBJ_NAME) {
            CFX_ByteStringC str = pDecoder->GetConstString();
            if(str == FX_BSTRC("FlateDecode") || str == FX_BSTRC("Fl") ||
                    str == FX_BSTRC("LZWDecode") || str == FX_BSTRC("LZW") ||
                    str == FX_BSTRC("RunLengthDecode") || str == FX_BSTRC("RL")) {
                m_pDocument->m_bMetaDataFlate = TRUE;
            }
        }
    }
    return PDFPARSE_ERROR_SUCCESS;
}
FX_DWORD CPDF_Parser::SetEncryptHandler()
{
    ReleaseEncryptHandler();
    SetEncryptDictionary(NULL);
    if (m_pTrailer == NULL) {
        return PDFPARSE_ERROR_FORMAT;
    }
    CPDF_Object* pEncryptObj = m_pTrailer->GetElement(FX_BSTRC("Encrypt"));
    if (pEncryptObj) {
        if (pEncryptObj->GetType() == PDFOBJ_DICTIONARY) {
            SetEncryptDictionary((CPDF_Dictionary*)pEncryptObj);
        } else if (pEncryptObj->GetType() == PDFOBJ_REFERENCE) {
            pEncryptObj = m_pDocument->GetIndirectObject(((CPDF_Reference*)pEncryptObj)->GetRefObjNum());
            if (pEncryptObj) {
                SetEncryptDictionary(pEncryptObj->GetDict());
            }
        }
    }
    if (m_bForceUseSecurityHandler) {
        FX_DWORD err = PDFPARSE_ERROR_HANDLER;
        if (m_pSecurityHandler == NULL) {
            return PDFPARSE_ERROR_HANDLER;
        }
        if (!m_pSecurityHandler->OnInit(this, m_pEncryptDict)) {
            return err;
        }
        CPDF_CryptoHandler* pCryptoHandler = m_pSecurityHandler->CreateCryptoHandler();
        if (!pCryptoHandler->Init(m_pEncryptDict, m_pSecurityHandler)) {
            delete pCryptoHandler;
            pCryptoHandler = NULL;
            return PDFPARSE_ERROR_HANDLER;
        }
        m_Syntax.SetEncrypt(pCryptoHandler);
    } else if (m_pEncryptDict) {
        CFX_ByteString filter = m_pEncryptDict->GetString(FX_BSTRC("Filter"));
        CPDF_SecurityHandler* pSecurityHandler = NULL;
        FX_DWORD err = PDFPARSE_ERROR_HANDLER;
        if (filter == FX_BSTRC("Standard")) {
            pSecurityHandler = FPDF_CreateStandardSecurityHandler();
            err = PDFPARSE_ERROR_PASSWORD;
        } else if (filter == FX_BSTRC("Adobe.PubSec")) {
            return PDFPARSE_ERROR_CERT;
        } else {
            CPDF_ModuleMgr* pModuleMgr = CPDF_ModuleMgr::Get();
            pSecurityHandler = pModuleMgr->CreateSecurityHandler(filter);
            if (pSecurityHandler == NULL && pModuleMgr->GetSecurityHandlerCallback()) {
                (*pModuleMgr->GetSecurityHandlerCallback())(filter);
                pSecurityHandler = pModuleMgr->CreateSecurityHandler(filter);
            }
        }
        if (pSecurityHandler == NULL) {
            return PDFPARSE_ERROR_HANDLER;
        }
        if (!pSecurityHandler->OnInit(this, m_pEncryptDict)) {
            delete pSecurityHandler;
            pSecurityHandler = NULL;
            return err;
        }
        m_pSecurityHandler = pSecurityHandler;
        CPDF_CryptoHandler* pCryptoHandler = pSecurityHandler->CreateCryptoHandler();
        if (!pCryptoHandler->Init(m_pEncryptDict, m_pSecurityHandler)) {
            delete pCryptoHandler;
            pCryptoHandler = NULL;
            return PDFPARSE_ERROR_HANDLER;
        }
        m_Syntax.SetEncrypt(pCryptoHandler);
    }
    return PDFPARSE_ERROR_SUCCESS;
}
void CPDF_Parser::ReleaseEncryptHandler()
{
    if (m_Syntax.crypto_handler_) {
        delete m_Syntax.crypto_handler_;
        m_Syntax.crypto_handler_ = NULL;
    }
    if (m_pSecurityHandler && !m_bForceUseSecurityHandler) {
        delete m_pSecurityHandler;
        m_pSecurityHandler = NULL;
    }
}
FX_FILESIZE CPDF_Parser::GetObjectOffset(FX_DWORD objnum)
{
    if (objnum >= (FX_DWORD)m_CrossRef.GetSize()) {
        return 0;
    }
    if (m_V5Type[objnum] == 1) {
        return m_CrossRef[objnum];
    }
    if (m_V5Type[objnum] == 2) {
        return m_CrossRef[(FX_INT32)m_CrossRef[objnum]];
    }
    return 0;
}
static FX_INT32 GetDirectInteger(CPDF_Dictionary* pDict, FX_BSTR key)
{
    CPDF_Object* pObj = pDict->GetElement(key);
    if (pObj == NULL) {
        return 0;
    }
    if (pObj->GetType() == PDFOBJ_NUMBER) {
        return ((CPDF_Number*)pObj)->GetInteger();
    }
    return 0;
}
static FX_BOOL CheckDirectType(CPDF_Dictionary* pDict, FX_BSTR key, FX_INT32 iType)
{
    CPDF_Object* pObj = pDict->GetElement(key);
    if (!pObj) {
        return TRUE;
    }
    return pObj->GetType() == iType;
}
FX_BOOL CPDF_Parser::LoadAllCrossRefV4(FX_FILESIZE xrefpos)
{
    if (!LoadCrossRefV4(xrefpos, 0, TRUE, FALSE)) {
        return FALSE;
    }
    m_pTrailer = LoadTrailerV4();
    if (m_pTrailer == NULL) {
        return FALSE;
    }
    FX_INT32 xrefsize = GetDirectInteger(m_pTrailer, FX_BSTRC("Size"));
    if (xrefsize <= 0 || xrefsize > (1 << 20)) {
        return FALSE;
    }
    m_CrossRef.SetSize(xrefsize);
    m_V5Type.SetSize(xrefsize);
    CFX_FileSizeArray CrossRefList, XRefStreamList;
    CrossRefList.Add(xrefpos);
    XRefStreamList.Add(GetDirectInteger(m_pTrailer, FX_BSTRC("XRefStm")));
    if (!CheckDirectType(m_pTrailer, FX_BSTRC("Prev"), PDFOBJ_NUMBER)) {
        return FALSE;
    }
    FX_FILESIZE newxrefpos = GetDirectInteger(m_pTrailer, FX_BSTRC("Prev"));
    if (newxrefpos == xrefpos) {
        return FALSE;
    }
    xrefpos = newxrefpos;
    while (xrefpos) {
        CrossRefList.InsertAt(0, xrefpos);
        LoadCrossRefV4(xrefpos, 0, TRUE, FALSE);
        CPDF_Dictionary* pDict = LoadTrailerV4();
        if (pDict == NULL) {
            return FALSE;
        }
        if (!CheckDirectType(pDict, FX_BSTRC("Prev"), PDFOBJ_NUMBER)) {
            pDict->Release();
            return FALSE;
        }
        newxrefpos = GetDirectInteger(pDict, FX_BSTRC("Prev"));
        if (newxrefpos == xrefpos) {
            pDict->Release();
            return FALSE;
        }
        xrefpos = newxrefpos;
        XRefStreamList.InsertAt(0, pDict->GetInteger(FX_BSTRC("XRefStm")));
        m_Trailers.Add(pDict);
    }
    for (FX_INT32 i = 0; i < CrossRefList.GetSize(); i ++)
        if (!LoadCrossRefV4(CrossRefList[i], XRefStreamList[i], FALSE, i == 0)) {
            return FALSE;
        }
    return TRUE;
}
FX_BOOL CPDF_Parser::LoadCrossRefV4(FX_FILESIZE pos, FX_FILESIZE streampos, FX_BOOL bSkip, FX_BOOL bFirst)
{
    m_Syntax.RestorePos(pos);
    if (m_Syntax.GetKeyword() != FX_BSTRC("xref")) {
        return FALSE;
    }
    if (m_bLinearized && m_LinearizedFirstPageXRefOffset == -1) {
        m_LinearizedFirstPageXRefOffset = m_Syntax.SavePos() - 4;
    }
    FX_LPVOID pResult = FXSYS_bsearch(&pos, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
    if (pResult == NULL) {
        m_SortedOffset.Add(pos);
    }
    if (streampos) {
        FX_LPVOID pResult = FXSYS_bsearch(&streampos, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
        if (pResult == NULL) {
            m_SortedOffset.Add(streampos);
        }
    }
    FX_BOOL bFirstSubSection = TRUE;
    while (1) {
        FX_FILESIZE SavedPos = m_Syntax.SavePos();
        FX_BOOL bIsNumber;
        CFX_ByteString word = m_Syntax.GetNextWord(bIsNumber);
        if (word.IsEmpty()) {
            return FALSE;
        }
        if (!bIsNumber) {
            m_Syntax.RestorePos(SavedPos);
            break;
        }
        FX_DWORD start_objnum = FXSYS_atoi(word);
        if (start_objnum >= (1 << 20)) {
            return FALSE;
        }
        FX_DWORD count = m_Syntax.GetDirectNum();
        m_Syntax.ToNextWord();
        SavedPos = m_Syntax.SavePos();
        FX_BOOL bFirstItem = FALSE;
        FX_INT32 recordsize = 20;
        if (bFirst) {
            bFirstItem = TRUE;
        }
        m_dwXrefStartObjNum = start_objnum;
        if (!bSkip) {
            char* pBuf = FX_Alloc(char, 1024 * recordsize + 1);
            pBuf[1024 * recordsize] = '\0';
            FX_INT32 nBlocks = count / 1024 + 1;
            FX_BOOL bFirstBlock = TRUE;
            for (FX_INT32 block = 0; block < nBlocks; block ++) {
                FX_INT32 block_size = block == nBlocks - 1 ? count % 1024 : 1024;
                m_Syntax.ReadBlock((FX_LPBYTE)pBuf, block_size * recordsize);
                for (FX_INT32 i = 0; i < block_size; i ++) {
                    FX_DWORD objnum = start_objnum + block * 1024 + i;
                    char* pEntry = pBuf + i * recordsize;
                    if (pEntry[17] == 'f') {
                        if (bFirstItem) {
                            objnum = 0;
                            bFirstItem = FALSE;
                        }
                        if (bFirstBlock && bFirstSubSection) {
                            FX_FILESIZE offset = (FX_FILESIZE)FXSYS_atoi64(pEntry);
                            FX_INT32 version = FXSYS_atoi(pEntry + 11);
                            if (offset == 0 && version == 65535 && start_objnum != 0) {
                                start_objnum--;
                                objnum = 0;
                            }
                        }
                        m_CrossRef.SetAtGrow(objnum, 0);
                        m_V5Type.SetAtGrow(objnum, 0);
                    } else {
                        FX_FILESIZE offset = (FX_FILESIZE)FXSYS_atoi64(pEntry);
                        if (offset == 0) {
                            for (FX_INT32 c = 0; c < 10; c ++) {
                                if (pEntry[c] < '0' || pEntry[c] > '9') {
                                    FX_Free(pBuf);
                                    return FALSE;
                                }
                            }
                        }
                        m_CrossRef.SetAtGrow(objnum, offset);
                        FX_INT32 version = FXSYS_atoi(pEntry + 11);
                        if (version >= 1) {
                            m_bVersionUpdated = TRUE;
                        }
                        m_ObjVersion.SetAtGrow(objnum, version);
                        if (m_CrossRef[objnum] < m_Syntax.m_FileLen) {
                            FX_LPVOID pResult = FXSYS_bsearch(&m_CrossRef[objnum], m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
                            if (pResult == NULL) {
                                m_SortedOffset.Add(m_CrossRef[objnum]);
                            }
                        }
                        m_V5Type.SetAtGrow(objnum, 1);
                    }
                    if (bFirstBlock) {
                        bFirstBlock = FALSE;
                    }
                }
            }
            FX_Free(pBuf);
        }
        m_Syntax.RestorePos(SavedPos + count * recordsize);
        bFirstSubSection = FALSE;
    }
    if (streampos)
        if (!LoadCrossRefV5(streampos, streampos, FALSE)) {
            return FALSE;
        }
    return TRUE;
}
FX_BOOL CPDF_Parser::LoadAllCrossRefV5(FX_FILESIZE xrefpos)
{
    if (!m_bInitObjectStreamMap) {
        m_ObjectStreamMap.InitHashTable(101, FALSE);
        m_ObjStmTimeCountMap.InitHashTable(101, FALSE);
        m_bXRefStream = TRUE;
        m_bInitObjectStreamMap = TRUE;
    }
    if (!LoadCrossRefV5(xrefpos, xrefpos, TRUE)) {
        return FALSE;
    }
    while (xrefpos) {
        FX_FILESIZE prePos = xrefpos;
        if (!LoadCrossRefV5(xrefpos, xrefpos, FALSE)) {
            return FALSE;
        }
        if(prePos == xrefpos) {
            return FALSE;
        }
    }
    m_Syntax.m_pRefStreamObjNums = &m_RefStreamObjNums;
    CPDF_PDFVersionChecker::VersionCheck(this, FPDF_FEATURE_XRefStm);
    return TRUE;
}
FX_BOOL CPDF_Parser::RebuildCrossRef()
{
    m_bLinearized = FALSE;
    m_CrossRef.RemoveAll();
    m_V5Type.RemoveAll();
    m_SortedOffset.RemoveAll();
    m_ObjVersion.RemoveAll();
    if (m_pTrailer) {
        m_pTrailer->Release();
        m_pTrailer = NULL;
    }
    FX_INT32 status = 0;
    FX_INT32 inside_index = 0;
    FX_DWORD objnum, gennum;
    FX_INT32 depth = 0;
    FX_LPBYTE buffer = FX_Alloc(FX_BYTE, 4096);
    FX_FILESIZE pos = m_Syntax.m_HeaderOffset;
    FX_FILESIZE start_pos, start_pos1;
    FX_FILESIZE last_obj = -1, last_xref = -1, last_trailer = -1;
    while (pos < m_Syntax.m_FileLen) {
        const FX_FILESIZE saved_pos = pos;
        FX_BOOL bOverFlow = FALSE;
        FX_DWORD size = (FX_DWORD)(m_Syntax.m_FileLen - pos);
        if (size > 4096) {
            size = 4096;
        }
        if (!m_Syntax.m_pFileAccess->ReadBlock(buffer, pos, size)) {
            m_Syntax.m_iStatus = PDFPARSE_ERROR_DATA_SCARCITY;
            break;
        }
        for (FX_DWORD i = 0; i < size; i ++) {
            FX_BYTE byte = buffer[i];
            switch (status) {
                case 0:
                    if (_PDF_CharType[byte] == 'W') {
                        status = 1;
                    }
                    if (byte <= '9' && byte >= '0') {
                        --i;
                        status = 1;
                    }
                    if (byte == '%') {
                        inside_index = 0;
                        status = 9;
                    }
                    if (byte == '(') {
                        status = 10;
                        depth = 1;
                    }
                    if (byte == '<') {
                        inside_index = 1;
                        status = 11;
                    }
                    if (byte == '\\') {
                        status = 13;
                    }
                    if (byte == 't') {
                        status = 7;
                        inside_index = 1;
                    }
                    break;
                case 1:
                    if (_PDF_CharType[byte] == 'W') {
                        break;
                    } else if (byte <= '9' && byte >= '0') {
                        start_pos = pos + i;
                        status = 2;
                        objnum = byte - '0';
                    } else if (byte == 't') {
                        status = 7;
                        inside_index = 1;
                    } else if (byte == 'x') {
                        status = 8;
                        inside_index = 1;
                    } else {
                        --i;
                        status = 0;
                    }
                    break;
                case 2:
                    if (byte <= '9' && byte >= '0') {
                        objnum = objnum * 10 + byte - '0';
                        break;
                    } else if (_PDF_CharType[byte] == 'W') {
                        status = 3;
                    } else {
                        --i;
                        status = 14;
                        inside_index = 0;
                    }
                    break;
                case 3:
                    if (byte <= '9' && byte >= '0') {
                        start_pos1 = pos + i;
                        status = 4;
                        gennum = byte - '0';
                    } else if (_PDF_CharType[byte] == 'W') {
                        break;
                    } else if (byte == 't') {
                        status = 7;
                        inside_index = 1;
                    } else {
                        --i;
                        status = 0;
                    }
                    break;
                case 4:
                    if (byte <= '9' && byte >= '0') {
                        gennum = gennum * 10 + byte - '0';
                        break;
                    } else if (_PDF_CharType[byte] == 'W') {
                        status = 5;
                    } else {
                        --i;
                        status = 0;
                    }
                    break;
                case 5:
                    if (byte == 'o') {
                        status = 6;
                        inside_index = 1;
                    } else if (_PDF_CharType[byte] == 'W') {
                        break;
                    } else if (byte <= '9' && byte >= '0') {
                        objnum = gennum;
                        gennum = byte - '0';
                        start_pos = start_pos1;
                        start_pos1 = pos + i;
                        status = 4;
                    } else if (byte == 't') {
                        status = 7;
                        inside_index = 1;
                    } else {
                        --i;
                        status = 0;
                    }
                    break;
                case 6:
                    switch (inside_index) {
                        case 1:
                            if (byte != 'b') {
                                --i;
                                status = 0;
                            } else {
                                inside_index ++;
                            }
                            break;
                        case 2:
                            if (byte != 'j') {
                                --i;
                                status = 0;
                            } else {
                                inside_index ++;
                            }
                            break;
                        case 3:
                            if (_PDF_CharType[byte] == 'W' || _PDF_CharType[byte] == 'D') {
                                if (objnum > 0x1000000) {
                                    status = 0;
                                    break;
                                }
                                FX_FILESIZE obj_pos = start_pos - m_Syntax.m_HeaderOffset;
                                last_obj = start_pos;
                                FX_LPVOID pResult = FXSYS_bsearch(&obj_pos, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
                                if (pResult == NULL) {
                                    m_SortedOffset.Add(obj_pos);
                                }
                                FX_FILESIZE obj_end = 0;
                                CPDF_Object *pObject = ParseIndirectObjectAtByStrict(m_pDocument, obj_pos, objnum, NULL, &obj_end);
                                if (pObject) {
                                    int iType =	pObject->GetType();
                                    if (iType == PDFOBJ_STREAM) {
                                        CPDF_Stream* pStream = (CPDF_Stream*)pObject;
                                        CPDF_Dictionary* pDict = pStream->GetDict();
                                        if (pDict) {
                                            if (pDict->KeyExist(FX_BSTRC("Type"))) {
                                                CFX_ByteString bsValue = pDict->GetString(FX_BSTRC("Type"));
                                                if (bsValue == FX_BSTRC("XRef") && pDict->KeyExist(FX_BSTRC("Size"))) {
                                                    CPDF_Object* pRoot = pDict->GetElement(FX_BSTRC("Root"));
                                                    if (pRoot && pRoot->GetDict() && pRoot->GetDict()->GetElement(FX_BSTRC("Pages"))) {
                                                        if (m_pTrailer) {
                                                            m_pTrailer->Release();
                                                        }
                                                        m_pTrailer = (CPDF_Dictionary*)pDict->Clone();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                FX_FILESIZE offset = 0;
                                m_Syntax.RestorePos(obj_pos);
                                offset = m_Syntax.FindTag(FX_BSTRC("obj"), 0);
                                if (offset == -1) {
                                    offset = 0;
                                } else {
                                    offset += 3;
                                }
                                FX_FILESIZE nLen = obj_end - obj_pos - offset;
                                if ((FX_DWORD)nLen > size - i) {
                                    pos = obj_end + m_Syntax.m_HeaderOffset;
                                    bOverFlow = TRUE;
                                } else {
                                    i += (FX_DWORD)nLen;
                                }
                                if (m_CrossRef.GetSize() > (FX_INT32)objnum && m_CrossRef[objnum]) {
                                    if (pObject) {
                                        if (GetRootObjNum() == objnum && obj_pos > m_CrossRef[objnum]) {
                                            m_pDocument->ReleaseIndirectObject(objnum);
                                        }
                                        FX_DWORD oldgen = m_ObjVersion.GetAt(objnum);
                                        m_CrossRef[objnum] = obj_pos;
                                        m_ObjVersion.SetAt(objnum, (FX_SHORT)gennum);
                                        if (oldgen != gennum) {
                                            m_bVersionUpdated = TRUE;
                                        }
                                    }
                                } else {
                                    m_CrossRef.SetAtGrow(objnum, obj_pos);
                                    m_V5Type.SetAtGrow(objnum, 1);
                                    m_ObjVersion.SetAtGrow(objnum, (FX_SHORT)gennum);
                                }
                                if (pObject) {
                                    pObject->Release();
                                }
                            }
                            --i;
                            status = 0;
                            break;
                    }
                    break;
                case 7:
                    if (inside_index == 7) {
                        if (_PDF_CharType[byte] == 'W' || _PDF_CharType[byte] == 'D') {
                            last_trailer = pos + i - 7;
                            m_Syntax.RestorePos(pos + i - m_Syntax.m_HeaderOffset);
                            CPDF_Object* pObj = m_Syntax.GetObject(m_pDocument, 0, 0, 0);
                            if (pObj) {
                                if (pObj->GetType() != PDFOBJ_DICTIONARY && pObj->GetType() != PDFOBJ_STREAM) {
                                    pObj->Release();
                                } else {
                                    CPDF_Dictionary* pTrailer = NULL;
                                    if (pObj->GetType() == PDFOBJ_STREAM) {
                                        pTrailer = ((CPDF_Stream*)pObj)->GetDict();
                                    } else {
                                        pTrailer = (CPDF_Dictionary*)pObj;
                                    }
                                    if (pTrailer) {
                                        if (m_pTrailer) {
                                            CPDF_Object* pRoot = pTrailer->GetElement(FX_BSTRC("Root"));
                                            if (pRoot == NULL || (pRoot->GetType() == PDFOBJ_REFERENCE &&
                                                                  (FX_DWORD)m_CrossRef.GetSize() > ((CPDF_Reference*)pRoot)->GetRefObjNum() &&
                                                                  m_CrossRef.GetAt(((CPDF_Reference*)pRoot)->GetRefObjNum()) != 0)) {
                                                FX_POSITION trailer_pos = pTrailer->GetStartPos();
                                                while (trailer_pos) {
                                                    CFX_ByteString key;
                                                    CPDF_Object* pObj = pTrailer->GetNextElement(trailer_pos, key);
                                                    m_pTrailer->SetAt(key, pObj->Clone(), m_pDocument);
                                                }
                                                pObj->Release();
                                            } else {
                                                pObj->Release();
                                            }
                                        } else {
                                            if (pObj->GetType() == PDFOBJ_STREAM) {
                                                m_pTrailer = (CPDF_Dictionary*)pTrailer->Clone();
                                                pObj->Release();
                                            } else {
                                                m_pTrailer = pTrailer;
                                            }
                                            FX_FILESIZE dwSavePos = m_Syntax.SavePos();
                                            CFX_ByteString strWord = m_Syntax.GetKeyword();
                                            if (!strWord.Compare(FX_BSTRC("startxref"))) {
                                                FX_BOOL bNumber = FALSE;
                                                CFX_ByteString bsOffset = m_Syntax.GetNextWord(bNumber);
                                                if (bNumber) {
                                                    m_LastXRefOffset = FXSYS_atoi(bsOffset);
                                                }
                                            }
                                            m_Syntax.RestorePos(dwSavePos);
                                        }
                                    } else {
                                        pObj->Release();
                                    }
                                }
                            }
                        }
                        --i;
                        status = 0;
                    } else if (byte == "trailer"[inside_index]) {
                        inside_index ++;
                    } else {
                        --i;
                        status = 0;
                    }
                    break;
                case 8:
                    if (inside_index == 4) {
                        last_xref = pos + i - 4;
                        status = 1;
                    } else if (byte == "xref"[inside_index]) {
                        inside_index ++;
                    } else {
                        --i;
                        status = 0;
                    }
                    break;
                case 9:
                    if (byte == '\r' || byte == '\n') {
                        status = 0;
                    }
                    break;
                case 10:
                    if (byte == ')') {
                        if (depth > 0) {
                            depth--;
                        }
                    } else if (byte == '(') {
                        depth++;
                    }
                    if (!depth) {
                        status = 0;
                    }
                    break;
                case 11:
                    if (byte == '<' && inside_index == 1) {
                        status = 12;
                    } else if (byte == '>') {
                        status = 0;
                    }
                    inside_index = 0;
                    break;
                case 12:
                    --i;
                    status = 0;
                    break;
                case 13:
                    if (_PDF_CharType[byte] == 'D' || _PDF_CharType[byte] == 'W') {
                        --i;
                        status = 0;
                    }
                    break;
                case 14:
                    if (_PDF_CharType[byte] == 'W') {
                        status = 0;
                    } else if (byte == '%' || byte == '(' || byte == '<' || byte == '\\') {
                        status = 0;
                        --i;
                    } else if (inside_index == 6) {
                        status = 0;
                        --i;
                    } else if (byte == "endobj"[inside_index]) {
                        inside_index++;
                    }
                    break;
            }
            if (bOverFlow) {
                size = 0;
                break;
            }
        }
        pos += size;
        if (pos == saved_pos) {
            break;
        }
    }
    if (last_xref != -1 && last_xref > last_obj) {
        last_trailer = last_xref;
    } else if (last_trailer == -1 || last_xref < last_obj) {
        last_trailer = m_Syntax.m_FileLen;
    }
    FX_FILESIZE offset = last_trailer - m_Syntax.m_HeaderOffset;
    FX_LPVOID pResult = FXSYS_bsearch(&offset, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
    if (pResult == NULL) {
        m_SortedOffset.Add(offset);
    }
    FX_Free(buffer);
    m_bRebuildXRef = TRUE;
    return TRUE;
}
static FX_FILESIZE _GetVarInt(FX_LPCBYTE p, FX_INT32 n)
{
    FX_FILESIZE result = 0;
    for (FX_INT32 i = 0; i < n; i ++) {
        result = result * 256 + p[i];
    }
    return result;
}
FX_BOOL CPDF_Parser::LoadCrossRefV5(FX_FILESIZE pos, FX_FILESIZE& prev, FX_BOOL bMainXRef)
{
    CPDF_Stream* pStream = (CPDF_Stream*)ParseIndirectObjectAt(m_pDocument, pos, 0, NULL);
    if (!pStream) {
        return FALSE;
    }
    if (pStream->GetType() != PDFOBJ_STREAM) {
        pStream->Destroy();
        return FALSE;
    }
    m_RefStreamObjNums.Add(pStream->m_ObjNum);
    if (m_bLinearized && m_LinearizedFirstPageXRefOffset == -1) {
        m_LinearizedFirstPageXRefOffset = pos;
    }
    prev = pStream->GetDict()->GetInteger64(FX_BSTRC("Prev"));
    FX_INT32 size = pStream->GetDict()->GetInteger(FX_BSTRC("Size"));
    if (size < 0) {
        pStream->Destroy();
        return FALSE;
    }
    if (bMainXRef) {
        m_pTrailer = (CPDF_Dictionary*)pStream->GetDict()->Clone();
        if (m_pDocument) {
            size = FX_MAX(size, (FX_INT32)(m_pDocument->GetLastObjNum() + 1));
        }
        FX_BOOL bRet = m_CrossRef.SetSize(size);
        if (!bRet) {
            return FALSE;
        }
        if (m_V5Type.SetSize(size)) {
            FXSYS_memset32(m_V5Type.GetData(), 0, size);
        }
    } else {
        m_Trailers.Add((CPDF_Dictionary*)pStream->GetDict()->Clone());
        if (size > m_V5Type.GetSize()) {
            m_CrossRef.SetSize(size);
            m_V5Type.SetSize(size);
        }
    }
    CFX_DWordArray IndexArray, WidthArray;
    FX_DWORD nSegs = 0;
    CPDF_Array* pArray = pStream->GetDict()->GetArray(FX_BSTRC("Index"));
    if (pArray == NULL) {
        IndexArray.Add(0);
        IndexArray.Add(size);
        nSegs = 1;
    } else {
        for (FX_DWORD i = 0; i < pArray->GetCount(); i ++) {
            IndexArray.Add(pArray->GetInteger(i));
        }
        nSegs = pArray->GetCount() / 2;
    }
    pArray = pStream->GetDict()->GetArray(FX_BSTRC("W"));
    if (pArray == NULL) {
        pStream->Destroy();
        return FALSE;
    }
    FX_DWORD totalwidth = 0;
    FX_DWORD i;
    for (i = 0; i < pArray->GetCount(); i ++) {
        WidthArray.Add(pArray->GetInteger(i));
        if (totalwidth + WidthArray[i] < totalwidth) {
            pStream->Destroy();
            return FALSE;
        }
        totalwidth += WidthArray[i];
    }
    if (totalwidth == 0 || WidthArray.GetSize() < 3) {
        pStream->Destroy();
        return FALSE;
    }
    CPDF_StreamAcc acc;
    acc.LoadAllData(pStream);
    FX_LPCBYTE pData = acc.GetData();
    FX_DWORD dwTotalSize = acc.GetSize();
    FX_DWORD segindex = 0;
    for (i = 0; i < nSegs; i ++) {
        FX_INT32 startnum = IndexArray[i * 2];
        if (startnum < 0) {
            continue;
        }
        m_dwXrefStartObjNum = startnum;
        FX_DWORD count = IndexArray[i * 2 + 1];
        if ((segindex + count) * (FX_DWORD)totalwidth > dwTotalSize) {
            count = dwTotalSize / totalwidth - segindex;
        }
        if (segindex + count < segindex || segindex + count == 0 || (FX_DWORD)totalwidth >= UINT_MAX / (segindex + count)) {
            continue;
        }
        FX_LPCBYTE segstart = pData + segindex * (FX_DWORD)totalwidth;
        if ((FX_DWORD)startnum + count < (FX_DWORD)startnum ||
                (FX_DWORD)startnum + count > (FX_DWORD)m_V5Type.GetSize()) {
            continue;
        }
        for (FX_DWORD j = 0; j < count; j ++) {
            FX_INT32 type = 1;
            FX_LPCBYTE entrystart = segstart + j * totalwidth;
            if (WidthArray[0]) {
                type = _GetVarInt(entrystart, WidthArray[0]);
            }
            if (m_V5Type[startnum + j] == 255) {
                FX_FILESIZE offset = _GetVarInt(entrystart + WidthArray[0], WidthArray[1]);
                m_CrossRef[startnum + j] = offset;
                FX_LPVOID pResult = FXSYS_bsearch(&offset, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
                if (pResult == NULL) {
                    m_SortedOffset.Add(offset);
                }
                continue;
            }
            if (m_V5Type[startnum + j]) {
                continue;
            }
            m_V5Type[startnum + j] = type;
            if (type == 0) {
                m_CrossRef[startnum + j] = 0;
            } else {
                FX_FILESIZE offset = _GetVarInt(entrystart + WidthArray[0], WidthArray[1]);
                m_CrossRef[startnum + j] = offset;
                if (type == 1) {
                    FX_WORD genenum = _GetVarInt(entrystart + WidthArray[0] + WidthArray[1], WidthArray[2]);
                    if (genenum >= 1) {
                        m_bVersionUpdated = TRUE;
                    }
                    m_ObjVersion.SetAtGrow(startnum + j, genenum);
                    FX_LPVOID pResult = FXSYS_bsearch(&offset, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
                    if (pResult == NULL) {
                        m_SortedOffset.Add(offset);
                    }
                } else {
                    if (offset < 0 || offset >= m_V5Type.GetSize()) {
                        pStream->Destroy();
                        return FALSE;
                    }
                    m_V5Type[offset] = 255;
                }
            }
        }
        segindex += count;
    }
    pStream->Destroy();
    return TRUE;
}
CPDF_Array* CPDF_Parser::GetIDArray()
{
    CPDF_Object* pID = m_pTrailer ? m_pTrailer->GetElement(FX_BSTRC("ID")) : NULL;
    if (pID == NULL) {
        return NULL;
    }
    if (pID->GetType() == PDFOBJ_REFERENCE) {
        pID = ParseIndirectObject(NULL, ((CPDF_Reference*)pID)->GetRefObjNum());
        m_pTrailer->SetAt(FX_BSTRC("ID"), pID);
    }
    if (pID == NULL || pID->GetType() != PDFOBJ_ARRAY) {
        return NULL;
    }
    return (CPDF_Array*)pID;
}
FX_DWORD CPDF_Parser::GetRootObjNum()
{
    CPDF_Object* pRef = m_pTrailer ? m_pTrailer->GetElement(FX_BSTRC("Root")) : NULL;
    if (pRef == NULL || pRef->GetType() != PDFOBJ_REFERENCE) {
        return 0;
    }
    return ((CPDF_Reference*) pRef)->GetRefObjNum();
}
FX_DWORD CPDF_Parser::GetInfoObjNum()
{
    CPDF_Object* pRef = m_pTrailer ? m_pTrailer->GetElement(FX_BSTRC("Info")) : NULL;
    if (pRef == NULL || pRef->GetType() != PDFOBJ_REFERENCE) {
        return 0;
    }
    return ((CPDF_Reference*) pRef)->GetRefObjNum();
}
CPDF_Object* CPDF_Parser::ParseIndirectObject(CPDF_IndirectObjects* pObjList, FX_DWORD objnum, PARSE_CONTEXT* pContext)
{
    if (objnum >= (FX_DWORD)m_CrossRef.GetSize()) {
        return NULL;
    }
    if (m_V5Type[objnum] == 1 || m_V5Type[objnum] == 255) {
        FX_FILESIZE pos = m_CrossRef[objnum];
        if (pos <= 0) {
            return NULL;
        }
        return ParseIndirectObjectAt(pObjList, pos, objnum, pContext);
    }
    if (m_V5Type[objnum] == 2) {
        CPDF_StreamAcc* pObjStream = GetObjectStream((FX_DWORD)m_CrossRef[objnum]);
        if (pObjStream == NULL) {
            return NULL;
        }
        FX_INT32 n = pObjStream->GetDict()->GetInteger(FX_BSTRC("N"));
        if (n <= 0) {
            return NULL;
        }
        FX_FILESIZE offset = pObjStream->GetDict()->GetInteger(FX_BSTRC("First"));
        if (offset <= 0) {
            return NULL;
        }
        CFX_AutoRestorer<FX_INT32> autoRestorer(&m_nLevel);
        if (m_nLevel++ > _PARSER_OBJECT_LEVLE_) {
            return NULL;
        }
        CPDF_SyntaxParser syntax;
        CFX_SmartPointer<IFX_FileStream> file(FX_CreateMemoryStream((FX_LPBYTE)pObjStream->GetData(), (size_t)pObjStream->GetSize(), FALSE));
        syntax.InitParser((IFX_FileStream*)file.Get(), 0);
        CPDF_Object* pRet = NULL;
        while (n) {
            FX_FILESIZE thisnum = (FX_FILESIZE)syntax.GetDirectNum64();
            FX_FILESIZE thisoff = (FX_FILESIZE)syntax.GetDirectNum64();
            if (thisnum == objnum) {
                syntax.RestorePos(offset + thisoff);
                pRet = syntax.GetObject(pObjList, 0, 0, 0, pContext);
                break;
            }
            n --;
        }
        return pRet;
    }
    return NULL;
}
struct OBJSTM_CACHEINFO {
    FX_DWORD time;
    FX_DWORD objnum;
};
extern "C" {
    static int compare(const void* data1, const void* data2)
    {
        return ((OBJSTM_CACHEINFO*)data1)->time - ((OBJSTM_CACHEINFO*)data2)->time;
    }
};
CPDF_StreamAcc* CPDF_Parser::GetObjectStream(FX_DWORD objnum)
{
    CPDF_StreamAcc* pStreamAcc = NULL;
    if (m_ObjectStreamMap.Lookup((void*)(FX_UINTPTR)objnum, (void*&)pStreamAcc)) {
        if (pStreamAcc && pStreamAcc->GetStream()) {
            const CPDF_Stream* pStream = pStreamAcc->GetStream();
            CPDF_Object* pStreamObj = m_pDocument ? m_pDocument->GetIndirectObject(objnum) : NULL;
            if (pStream != pStreamObj) {
                delete pStreamAcc;
                m_ObjectStreamMap.RemoveKey((void*)(FX_UINTPTR)objnum);
            } else {
                m_nTimeCount++;
                m_ObjStmTimeCountMap.SetAt((void*)(FX_UINTPTR)objnum, (void*)(FX_UINTPTR)m_nTimeCount);
                return pStreamAcc;
            }
        }
    }
    const CPDF_Stream* pStream = m_pDocument ? (CPDF_Stream*)m_pDocument->GetIndirectObject(objnum) : NULL;
    if (pStream == NULL || pStream->GetType() != PDFOBJ_STREAM) {
        return NULL;
    }
    pStreamAcc = FX_NEW CPDF_StreamAcc;
    pStreamAcc->LoadAllData(pStream);
    m_ObjectStreamMap.SetAt((void*)(FX_UINTPTR)objnum, pStreamAcc);
    m_nTimeCount++;
    m_ObjStmTimeCountMap.SetAt((void*)(FX_UINTPTR)objnum, (void*)(FX_UINTPTR)m_nTimeCount);
    return pStreamAcc;
}
FX_FILESIZE CPDF_Parser::GetObjectSize(FX_DWORD objnum)
{
    if (objnum >= (FX_DWORD)m_CrossRef.GetSize()) {
        return 0;
    }
    if (m_V5Type[objnum] == 2) {
        objnum = (FX_DWORD)m_CrossRef[objnum];
    }
    if (m_V5Type[objnum] == 1 || m_V5Type[objnum] == 255) {
        FX_FILESIZE offset = m_CrossRef[objnum];
        if (offset == 0) {
            return 0;
        }
        FX_LPVOID pResult = FXSYS_bsearch(&offset, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
        if (pResult == NULL) {
            return 0;
        }
        if ((FX_FILESIZE*)pResult - (FX_FILESIZE*)m_SortedOffset.GetData() == m_SortedOffset.GetSize() - 1) {
            return 0;
        }
        return ((FX_FILESIZE*)pResult)[1] - offset;
    }
    return 0;
}
int CPDF_Parser::GetObjectVersion(FX_DWORD objnum)
{
    if(objnum >= (FX_DWORD)m_ObjVersion.GetSize()) {
        return 0;
    }
    return m_ObjVersion[objnum];
}
void CPDF_Parser::GetIndirectBinary(FX_DWORD objnum, FX_LPBYTE& pBuffer, FX_DWORD& size)
{
    pBuffer = NULL;
    size = 0;
    if (objnum >= (FX_DWORD)m_CrossRef.GetSize()) {
        return;
    }
    if (m_V5Type[objnum] == 2) {
        CPDF_StreamAcc* pObjStream = GetObjectStream((FX_DWORD)m_CrossRef[objnum]);
        if (pObjStream == NULL) {
            return;
        }
        FX_INT32 n = pObjStream->GetDict()->GetInteger(FX_BSTRC("N"));
        FX_INT32 offset = pObjStream->GetDict()->GetInteger(FX_BSTRC("First"));
        CPDF_SyntaxParser syntax;
        FX_LPCBYTE pData = pObjStream->GetData();
        FX_DWORD totalsize = pObjStream->GetSize();
        CFX_SmartPointer<IFX_FileStream> file(FX_CreateMemoryStream((FX_LPBYTE)pData, (size_t)totalsize, FALSE));
        syntax.InitParser(file.Get(), 0);
        while (n) {
            FX_DWORD thisnum = syntax.GetDirectNum();
            FX_DWORD thisoff = syntax.GetDirectNum();
            if (thisnum == objnum) {
                if (n == 1) {
                    size = totalsize - (thisoff + offset);
                } else {
                    FX_DWORD nextnum = syntax.GetDirectNum();
                    FX_DWORD nextoff = syntax.GetDirectNum();
                    if(nextoff < thisoff) {
                        return;
                    }
                    size = nextoff - thisoff;
                }
                if(size > totalsize) {
                    return;
                }
                pBuffer = FX_Alloc(FX_BYTE, size);
                if (pBuffer) {
                    FXSYS_memcpy32(pBuffer, pData + thisoff + offset, size);
                }
                return;
            }
            n --;
        }
        return;
    }
    if (m_V5Type[objnum] == 1) {
        FX_FILESIZE pos = m_CrossRef[objnum];
        if (pos == 0) {
            return;
        }
        FX_FILESIZE SavedPos = m_Syntax.SavePos();
        m_Syntax.RestorePos(pos);
        FX_BOOL bIsNumber;
        CFX_ByteString word = m_Syntax.GetNextWord(bIsNumber);
        if (!bIsNumber) {
            m_Syntax.RestorePos(SavedPos);
            return;
        }
        FX_DWORD real_objnum = FXSYS_atoi(word);
        if (real_objnum && real_objnum != objnum) {
            m_Syntax.RestorePos(SavedPos);
            return;
        }
        word = m_Syntax.GetNextWord(bIsNumber);
        if (!bIsNumber) {
            m_Syntax.RestorePos(SavedPos);
            return;
        }
        if (m_Syntax.GetKeyword() != FX_BSTRC("obj")) {
            m_Syntax.RestorePos(SavedPos);
            return;
        }
        FX_LPVOID pResult = FXSYS_bsearch(&pos, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
        if (pResult == NULL) {
            m_Syntax.RestorePos(SavedPos);
            return;
        }
        FX_FILESIZE nextoff = ((FX_FILESIZE*)pResult + 1 < (FX_FILESIZE*)m_SortedOffset.GetData() + m_SortedOffset.GetSize() ? ((FX_FILESIZE*)pResult)[1] : 0);
        FX_BOOL bNextOffValid = FALSE;
        if (nextoff > pos) {
            m_Syntax.RestorePos(nextoff);
            word = m_Syntax.GetNextWord(bIsNumber);
            if (word == FX_BSTRC("xref")) {
                bNextOffValid = TRUE;
            } else if (bIsNumber) {
                word = m_Syntax.GetNextWord(bIsNumber);
                if (bIsNumber && m_Syntax.GetKeyword() == FX_BSTRC("obj")) {
                    bNextOffValid = TRUE;
                }
            }
        }
        if (!bNextOffValid) {
            m_Syntax.RestorePos(pos);
            while (1) {
                FX_FILESIZE pos = m_Syntax.SavePos();
                if (m_Syntax.GetKeyword() == FX_BSTRC("endobj")) {
                    break;
                }
                if (m_Syntax.SavePos() == m_Syntax.m_FileLen || m_Syntax.SavePos() == pos) {
                    break;
                }
            }
            nextoff = m_Syntax.SavePos();
        }
        size = (FX_DWORD)(nextoff - pos);
        pBuffer = FX_Alloc(FX_BYTE, size);
        m_Syntax.RestorePos(pos);
        m_Syntax.ReadBlock(pBuffer, size);
        m_Syntax.RestorePos(SavedPos);
    }
}
FX_BOOL CPDF_Parser::GetIndirectBinary(FX_DWORD objnum, FX_LPBYTE& pBuffer, FX_DWORD& buffSize, FX_DWORD& leftSize, FX_FILESIZE& savedPos)
{
#define BLOCK_SIZE   1024*1024*10
    if(pBuffer == NULL) {
        if (objnum >= (FX_DWORD)m_CrossRef.GetSize()) {
            return FALSE;
        }
        if (m_V5Type[objnum] != 1) {
            return FALSE;
        }
        FX_FILESIZE pos = m_CrossRef[objnum];
        if (pos == 0) {
            return FALSE;
        }
        savedPos = m_Syntax.SavePos();
        m_Syntax.RestorePos(pos);
        FX_BOOL bIsNumber;
        CFX_ByteString word = m_Syntax.GetNextWord(bIsNumber);
        if (!bIsNumber) {
            m_Syntax.RestorePos(savedPos);
            return FALSE;
        }
        FX_DWORD real_objnum = FXSYS_atoi(word);
        if (real_objnum && real_objnum != objnum) {
            m_Syntax.RestorePos(savedPos);
            return FALSE;
        }
        word = m_Syntax.GetNextWord(bIsNumber);
        if (!bIsNumber) {
            m_Syntax.RestorePos(savedPos);
            return FALSE;
        }
        if (m_Syntax.GetKeyword() != FX_BSTRC("obj")) {
            m_Syntax.RestorePos(savedPos);
            return FALSE;
        }
        FX_LPVOID pResult = FXSYS_bsearch(&pos, m_SortedOffset.GetData(), m_SortedOffset.GetSize(), sizeof(FX_FILESIZE), _CompareFileSize);
        if (pResult == NULL) {
            m_Syntax.RestorePos(savedPos);
            return FALSE;
        }
        FX_FILESIZE nextoff = ((FX_FILESIZE*)pResult + 1 < (FX_FILESIZE*)m_SortedOffset.GetData() + m_SortedOffset.GetSize() ? ((FX_FILESIZE*)pResult)[1] : 0);
        FX_BOOL bNextOffValid = FALSE;
        if (nextoff > pos) {
            m_Syntax.RestorePos(nextoff);
            word = m_Syntax.GetNextWord(bIsNumber);
            if (word == FX_BSTRC("xref")) {
                bNextOffValid = TRUE;
            } else if (bIsNumber) {
                word = m_Syntax.GetNextWord(bIsNumber);
                if (bIsNumber && m_Syntax.GetKeyword() == FX_BSTRC("obj")) {
                    bNextOffValid = TRUE;
                }
            }
        }
        if (!bNextOffValid) {
            m_Syntax.RestorePos(pos);
            while (1) {
                FX_FILESIZE pos = m_Syntax.SavePos();
                if (m_Syntax.GetKeyword() == FX_BSTRC("endobj")) {
                    break;
                }
                if (m_Syntax.SavePos() == m_Syntax.m_FileLen || m_Syntax.SavePos() == pos) {
                    break;
                }
            }
            nextoff = m_Syntax.SavePos();
        }
        FX_DWORD size = (FX_DWORD)(nextoff - pos);
        m_Syntax.RestorePos(pos);
        if(size > BLOCK_SIZE) {
            pBuffer = FX_Alloc(FX_BYTE, BLOCK_SIZE);
            buffSize = BLOCK_SIZE;
        } else {
            pBuffer = FX_Alloc(FX_BYTE, size);
            buffSize = size;
        }
        leftSize = size;
    }
    if(leftSize <= buffSize) {
        m_Syntax.ReadBlock(pBuffer, leftSize);
        m_Syntax.RestorePos(savedPos);
        buffSize = leftSize;
        leftSize = 0;
    } else {
        m_Syntax.ReadBlock(pBuffer, buffSize);
        leftSize -= buffSize;
    }
    return TRUE;
}
CPDF_Object* CPDF_Parser::ParseIndirectObjectAt(CPDF_IndirectObjects* pObjList, FX_FILESIZE pos, FX_DWORD objnum,
        PARSE_CONTEXT* pContext)
{
    FX_FILESIZE SavedPos = m_Syntax.SavePos();
    m_Syntax.RestorePos(pos);
    FX_BOOL bIsNumber;
    CFX_ByteString word = m_Syntax.GetNextWord(bIsNumber);
    if (!bIsNumber) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    FX_FILESIZE objOffset = m_Syntax.SavePos();
    objOffset -= word.GetLength();
    FX_DWORD real_objnum = FXSYS_atoi(word);
    if (objnum && real_objnum != objnum) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    word = m_Syntax.GetNextWord(bIsNumber);
    if (!bIsNumber) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    FX_DWORD gennum = FXSYS_atoi(word);
    if(gennum == 0xFFFFFFFF) {
        gennum--;
    }
    if (m_Syntax.GetKeyword() != FX_BSTRC("obj")) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    CPDF_Object* pObj = m_Syntax.GetObject(pObjList, objnum, gennum, 0, pContext);
    FX_FILESIZE endOffset = m_Syntax.SavePos();
    CFX_ByteString bsWord = m_Syntax.GetKeyword();
    if (bsWord == FX_BSTRC("endobj")) {
        endOffset = m_Syntax.SavePos();
    }
    FX_DWORD objSize = endOffset - objOffset;
    m_Syntax.RestorePos(SavedPos);
    if (pObj && !objnum) {
        pObj->m_ObjNum = real_objnum;
    }
    return pObj;
}
CPDF_Object* CPDF_Parser::ParseIndirectObjectAtByStrict(CPDF_IndirectObjects* pObjList, FX_FILESIZE pos, FX_DWORD objnum,
        struct PARSE_CONTEXT* pContext, FX_FILESIZE *pResultPos)
{
    FX_FILESIZE SavedPos = m_Syntax.SavePos();
    m_Syntax.RestorePos(pos);
    FX_BOOL bIsNumber;
    CFX_ByteString word = m_Syntax.GetNextWord(bIsNumber);
    if (!bIsNumber) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    FX_DWORD real_objnum = FXSYS_atoi(word);
    if (objnum && real_objnum != objnum) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    word = m_Syntax.GetNextWord(bIsNumber);
    if (!bIsNumber) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    FX_DWORD gennum = FXSYS_atoi(word);
    if(gennum == 0xFFFFFFFF) {
        gennum--;
    }
    if (m_Syntax.GetKeyword() != FX_BSTRC("obj")) {
        m_Syntax.RestorePos(SavedPos);
        return NULL;
    }
    CPDF_Object* pObj = m_Syntax.GetObjectByStrict(pObjList, objnum, gennum, 0, pContext);
    if (pResultPos) {
        *pResultPos = m_Syntax.m_Pos;
    }
    m_Syntax.RestorePos(SavedPos);
    return pObj;
}
CPDF_Dictionary* CPDF_Parser::LoadTrailerV4()
{
    if (m_Syntax.GetKeyword() != FX_BSTRC("trailer")) {
        return NULL;
    }
    CPDF_Object* pObj = m_Syntax.GetObject(m_pDocument, 0, 0, 0);
    if (pObj == NULL || pObj->GetType() != PDFOBJ_DICTIONARY) {
        if (pObj) {
            pObj->Release();
        }
        return NULL;
    }
    return (CPDF_Dictionary*)pObj;
}
FX_DWORD CPDF_Parser::GetPermissions(FX_BOOL bCheckRevision)
{
    if (m_pSecurityHandler == NULL) {
        return (FX_DWORD) - 1;
    }
    FX_DWORD dwPermission = m_pSecurityHandler->GetPermissions();
    if (m_pEncryptDict && m_pEncryptDict->GetString(FX_BSTRC("Filter")) == FX_BSTRC("Standard")) {
        dwPermission &= 0xFFFFFFFC;
        dwPermission |= 0xFFFFF0C0;
        if(bCheckRevision && m_pEncryptDict->GetInteger(FX_BSTRC("R")) == 2) {
            dwPermission &= 0xFFFFF0FF;
        }
    }
    return dwPermission;
}
FX_BOOL CPDF_Parser::IsOwner()
{
    return m_pSecurityHandler == NULL ? TRUE : m_pSecurityHandler->IsOwner();
}
void CPDF_Parser::SetPassword(const FX_CHAR* password)
{
    m_Password = password;
}
void CPDF_Parser::SetSecurityHandler(CPDF_SecurityHandler* pSecurityHandler, FX_BOOL bForced)
{
    ASSERT(m_pSecurityHandler == NULL);
    if (m_pSecurityHandler && !m_bForceUseSecurityHandler) {
        delete m_pSecurityHandler;
        m_pSecurityHandler = NULL;
    }
    m_bForceUseSecurityHandler = bForced;
    m_pSecurityHandler = pSecurityHandler;
    if (m_bForceUseSecurityHandler) {
        return;
    }
    m_Syntax.crypto_handler_ = pSecurityHandler->CreateCryptoHandler();
    m_Syntax.crypto_handler_->Init(NULL, pSecurityHandler);
}
void CPDF_Parser::DeleteIndirectObject(FX_DWORD objnum)
{
    if (objnum >= (FX_DWORD)m_CrossRef.GetSize()) {
        return;
    }
    m_V5Type[objnum] = 0;
}
FX_BOOL CPDF_Parser::IsLinearizedFile(IFX_FileRead* pFileAccess, FX_DWORD offset)
{
    if (pFileAccess->GetSize() < 4096) {
        return FALSE;
    }
    m_Syntax.RestorePos(m_Syntax.m_HeaderOffset + 9);
    FX_FILESIZE SavedPos = m_Syntax.SavePos();
    FX_BOOL bIsNumber;
    CFX_ByteString word = m_Syntax.GetNextWord(bIsNumber);
    if (!bIsNumber) {
        return FALSE;
    }
    FX_DWORD objnum = FXSYS_atoi(word);
    word = m_Syntax.GetNextWord(bIsNumber);
    if (!bIsNumber) {
        return FALSE;
    }
    FX_DWORD gennum = FXSYS_atoi(word);
    if(gennum == 0xFFFFFFFF) {
        gennum--;
    }
    if (m_Syntax.GetKeyword() != FX_BSTRC("obj")) {
        m_Syntax.RestorePos(SavedPos);
        return FALSE;
    }
    m_pLinearized = m_Syntax.GetObject(NULL, objnum, gennum, 0);
    if (!m_pLinearized) {
        return FALSE;
    }
    CPDF_Dictionary* pLinearizedDict = m_pLinearized->GetDict();
    if (pLinearizedDict
            && pLinearizedDict->KeyExist(FX_BSTRC("Linearized"))
            && pLinearizedDict->KeyExist(FX_BSTRC("L"))
            && pLinearizedDict->KeyExist(FX_BSTRC("H"))
            && pLinearizedDict->KeyExist(FX_BSTRC("O"))
            && pLinearizedDict->KeyExist(FX_BSTRC("E"))
            && pLinearizedDict->KeyExist(FX_BSTRC("N"))
            && pLinearizedDict->KeyExist(FX_BSTRC("T"))) {
        m_Syntax.GetNextWord(bIsNumber);
        CPDF_Object *pObj = pLinearizedDict->GetElement(FX_BSTRC("Linearized"));
        if (!pObj || pObj->GetType() != PDFOBJ_NUMBER || pObj->GetInteger() != 1) {
            m_pLinearized->Release();
            m_pLinearized = NULL;
            return FALSE;
        }
        pObj = pLinearizedDict->GetElement(FX_BSTRC("L"));
        if (!pObj || pObj->GetType() != PDFOBJ_NUMBER || pObj->GetInteger64() != pFileAccess->GetSize()) {
            m_pLinearized->Release();
            m_pLinearized = NULL;
            return FALSE;
        }
        CPDF_Object *pNo = pLinearizedDict->GetElement(FX_BSTRC("P"));
        if (pNo && pNo->GetType() == PDFOBJ_NUMBER) {
            m_dwFirstPageNo = pNo->GetInteger();
        }
        CPDF_Object *pTable = pLinearizedDict->GetElement(FX_BSTRC("T"));
        if (pTable && pTable->GetType() == PDFOBJ_NUMBER) {
            m_LastXRefOffset = pTable->GetInteger();
        }
        m_bLinearized = TRUE;
        return TRUE;
    }
    m_pLinearized->Release();
    m_pLinearized = NULL;
    return FALSE;
}
CPDF_SyntaxParser::CPDF_SyntaxParser()
{
    m_pFileAccess = NULL;
    crypto_handler_ = NULL;
    m_pFileBuf = NULL;
    m_BufSize = CPDF_ModuleMgr::Get()->m_FileBufSize;
    m_pFileBuf = NULL;
    m_MetadataObjnum = 0;
    m_pRefStreamObjNums = NULL;
    m_dwWordPos = 0;
#if defined(_FPDFAPI_MINI_)
    m_bFileStream = TRUE;
#else
    m_bFileStream = FALSE;
#endif
    m_Level = 0;
    m_TotalMemoryStreamSize = 0;
    m_iStatus = PDFPARSE_ERROR_SUCCESS;
}
CPDF_SyntaxParser::~CPDF_SyntaxParser()
{
    if (m_pFileBuf) {
        FX_Free(m_pFileBuf);
    }
}
FX_BOOL CPDF_SyntaxParser::GetCharAt(FX_FILESIZE pos, FX_BYTE& ch)
{
    FX_FILESIZE save_pos = m_Pos;
    m_Pos = pos;
    FX_BOOL ret = GetNextChar(ch);
    m_Pos = save_pos;
    return ret;
}
FX_BOOL CPDF_SyntaxParser::GetNextChar(FX_BYTE& ch)
{
    FX_FILESIZE pos = m_Pos + m_HeaderOffset;
    if (pos >= m_FileLen) {
        return FALSE;
    }
    if (m_BufOffset >= pos || (FX_FILESIZE)(m_BufOffset + m_BufSize) <= pos) {
        FX_FILESIZE read_pos = pos;
        FX_DWORD read_size = m_BufSize;
        if ((FX_FILESIZE)read_size > m_FileLen) {
            read_size = (FX_DWORD)m_FileLen;
        }
        if ((FX_FILESIZE)(read_pos + read_size) > m_FileLen) {
            if (m_FileLen < (FX_FILESIZE)read_size) {
                read_pos = 0;
                read_size = (FX_DWORD)m_FileLen;
            } else {
                read_pos = m_FileLen - read_size;
            }
        }
        if (!m_pFileAccess->ReadBlock(m_pFileBuf, read_pos, read_size)) {
            m_iStatus = PDFPARSE_ERROR_DATA_SCARCITY;
            return FALSE;
        }
        m_BufOffset = read_pos;
    }
    ch = m_pFileBuf[pos - m_BufOffset];
    m_Pos ++;
    return TRUE;
}
FX_BOOL CPDF_SyntaxParser::GetCharAtBackward(FX_FILESIZE pos, FX_BYTE& ch)
{
    pos += m_HeaderOffset;
    if (pos >= m_FileLen) {
        return FALSE;
    }
    if (m_BufOffset >= pos || (FX_FILESIZE)(m_BufOffset + m_BufSize) <= pos) {
        FX_FILESIZE read_pos;
        if (pos < (FX_FILESIZE)m_BufSize) {
            read_pos = 0;
        } else {
            read_pos = pos - m_BufSize + 1;
        }
        FX_DWORD read_size = m_BufSize;
        if ((FX_FILESIZE)(read_pos + read_size) > m_FileLen) {
            if (m_FileLen < (FX_FILESIZE)read_size) {
                read_pos = 0;
                read_size = (FX_DWORD)m_FileLen;
            } else {
                read_pos = m_FileLen - read_size;
            }
        }
        if (!m_pFileAccess->ReadBlock(m_pFileBuf, read_pos, read_size))	{
            m_iStatus = PDFPARSE_ERROR_DATA_SCARCITY;
            return FALSE;
        }
        m_BufOffset = read_pos;
    }
    ch = m_pFileBuf[pos - m_BufOffset];
    return TRUE;
}
FX_BOOL CPDF_SyntaxParser::ReadBlock(FX_LPBYTE pBuf, FX_DWORD size)
{
    if (!m_pFileAccess->ReadBlock(pBuf, m_Pos + m_HeaderOffset, size)) {
        m_iStatus = PDFPARSE_ERROR_DATA_SCARCITY;
        return FALSE;
    }
    m_Pos += size;
    return TRUE;
}
#define MAX_WORD_BUFFER 256
void CPDF_SyntaxParser::GetNextWord()
{
    m_WordSize = 0;
    m_bIsNumber = TRUE;
    FX_BYTE ch;
    if (!GetNextChar(ch)) {
        return;
    }
    FX_BYTE type = _PDF_CharType[ch];
    while (1) {
        while (type == 'W') {
            if (!GetNextChar(ch)) {
                return;
            }
            type = _PDF_CharType[ch];
        }
        if (ch != '%') {
            break;
        }
        while (1) {
            if (!GetNextChar(ch)) {
                return;
            }
            if (ch == '\r' || ch == '\n') {
                break;
            }
        }
        type = _PDF_CharType[ch];
    }
    if (type == 'D') {
        m_bIsNumber = FALSE;
        m_WordBuffer[m_WordSize++] = ch;
        if (ch == '/') {
            while (1) {
                if (!GetNextChar(ch)) {
                    return;
                }
                type = _PDF_CharType[ch];
                if (type != 'R' && type != 'N') {
                    m_Pos --;
                    return;
                }
                if (m_WordSize < MAX_WORD_BUFFER) {
                    m_WordBuffer[m_WordSize++] = ch;
                }
            }
        } else if (ch == '<') {
            if (!GetNextChar(ch)) {
                return;
            }
            if (ch == '<') {
                m_WordBuffer[m_WordSize++] = ch;
            } else {
                m_Pos --;
            }
        } else if (ch == '>') {
            if (!GetNextChar(ch)) {
                return;
            }
            if (ch == '>') {
                m_WordBuffer[m_WordSize++] = ch;
            } else {
                m_Pos --;
            }
        }
        return;
    }
    while (1) {
        if (m_WordSize < MAX_WORD_BUFFER) {
            m_WordBuffer[m_WordSize++] = ch;
        }
        if (type != 'N') {
            m_bIsNumber = FALSE;
        }
        if (!GetNextChar(ch)) {
            return;
        }
        type = _PDF_CharType[ch];
        if (type == 'D' || type == 'W') {
            m_Pos --;
            break;
        }
    }
}
CFX_ByteString CPDF_SyntaxParser::ReadString()
{
    FX_BYTE ch;
    if (!GetNextChar(ch)) {
        return CFX_ByteString();
    }
    CFX_ByteTextBuf buf;
    FX_INT32 parlevel = 0;
    FX_INT32 status = 0, iEscCode = 0;
    while (1) {
        switch (status) {
            case 0:
                if (ch == ')') {
                    if (parlevel == 0) {
                        return buf.GetByteString();
                    }
                    parlevel --;
                    buf.AppendChar(')');
                } else if (ch == '(') {
                    parlevel ++;
                    buf.AppendChar('(');
                } else if (ch == '\\') {
                    status = 1;
                } else {
                    buf.AppendChar(ch);
                }
                break;
            case 1:
                if (ch >= '0' && ch <= '7') {
                    iEscCode = ch - '0';
                    status = 2;
                    break;
                }
                if (ch == 'n') {
                    buf.AppendChar('\n');
                } else if (ch == 'r') {
                    buf.AppendChar('\r');
                } else if (ch == 't') {
                    buf.AppendChar('\t');
                } else if (ch == 'b') {
                    buf.AppendChar('\b');
                } else if (ch == 'f') {
                    buf.AppendChar('\f');
                } else if (ch == '\r') {
                    status = 4;
                    break;
                } else if (ch == '\n') {
                } else {
                    buf.AppendChar(ch);
                }
                status = 0;
                break;
            case 2:
                if (ch >= '0' && ch <= '7') {
                    iEscCode = iEscCode * 8 + ch - '0';
                    status = 3;
                } else {
                    buf.AppendChar(iEscCode);
                    status = 0;
                    continue;
                }
                break;
            case 3:
                if (ch >= '0' && ch <= '7') {
                    iEscCode = iEscCode * 8 + ch - '0';
                    buf.AppendChar(iEscCode);
                    status = 0;
                } else {
                    buf.AppendChar(iEscCode);
                    status = 0;
                    continue;
                }
                break;
            case 4:
                status = 0;
                if (ch != '\n') {
                    continue;
                }
                break;
        }
        if (!GetNextChar(ch)) {
            break;
        }
    }
    GetNextChar(ch);
    return buf.GetByteString();
}
CFX_ByteString CPDF_SyntaxParser::ReadHexString()
{
    FX_BYTE ch;
    if (!GetNextChar(ch)) {
        return CFX_ByteString();
    }
    CFX_BinaryBuf buf;
    FX_BOOL bFirst = TRUE;
    FX_BYTE code = 0;
    while (1) {
        if (ch == '>') {
            break;
        }
        if (ch >= '0' && ch <= '9') {
            if (bFirst) {
                code = (ch - '0') * 16;
            } else {
                code += ch - '0';
                buf.AppendByte((FX_BYTE)code);
            }
            bFirst = !bFirst;
        } else if (ch >= 'A' && ch <= 'F') {
            if (bFirst) {
                code = (ch - 'A' + 10) * 16;
            } else {
                code += ch - 'A' + 10;
                buf.AppendByte((FX_BYTE)code);
            }
            bFirst = !bFirst;
        } else if (ch >= 'a' && ch <= 'f') {
            if (bFirst) {
                code = (ch - 'a' + 10) * 16;
            } else {
                code += ch - 'a' + 10;
                buf.AppendByte((FX_BYTE)code);
            }
            bFirst = !bFirst;
        }
        if (!GetNextChar(ch)) {
            break;
        }
    }
    if (!bFirst) {
        buf.AppendByte((FX_BYTE)code);
    }
    return buf.GetByteString();
}
void CPDF_SyntaxParser::ToNextLine()
{
    FX_BYTE ch;
    while (1) {
        if (!GetNextChar(ch)) {
            return;
        }
        if (ch == '\n') {
            return;
        }
        if (ch == '\r') {
            GetNextChar(ch);
            if (ch == '\n') {
                return;
            } else {
                m_Pos --;
                return;
            }
        }
    }
}
void CPDF_SyntaxParser::ToNextWord()
{
    FX_BYTE ch;
    if (!GetNextChar(ch)) {
        return;
    }
    FX_BYTE type = _PDF_CharType[ch];
    while (1) {
        while (type == 'W') {
            m_dwWordPos = m_Pos;
            if (!GetNextChar(ch)) {
                return;
            }
            type = _PDF_CharType[ch];
        }
        if (ch != '%') {
            break;
        }
        while (1) {
            if (!GetNextChar(ch)) {
                return;
            }
            if (ch == '\r' || ch == '\n') {
                break;
            }
        }
        type = _PDF_CharType[ch];
    }
    m_Pos --;
}
CFX_ByteString CPDF_SyntaxParser::GetNextWord(FX_BOOL& bIsNumber)
{
    GetNextWord();
    bIsNumber = m_bIsNumber;
    return CFX_ByteString((FX_LPCSTR)m_WordBuffer, m_WordSize);
}
CFX_ByteString CPDF_SyntaxParser::GetKeyword()
{
    GetNextWord();
    return CFX_ByteString((FX_LPCSTR)m_WordBuffer, m_WordSize);
}
CPDF_Object* CPDF_SyntaxParser::GetObject(CPDF_IndirectObjects* pObjList, FX_DWORD objnum, FX_DWORD gennum, FX_INT32 level, PARSE_CONTEXT* pContext, FX_BOOL bDecrypt)
{
    CFX_AutoRestorer<int> autoRestorer(&m_Level);
    if (m_Level++ > _PARSER_OBJECT_LEVLE_) {
        return NULL;
    }
    FX_FILESIZE SavedPos = m_Pos;
    FX_BOOL bTypeOnly = pContext && (pContext->m_Flags & PDFPARSE_TYPEONLY);
    FX_BOOL bIsNumber;
    CFX_ByteString word = GetNextWord(bIsNumber);
    CPDF_Object* pRet = NULL;
    if (word.GetLength() == 0) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_INVALID;
        }
        return NULL;
    }
    FX_FILESIZE wordOffset = m_Pos - word.GetLength();
    if (bIsNumber) {
        FX_FILESIZE SavedPos = m_Pos;
        CFX_ByteString nextword = GetNextWord(bIsNumber);
        if (bIsNumber) {
            CFX_ByteString nextword2 = GetNextWord(bIsNumber);
            if (nextword2 == FX_BSTRC("R")) {
                FX_DWORD objnum = FXSYS_atoi(word);
                FX_INT32 objversion = FXSYS_atoi(nextword);
                if (bTypeOnly) {
                    return (CPDF_Object*)PDFOBJ_REFERENCE;
                }
                pRet = CPDF_Reference::Create(pObjList, objnum, objversion);
                return pRet;
            } else {
                m_Pos = SavedPos;
                if (bTypeOnly) {
                    return (CPDF_Object*)PDFOBJ_NUMBER;
                }
                pRet = CPDF_Number::Create(word);
                return pRet;
            }
        } else {
            m_Pos = SavedPos;
            if (bTypeOnly) {
                return (CPDF_Object*)PDFOBJ_NUMBER;
            }
            pRet = CPDF_Number::Create(word);
            return pRet;
        }
    }
    if (word == FX_BSTRC("true") || word == FX_BSTRC("false")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_BOOLEAN;
        }
        pRet = CPDF_Boolean::Create(word == FX_BSTRC("true"));
        return pRet;
    }
    if (word == FX_BSTRC("null")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_NULL;
        }
        pRet = CPDF_Null::Create();
        return pRet;
    }
    if (word == FX_BSTRC("(")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_STRING;
        }
        FX_FILESIZE SavedPos = m_Pos - 1;
        CFX_ByteString str = ReadString();
        if (crypto_handler_ && bDecrypt && (!m_pRefStreamObjNums || (m_pRefStreamObjNums->Find(objnum) == -1))) {
            crypto_handler_->Decrypt(objnum, gennum, str);
        }
        pRet = CPDF_String::Create(str, FALSE);
        return pRet;
    }
    if (word == FX_BSTRC("<")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_STRING;
        }
        FX_FILESIZE SavedPos = m_Pos - 1;
        CFX_ByteString str = ReadHexString();
        if (crypto_handler_ && bDecrypt && (!m_pRefStreamObjNums || (m_pRefStreamObjNums->Find(objnum) == -1))) {
            crypto_handler_->Decrypt(objnum, gennum, str);
        }
        pRet = CPDF_String::Create(str, TRUE);
        return pRet;
    }
    if (word == FX_BSTRC("[")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_ARRAY;
        }
        CPDF_Array* pArray = CPDF_Array::Create();
        FX_FILESIZE firstPos = m_Pos - 1;
        while (1) {
            FX_FILESIZE SavedPos = m_Pos;
            CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, level + 1);
            if (pObj == NULL) {
                return pArray;
            }
            pArray->Add(pObj);
        }
    }
    if (word[0] == '/') {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_NAME;
        }
        pRet = CPDF_Name::Create(PDF_NameDecode(CFX_ByteStringC(m_WordBuffer + 1, m_WordSize - 1)));
        return pRet;
    }
    if (word == FX_BSTRC("<<")) {
        FX_FILESIZE saveDictOffset = m_Pos - 2;
        FX_DWORD dwDictSize = 0;
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_DICTIONARY;
        }
        if (pContext) {
            pContext->m_DictStart = SavedPos;
        }
        CPDF_Dictionary* pDict = CPDF_Dictionary::Create();
        FX_INT32 nKeys = 0;
        FX_FILESIZE dwSignValuePos = 0;
        while (1) {
            FX_BOOL bIsNumber;
            CFX_ByteString key = GetNextWord(bIsNumber);
            if (key.IsEmpty()) {
                if (pDict) {
                    pDict->Release();
                }
                return NULL;
            }
            FX_FILESIZE SavedPos = m_Pos - key.GetLength();
            if (key == FX_BSTRC(">>")) {
                dwDictSize = m_Pos - saveDictOffset;
                break;
            }
            if (key == FX_BSTRC("endobj")) {
                dwDictSize = m_Pos - 6 - saveDictOffset;
                m_Pos = SavedPos;
                break;
            }
            if (key[0] != '/') {
                continue;
            }
            nKeys ++;
            key = PDF_NameDecode(key);
            if(key.IsEmpty()) {
                continue;
            }
            if (key == FX_BSTRC("/Contents")) {
                dwSignValuePos = m_Pos;
            }
            CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, level + 1);
            if (pObj == NULL) {
                continue;
            }
            if (key == FX_BSTRC("/Rect") && pObj->GetType() == PDFOBJ_ARRAY && ((CPDF_Array*)pObj)->GetCount() < 4) {
                while (((CPDF_Array*)pObj)->GetCount() < 4) {
                    ((CPDF_Array*)pObj)->Add(CPDF_Number::Create("0"));
                }
            }
            CFX_ByteStringC keyNoSlash(((FX_LPCSTR)key) + 1, key.GetLength() - 1);
            if (key.GetLength() == 1) {
                pDict->SetAt(keyNoSlash, pObj);
            } else {
                if (nKeys < 32) {
                    pDict->SetAt(keyNoSlash, pObj);
                } else {
                    pDict->AddValue(keyNoSlash, pObj);
                }
            }
        }
        CPDF_Object* pContainer = pContext ? pContext->m_pRootContainer : NULL;
        if (IsSignatureDict(pDict, objnum, pContainer) && dwSignValuePos) {
            CFX_AutoRestorer<FX_FILESIZE> dwSavePos(&m_Pos);
            m_Pos = dwSignValuePos;
            CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, level + 1, NULL, FALSE);
            pDict->SetAt(FX_BSTRC("Contents"), pObj);
        }
        if (pContext) {
            pContext->m_DictEnd = m_Pos;
            if (pContext->m_Flags & PDFPARSE_NOSTREAM) {
                return pDict;
            }
        }
        FX_FILESIZE SavedPos = m_Pos;
        FX_BOOL bIsNumber;
        CFX_ByteString nextword = GetNextWord(bIsNumber);
        if (nextword == FX_BSTRC("stream")) {
            CPDF_Stream* pStream = ReadStream(pDict, pContext, objnum, gennum);
            if (pStream) {
                return pStream;
            }
            pDict->Release();
            return NULL;
        } else {
            m_Pos = SavedPos;
            return pDict;
        }
    }
    if (word == FX_BSTRC(">>")) {
        m_Pos = SavedPos;
        return NULL;
    }
    if (bTypeOnly) {
        return (CPDF_Object*)PDFOBJ_INVALID;
    }
    return NULL;
}
CPDF_Object* CPDF_SyntaxParser::GetObjectByStrict(CPDF_IndirectObjects* pObjList, FX_DWORD objnum, FX_DWORD gennum,
        FX_INT32 level, struct PARSE_CONTEXT* pContext)
{
    CFX_AutoRestorer<int> autoRestorer(&m_Level);
    if (m_Level++ > _PARSER_OBJECT_LEVLE_) {
        return NULL;
    }
    FX_FILESIZE SavedPos = m_Pos;
    FX_BOOL bTypeOnly = pContext && (pContext->m_Flags & PDFPARSE_TYPEONLY);
    FX_BOOL bIsNumber;
    CFX_ByteString word = GetNextWord(bIsNumber);
    if (word.GetLength() == 0) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_INVALID;
        }
        return NULL;
    }
    if (bIsNumber) {
        FX_FILESIZE SavedPos = m_Pos;
        CFX_ByteString nextword = GetNextWord(bIsNumber);
        if (bIsNumber) {
            CFX_ByteString nextword2 = GetNextWord(bIsNumber);
            if (nextword2 == FX_BSTRC("R")) {
                FX_DWORD objnum = FXSYS_atoi(word);
                FX_DWORD objversion = FXSYS_atoi(nextword);
                if (bTypeOnly) {
                    return (CPDF_Object*)PDFOBJ_REFERENCE;
                }
                return CPDF_Reference::Create(pObjList, objnum, objversion);
            } else {
                m_Pos = SavedPos;
                if (bTypeOnly) {
                    return (CPDF_Object*)PDFOBJ_NUMBER;
                }
                return CPDF_Number::Create(word);
            }
        } else {
            m_Pos = SavedPos;
            if (bTypeOnly) {
                return (CPDF_Object*)PDFOBJ_NUMBER;
            }
            return CPDF_Number::Create(word);
        }
    }
    if (word == FX_BSTRC("true") || word == FX_BSTRC("false")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_BOOLEAN;
        }
        return CPDF_Boolean::Create(word == FX_BSTRC("true"));
    }
    if (word == FX_BSTRC("null")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_NULL;
        }
        return CPDF_Null::Create();
    }
    if (word == FX_BSTRC("(")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_STRING;
        }
        CFX_ByteString str = ReadString();
        if (crypto_handler_ && (!m_pRefStreamObjNums || (m_pRefStreamObjNums->Find(objnum) == -1))) {
            crypto_handler_->Decrypt(objnum, gennum, str);
        }
        return CPDF_String::Create(str, FALSE);
    }
    if (word == FX_BSTRC("<")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_STRING;
        }
        CFX_ByteString str = ReadHexString();
        if (crypto_handler_ && (!m_pRefStreamObjNums || (m_pRefStreamObjNums->Find(objnum) == -1))) {
            crypto_handler_->Decrypt(objnum, gennum, str);
        }
        return CPDF_String::Create(str, TRUE);
    }
    if (word == FX_BSTRC("[")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_ARRAY;
        }
        CPDF_Array* pArray = CPDF_Array::Create();
        while (1) {
            CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, level + 1);
            if (pObj == NULL) {
                if (m_WordBuffer[0] == ']') {
                    return pArray;
                }
                if (pArray) {
                    pArray->Release();
                }
                return NULL;
            }
            pArray->Add(pObj);
        }
    }
    if (word[0] == '/') {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_NAME;
        }
        return CPDF_Name::Create(PDF_NameDecode(CFX_ByteStringC(m_WordBuffer + 1, m_WordSize - 1)));
    }
    if (word == FX_BSTRC("<<")) {
        if (bTypeOnly) {
            return (CPDF_Object*)PDFOBJ_DICTIONARY;
        }
        if (pContext) {
            pContext->m_DictStart = SavedPos;
        }
        CPDF_Dictionary* pDict = CPDF_Dictionary::Create();
        FX_INT32 DictNum = 0;
        while (1) {
            FX_BOOL bIsNumber;
            FX_FILESIZE SavedPos = m_Pos;
            CFX_ByteString key = GetNextWord(bIsNumber);
            if (key.IsEmpty()) {
                if (pDict) {
                    pDict->Release();
                }
                return NULL;
            }
            if (key == FX_BSTRC(">>")) {
                if(DictNum == 0) {
                    break;
                }
                DictNum --;
            }
            if (key == FX_BSTRC("<<")) {
                DictNum ++;
            }
            if(DictNum != 0) {
                continue;
            }
            if (key == FX_BSTRC("endobj")) {
                m_Pos = SavedPos;
                break;
            }
            if (key[0] != '/') {
                continue;
            }
            key = PDF_NameDecode(key);
            CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, level + 1);
            if (pObj == NULL) {
                if (pDict) {
                    pDict->Release();
                }
                FX_BYTE ch;
                while (1) {
                    if (!GetNextChar(ch)) {
                        break;
                    }
                    if (ch == 0x0A || ch == 0x0D) {
                        break;
                    }
                }
                return NULL;
            }
            if (key.GetLength() == 1) {
                pDict->SetAt(CFX_ByteStringC(((FX_LPCSTR)key) + 1, key.GetLength() - 1), pObj);
            } else {
                pDict->AddValue(CFX_ByteStringC(((FX_LPCSTR)key) + 1, key.GetLength() - 1), pObj);
            }
        }
        if (pContext) {
            pContext->m_DictEnd = m_Pos;
            if (pContext->m_Flags & PDFPARSE_NOSTREAM) {
                return pDict;
            }
        }
        FX_FILESIZE SavedPos = m_Pos;
        FX_BOOL bIsNumber;
        CFX_ByteString nextword = GetNextWord(bIsNumber);
        if (nextword == FX_BSTRC("stream")) {
            CPDF_Stream* pStream = ReadStream(pDict, pContext, objnum, gennum);
            if (pStream) {
                return pStream;
            }
            if (pDict) {
                pDict->Release();
            }
            return NULL;
        } else {
            m_Pos = SavedPos;
            return pDict;
        }
    }
    if (word == FX_BSTRC(">>")) {
        m_Pos = SavedPos;
        return NULL;
    }
    if (bTypeOnly) {
        return (CPDF_Object*)PDFOBJ_INVALID;
    }
    return NULL;
}
FX_BOOL findRef(CFX_DWordArray ref, FX_DWORD refobjnum)
{
    for (int i = 0; i < ref.GetSize(); i++) {
        if (ref.GetAt(i) == refobjnum) {
            return TRUE;
        }
    }
    return FALSE;
}
FX_FILESIZE GetStreamLength(CFX_DWordArray& dwRef, CPDF_Object* pLenObj, FX_DWORD objnum)
{
    FX_FILESIZE len = 0;
    CPDF_Object* pObj = pLenObj;
    FX_DWORD objNum = objnum;
    if (!findRef(dwRef, objNum)) {
        dwRef.Add(objNum);
    } else {
        return len;
    }
    while (pObj) {
        if (pObj->GetType() == PDFOBJ_REFERENCE) {
            FX_DWORD refobjnum = ((CPDF_Reference*)pObj)->GetRefObjNum();
            if (!findRef(dwRef, refobjnum)) {
                dwRef.Add(refobjnum);
                pObj = ((CPDF_Reference*)pObj)->GetDirect();
            } else {
                break;
            }
        } else if (pObj->GetType() == PDFOBJ_NUMBER) {
            dwRef.RemoveAll();
            len = pObj->GetInteger64();
            break;
        } else {
            dwRef.RemoveAll();
            break;
        }
    }
    return len;
}
CPDF_Stream* CPDF_SyntaxParser::ReadStream(CPDF_Dictionary* pDict, PARSE_CONTEXT* pContext,
        FX_DWORD objnum, FX_DWORD gennum)
{
    CPDF_Object* pLenObj = pDict->GetElement(FX_BSTRC("Length"));
    FX_FILESIZE len = 0;
    if (pLenObj) {
        len = GetStreamLength(m_dwRef, pLenObj, objnum);
    }
    if (len < 0) {
        len = 0;
    }
    ToNextLine();
    FX_FILESIZE StreamStartPos = m_Pos;
    if (pContext) {
        pContext->m_DataStart = m_Pos;
    }
    m_Pos += len;
    CPDF_CryptoHandler* pCryptoHandler = crypto_handler_;
    if (objnum == (FX_DWORD)m_MetadataObjnum) {
        pCryptoHandler = NULL;
    }
    if (m_pRefStreamObjNums && (m_pRefStreamObjNums->Find(objnum) != -1)) {
        pCryptoHandler = NULL;
    }
    if (1) {
        GetNextWord();
        if (m_WordSize < 9 || FXSYS_memcmp32(m_WordBuffer, "endstream", 9)) {
            m_Pos = StreamStartPos;
            FX_FILESIZE offset = FindTag(FX_BSTRC("endstream"), 0);
            if (offset >= 0) {
                FX_FILESIZE curPos = m_Pos;
                m_Pos = StreamStartPos;
                FX_FILESIZE endobjOffset = FindTag(FX_BSTRC("endobj"), 0);
                if (endobjOffset < offset && endobjOffset >= 0) {
                    offset = endobjOffset;
                } else {
                    m_Pos = curPos;
                }
                FX_BYTE byte1, byte2;
                GetCharAt(StreamStartPos + offset - 1, byte1);
                GetCharAt(StreamStartPos + offset - 2, byte2);
                len = offset;
                if (pCryptoHandler != NULL) {
                    if (byte1 == 0x0a && byte2 == 0x0d) {
                        len -= 2;
                    } else if (byte1 == 0x0a || byte1 == 0x0d) {
                        len --;
                    }
                }
                if (len < 0) {
                    return NULL;
                }
                pDict->SetAtInteger64(FX_BSTRC("Length"), len);
            } else {
                m_Pos = StreamStartPos;
                FX_FILESIZE endobjOffset = FindTag(FX_BSTRC("endobj"), 0);
                if (endobjOffset < 0) {
                    return NULL;
                }
                if(len <= 0) {
                    len = endobjOffset;
                } else {
                    len = FX_MIN(len, endobjOffset);
                }
            }
        }
    }
    m_Pos = StreamStartPos;
    CPDF_Stream* pStream;
#if defined(_FPDFAPI_MINI_) && !defined(_FXCORE_FEATURE_ALL_)
    pStream = FX_NEW CPDF_Stream(m_pFileAccess, pCryptoHandler, m_HeaderOffset + m_Pos, len, pDict, gennum);
    m_Pos += len;
#else
    FX_LPBYTE pData = NULL;
    if (len) {
        pData = FX_Alloc(FX_BYTE, len);
        if (!pData) {
            return NULL;
        }
        ReadBlock(pData, len);
        if (pCryptoHandler) {
            CFX_BinaryBuf dest_buf;
            dest_buf.EstimateSize(pCryptoHandler->DecryptGetSize(len));
            FX_LPVOID context = pCryptoHandler->DecryptStart(objnum, gennum);
            pCryptoHandler->DecryptStream(context, pData, len, dest_buf);
            pCryptoHandler->DecryptFinish(context, dest_buf);
            FX_Free(pData);
            pData = dest_buf.GetBuffer();
            len = dest_buf.GetSize();
            dest_buf.DetachBuffer();
        }
    }
    pStream = FX_NEW CPDF_Stream(pData, len, pDict);
    m_TotalMemoryStreamSize += len;
#endif
    if (pContext) {
        pContext->m_DataEnd = pContext->m_DataStart + len;
    }
    StreamStartPos = m_Pos;
    GetNextWord();
    if (m_WordSize == 6 && 0 == FXSYS_memcmp32(m_WordBuffer, "endobj", 6)) {
        m_Pos = StreamStartPos;
    }
    return pStream;
}
void CPDF_SyntaxParser::InitParser(IFX_FileRead* pFileAccess, FX_DWORD HeaderOffset)
{
    if (m_pFileBuf) {
        FX_Free(m_pFileBuf);
        m_pFileBuf = NULL;
    }
    m_pFileBuf = FX_Alloc(FX_BYTE, m_BufSize);
    m_HeaderOffset = HeaderOffset;
    m_FileLen = pFileAccess->GetSize();
    m_Pos = 0;
    m_pFileAccess = pFileAccess;
    m_BufOffset = 0;
    pFileAccess->ReadBlock(m_pFileBuf, 0, (size_t)((FX_FILESIZE)m_BufSize > m_FileLen ? m_FileLen : m_BufSize));
}
FX_INT32 CPDF_SyntaxParser::GetDirectNum()
{
    GetNextWord();
    if (!m_bIsNumber) {
        return 0;
    }
    m_WordBuffer[m_WordSize] = 0;
    return FXSYS_atoi((FX_LPCSTR)m_WordBuffer);
}
FX_INT64 CPDF_SyntaxParser::GetDirectNum64()
{
    GetNextWord();
    if (!m_bIsNumber) {
        return 0;
    }
    m_WordBuffer[m_WordSize] = 0;
    return FXSYS_atoi64((FX_LPCSTR)m_WordBuffer);
}
FX_BOOL CPDF_SyntaxParser::IsWholeWord(FX_FILESIZE startpos, FX_FILESIZE limit, FX_LPCBYTE tag, FX_DWORD taglen)
{
    FX_BYTE type = _PDF_CharType[tag[0]];
    FX_BOOL bCheckLeft = type != 'D' && type != 'W';
    type = _PDF_CharType[tag[taglen - 1]];
    FX_BOOL bCheckRight = type != 'D' && type != 'W';
    FX_BYTE ch;
    if (bCheckRight && startpos + (FX_INT32)taglen <= limit && GetCharAt(startpos + (FX_INT32)taglen, ch)) {
        FX_BYTE type = _PDF_CharType[ch];
        if (type == 'N' || type == 'R') {
            return FALSE;
        }
    }
    if (bCheckLeft && startpos > 0 && GetCharAt(startpos - 1, ch)) {
        FX_BYTE type = _PDF_CharType[ch];
        if (type == 'N' || type == 'R') {
            return FALSE;
        }
    }
    return TRUE;
}
FX_BOOL CPDF_SyntaxParser::SearchWord(FX_BSTR tag, FX_BOOL bWholeWord, FX_BOOL bForward, FX_FILESIZE limit)
{
    FX_INT32 taglen = tag.GetLength();
    if (taglen == 0) {
        return FALSE;
    }
    FX_FILESIZE pos = m_Pos;
    FX_INT32 offset = 0;
    if (!bForward) {
        offset = taglen - 1;
    }
    FX_LPCBYTE tag_data = tag;
    FX_BYTE byte;
    while (1) {
        if (bForward) {
            if (limit) {
                if (pos >= m_Pos + limit) {
                    return FALSE;
                }
            }
            if (!GetCharAt(pos, byte)) {
                return FALSE;
            }
        } else {
            if (limit) {
                if (pos <= m_Pos - limit) {
                    return FALSE;
                }
            }
            if (!GetCharAtBackward(pos, byte)) {
                return FALSE;
            }
        }
        if (byte == tag_data[offset]) {
            if (bForward) {
                offset ++;
                if (offset < taglen) {
                    pos ++;
                    continue;
                }
            } else {
                offset --;
                if (offset >= 0) {
                    pos --;
                    continue;
                }
            }
            FX_FILESIZE startpos = bForward ? pos - taglen + 1 : pos;
            if (!bWholeWord || IsWholeWord(startpos, limit, tag, taglen)) {
                m_Pos = startpos;
                return TRUE;
            }
        }
        if (bForward) {
            offset = byte == tag_data[0] ? 1 : 0;
            pos ++;
        } else {
            offset = byte == tag_data[taglen - 1] ? taglen - 2 : taglen - 1;
            pos --;
        }
        if (pos < 0) {
            return FALSE;
        }
    }
    return FALSE;
}
struct _SearchTagRecord {
    FX_LPCBYTE	m_pTag;
    FX_DWORD	m_Len;
    FX_DWORD	m_Offset;
};
FX_INT32 CPDF_SyntaxParser::SearchMultiWord(FX_BSTR tags, FX_BOOL bWholeWord, FX_FILESIZE limit)
{
    if (tags.GetLength() == 0) {
        return -1;
    }
    FX_INT32 ntags = 1, i;
    for (i = 0; i < tags.GetLength(); i ++)
        if (tags[i] == 0) {
            ntags ++;
        }
    _SearchTagRecord* pPatterns = FX_Alloc(_SearchTagRecord, ntags);
    FX_DWORD start = 0, itag = 0, max_len = 0;
    for (i = 0; i <= tags.GetLength(); i ++) {
        if (tags[i] == 0) {
            FX_DWORD len = i - start;
            if (len > max_len) {
                max_len = len;
            }
            pPatterns[itag].m_pTag = tags.GetPtr() + start;
            pPatterns[itag].m_Len = len;
            pPatterns[itag].m_Offset = 0;
            start = i + 1;
            itag ++;
        }
    }
    FX_FILESIZE pos = m_Pos;
    FX_BYTE byte;
    GetCharAt(pos++, byte);
    FX_INT32 found = -1;
    while (1) {
        for (i = 0; i < ntags; i ++) {
            if (pPatterns[i].m_pTag[pPatterns[i].m_Offset] == byte) {
                pPatterns[i].m_Offset ++;
                if (pPatterns[i].m_Offset == pPatterns[i].m_Len) {
                    if (!bWholeWord || IsWholeWord(pos - pPatterns[i].m_Len, limit, pPatterns[i].m_pTag, pPatterns[i].m_Len)) {
                        found = i;
                        goto end;
                    } else {
                        if (pPatterns[i].m_pTag[0] == byte) {
                            pPatterns[i].m_Offset = 1;
                        } else {
                            pPatterns[i].m_Offset = 0;
                        }
                    }
                }
            } else {
                if (pPatterns[i].m_pTag[0] == byte) {
                    pPatterns[i].m_Offset = 1;
                } else {
                    pPatterns[i].m_Offset = 0;
                }
            }
        }
        if (limit && pos >= m_Pos + limit) {
            goto end;
        }
        if (!GetCharAt(pos, byte)) {
            goto end;
        }
        pos ++;
    }
end:
    FX_Free(pPatterns);
    return found;
}
FX_FILESIZE CPDF_SyntaxParser::FindTag(FX_BSTR tag, FX_FILESIZE limit)
{
    FX_INT32 taglen = tag.GetLength();
    if (taglen == 0) {
        return -1;
    }
    FX_INT32 match = 0;
    limit += m_Pos;
    FX_FILESIZE startpos = m_Pos;
    while (1) {
        FX_BYTE ch;
        if (!GetNextChar(ch)) {
            return -1;
        }
        if (ch == tag[match]) {
            match ++;
            if (match == taglen) {
                FX_FILESIZE SavedPos = m_Pos;
                FX_BYTE tagNextCh;
                if (!GetNextChar(tagNextCh)) {
                    return -1;
                }
                m_Pos = SavedPos;
                if (_PDF_CharType[tagNextCh] != 'W') {
                    match = 0;
                    continue;
                }
                return m_Pos - startpos - taglen;
            }
        } else {
            match = ch == tag[0] ? 1 : 0;
        }
        if (limit && m_Pos == limit) {
            return -1;
        }
    }
    return -1;
}
void CPDF_SyntaxParser::GetBinary(FX_BYTE* buffer, FX_DWORD size)
{
    FX_DWORD offset = 0;
    FX_BYTE ch;
    while (1) {
        if (!GetNextChar(ch)) {
            return;
        }
        buffer[offset++] = ch;
        if (offset == size) {
            break;
        }
    }
}
