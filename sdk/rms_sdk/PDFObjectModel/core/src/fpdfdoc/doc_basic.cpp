/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include "../../include/fpdfdoc/fpdf_doc.h"
#include "../../include/fdrm/fx_crypt.h"
const int nMaxRecursion = 32;
CPDF_NameTree::CPDF_NameTree(CPDF_Dictionary* pParent, FX_BSTR sCategory)
    : m_pParent(pParent)
    , m_pRoot(NULL)
    , m_bsCategory(sCategory)
{
    if (pParent == NULL) {
        return;
    }
    m_pRoot = pParent->GetDict(sCategory);
}
static CPDF_Object* SearchNameNode(CPDF_Dictionary* pNode, const CFX_ByteString& csPDFDoc, const CFX_ByteString& csUnicode,
                                   int& nIndex, CPDF_Array** ppFind, CFX_ArrayTemplate<CPDF_Dictionary*>* pKidsArray = NULL, int nLevel = 0)
{
    if (nLevel > nMaxRecursion) {
        return NULL;
    }
    if (pKidsArray) {
        pKidsArray->Add(pNode);
    }
    CPDF_Array* pLimits = pNode->GetArray(FX_BSTRC("Limits"));
    if (pLimits != NULL) {
        CFX_ByteString csLeft = pLimits->GetString(0);
        CFX_ByteString csRight = pLimits->GetString(1);
        if (csLeft.Compare(csRight) > 0) {
            CFX_ByteString csTmp = csRight;
            csRight = csLeft;
            csLeft = csTmp;
        }
        if (csPDFDoc.Compare(csLeft) < 0 || csPDFDoc.Compare(csRight) > 0) {
            if(csUnicode.Compare(csLeft) < 0 || csUnicode.Compare(csRight) > 0) {
                return NULL;
            }
        }
    }
    CPDF_Array* pNames = pNode->GetArray(FX_BSTRC("Names"));
    if (pNames) {
        FX_DWORD dwCount = pNames->GetCount() / 2;
        for (FX_DWORD i = 0; i < dwCount; i ++) {
            CFX_ByteString csValue = pNames->GetString(i * 2);
            FX_INT32 iCompare = csValue.Compare(csPDFDoc);
            if (iCompare <= 0) {
                if (ppFind != NULL) {
                    *ppFind = pNames;
                }
                if (iCompare < 0) {
                    continue;
                }
            } else {
                FX_INT32 iCompare2 = csValue.Compare(csUnicode);
                if (iCompare2 <= 0) {
                    if (ppFind != NULL) {
                        *ppFind = pNames;
                    }
                    if (iCompare2 < 0) {
                        continue;
                    }
                } else {
                    break;
                }
            }
            nIndex += i;
            return pNames->GetElementValue(i * 2 + 1);
        }
        nIndex += dwCount;
        return NULL;
    }
    CPDF_Array* pKids = pNode->GetArray(FX_BSTRC("Kids"));
    if (pKids == NULL) {
        return NULL;
    }
    for (FX_DWORD i = 0; i < pKids->GetCount(); i ++) {
        CPDF_Dictionary* pKid = pKids->GetDict(i);
        if (pKid == NULL || pNode == pKid) {
            continue;
        }
        CPDF_Object* pFound = SearchNameNode(pKid, csPDFDoc, csUnicode, nIndex, ppFind, pKidsArray, nLevel + 1);
        if (pFound) {
            return pFound;
        }
    }
    return NULL;
}
static int CountNames(CPDF_Dictionary* pNode, int nLevel = 0)
{
    if (nLevel > nMaxRecursion) {
        return 0;
    }
    CPDF_Array* pNames = pNode->GetArray(FX_BSTRC("Names"));
    if (pNames) {
        return pNames->GetCount() / 2;
    }
    CPDF_Array* pKids = pNode->GetArray(FX_BSTRC("Kids"));
    if (pKids == NULL) {
        return 0;
    }
    int nCount = 0;
    for (FX_DWORD i = 0; i < pKids->GetCount(); i ++) {
        CPDF_Dictionary* pKid = pKids->GetDict(i);
        if (pKid == NULL || pNode == pKid) {
            continue;
        }
        nCount += CountNames(pKid, nLevel + 1);
    }
    return nCount;
}
static FX_BOOL AddNameNode(CPDF_Document* pDoc, CPDF_Dictionary* pNode, const CFX_ByteString& csPDFDoc, const CFX_ByteString& csUnicode,
                           CPDF_Object* pValue, int& nIndex, FX_BOOL& bNew, int nLevel = 0)
{
    if (nLevel > nMaxRecursion) {
        return TRUE;
    }
    if (pNode && !pNode->KeyExist(FX_BSTRC("Names")) && !pNode->KeyExist(FX_BSTRC("Kids"))) {
        nIndex = 0;
        bNew = TRUE;
        CPDF_Array* pArray = CPDF_Array::Create();
        if (pArray == NULL) {
            return TRUE;
        }
        pNode->SetAt(FX_BSTRC("Names"), pArray);
        pArray->Add(CPDF_String::Create(csPDFDoc));
        pArray->Add(pValue, pDoc);
        return TRUE;
    }
    FX_BOOL bOutRange = FALSE;
    CPDF_Array* pLimits = pNode->GetArray(FX_BSTRC("Limits"));
    if (pLimits) {
        CFX_ByteString csLeft = pLimits->GetString(0);
        CFX_ByteString csRight = pLimits->GetString(1);
        if (csPDFDoc.Compare(csRight) > 0 && csUnicode.Compare(csRight) > 0) {
            bOutRange = TRUE;
        }
    }
    CPDF_Array* pNames = pNode->GetArray(FX_BSTRC("Names"));
    if (pNames) {
        FX_DWORD i = 0;
        FX_DWORD dwCount = pNames->GetCount() / 2;
        if (bOutRange) {
            nIndex += dwCount;
            return FALSE;
        }
        for (; i < dwCount; i ++) {
            CFX_ByteString csValue = pNames->GetString(i * 2);
            FX_INT32 iCompare = csValue.Compare(csPDFDoc);
            FX_INT32 iCompare2 = csValue.Compare(csUnicode);
            if (iCompare == 0 || iCompare2 == 0 ) {
                pNames->SetAt(i * 2 + 1, pValue, pDoc);
                nIndex += i;
                bNew = FALSE;
                break;
            }
            if (iCompare > 0) {
                pNames->InsertAt(i * 2, CPDF_String::Create(csPDFDoc));
                pNames->InsertAt(i * 2 + 1, pValue, pDoc);
                nIndex += i;
                bNew = TRUE;
                break;
            }
        }
        if (i == 0 && pLimits) {
            pLimits->SetAt(0, CPDF_String::Create(csPDFDoc));
        }
        if (i == dwCount) {
            nIndex += i;
            pNames->Add(CPDF_String::Create(csPDFDoc));
            pNames->Add(pValue, pDoc);
            if (pLimits) {
                pLimits->SetAt(1, CPDF_String::Create(csPDFDoc));
            }
        }
        return TRUE;
    }
    CPDF_Array* pKids = pNode->GetArray(FX_BSTRC("Kids"));
    if (!pKids) {
        return TRUE;
    }
    FX_BOOL b = FALSE;
    for (FX_DWORD i = 0; i < pKids->GetCount(); i ++) {
        CPDF_Dictionary* pKid = pKids->GetDict(i);
        if (!pKid || pNode == pKid) {
            continue;
        }
        b = AddNameNode(pDoc, pKid, csPDFDoc, csUnicode, pValue, nIndex, bNew, nLevel + 1);
        if (b) {
            if ( bNew && pLimits) {
                CFX_ByteString csLeft = pLimits->GetString(0);
                CFX_ByteString csRight = pLimits->GetString(1);
                if (csPDFDoc.Compare(csLeft) < 0) {
                    pLimits->SetAt(0, CPDF_String::Create(csPDFDoc));
                    return TRUE;
                }
                if (csPDFDoc.Compare(csRight) > 0) {
                    pLimits->SetAt(1, CPDF_String::Create(csPDFDoc));
                    return TRUE;
                }
            }
            break;
        }
    }
    return b;
}
static FX_BOOL InsertNameNodeToLast(CPDF_Document* pDoc, CPDF_Dictionary* pNode, const CFX_ByteString& csPDFDoc, CPDF_Object* pValue, int nLevel = 0)
{
    if (nLevel > nMaxRecursion) {
        return TRUE;
    }
    CPDF_Array* pLimits = pNode->GetArray(FX_BSTRC("Limits"));
    CPDF_Array* pNames = pNode->GetArray(FX_BSTRC("Names"));
    if (pNames) {
        pNames->Add(CPDF_String::Create(csPDFDoc));
        pNames->Add(pValue, pDoc);
        if (pLimits) {
            CFX_ByteString csRight = pLimits->GetString(1);
            if (csPDFDoc.Compare(csRight) > 0) {
                pLimits->SetAt(1, CPDF_String::Create(csPDFDoc));
            }
        }
        return TRUE;
    }
    CPDF_Array* pKids = pNode->GetArray(FX_BSTRC("Kids"));
    if (!pKids) {
        return TRUE;
    }
    FX_INT32 nIndex = pKids->GetCount() - 1;
    CPDF_Dictionary* pKid = NULL;
    while (!pKid && nIndex >= 0) {
        pKid = pKids->GetDict(nIndex--);
    }
    if (!pKid) {
        return TRUE;
    }
    FX_BOOL b = InsertNameNodeToLast(pDoc, pKid, csPDFDoc, pValue, nLevel + 1);
    if (b && pLimits) {
        CFX_ByteString csRight = pLimits->GetString(1);
        if (csPDFDoc.Compare(csRight) > 0) {
            pLimits->SetAt(1, CPDF_String::Create(csPDFDoc));
        }
    }
    return b;
}
int CPDF_NameTree::GetCount() const
{
    if (m_pRoot == NULL) {
        return 0;
    }
    return ::CountNames(m_pRoot);
}
static void FPDF_NameTree_GetAlternateName(const CFX_ByteString& csName, CFX_ByteString& csPDFDoc, CFX_ByteString& csUnicode)
{
    if (csName.GetLength() >= 2 && ((csName[0] == 0xfe && csName[1] == 0xff) || (csName[0] == 0xff && csName[1] == 0xfe))) {
        csPDFDoc = csName;
        csUnicode = csName;
    } else {
        csPDFDoc = csName;
        CFX_WideString wsName = PDF_DecodeText(csName);
        csUnicode.Empty();
        csUnicode.Reserve(csName.GetLength() * 2 + 4);
        FX_BYTE cTemp1 = 0xfe;
        csUnicode += static_cast<char>(cTemp1);
        FX_BYTE cTemp2 = 0xff;
        csUnicode += static_cast<char>(cTemp2);
        for (FX_INT32 i = 0, c = wsName.GetLength(); i < c; i++) {
            csUnicode += (wsName[i] >> 8) & 0xFF;
            csUnicode += (wsName[i]) & 0xFF;
        }
    }
}
int CPDF_NameTree::SetValue(CPDF_Document* pDoc, const CFX_ByteString& csName, CPDF_Object* pValue)
{
    ASSERT(!csName.IsEmpty());
    if (pDoc == NULL  || m_pParent == NULL || pValue == NULL || m_bsCategory.IsEmpty()) {
        return -1;
    }
    if (m_pRoot == NULL) {
        m_pRoot = CPDF_Dictionary::Create();
        if (m_pRoot == NULL) {
            return -1;
        }
        FX_DWORD dwObjNum = pDoc->AddIndirectObject(m_pRoot);
        m_pParent->SetAtReference(m_bsCategory, pDoc, dwObjNum);
    }
    CPDF_Array *pFind = NULL;
    int index = 0;
    CFX_ByteString csPDFDoc, csUnicode;
    FPDF_NameTree_GetAlternateName(csName, csPDFDoc, csUnicode);
    if (SearchNameNode(m_pRoot, csPDFDoc, csUnicode, index, &pFind)) {
        if (pFind == NULL) {
            return index;
        }
        FX_DWORD dwCount = pFind->GetCount() / 2;
        for (FX_DWORD i = 0; i < dwCount; i ++) {
            CFX_ByteString csValue = pFind->GetString(i * 2);
            FX_INT32 iCompare = csValue.Compare(csPDFDoc);
            FX_INT32 iUCompare = csValue.Compare(csUnicode);
            if (iCompare == 0 || iUCompare == 0) {
                pFind->SetAt(i * 2 + 1, pValue);
                return index;
            }
        }
    } else {
        index = 0;
        FX_BOOL bNew = FALSE;
        FX_BOOL bInsert = AddNameNode(pDoc, m_pRoot, csPDFDoc, csUnicode, pValue, index, bNew);
        if (!bInsert) {
            bInsert = InsertNameNodeToLast(pDoc, m_pRoot, csName, pValue);
        }
        return bInsert ? index : -1;
    }
    return index;
}
#if _FXM_PLATFORM_  == _FXM_PLATFORM_APPLE_ || _FXM_PLATFORM_  == _FXM_PLATFORM_WINDOWS_
static CFX_WideString ChangeSlashToPlatform(FX_LPCWSTR str)
{
    CFX_WideString result;
    while (*str) {
        if (*str == '/') {
#if _FXM_PLATFORM_  == _FXM_PLATFORM_APPLE_
            result += ':';
#elif _FXM_PLATFORM_  == _FXM_PLATFORM_WINDOWS_
            result += '\\';
#else
            result += *str;
#endif
        } else {
            result += *str;
        }
        str++;
    }
    return result;
}
#endif
static CFX_WideString FILESPEC_DecodeFileName(FX_WSTR filepath)
{
    if (filepath.GetLength() <= 1) {
        return CFX_WideString();
    }
#if _FXM_PLATFORM_  == _FXM_PLATFORM_APPLE_
    if (filepath.Left(sizeof("/Mac") - 1) == CFX_WideStringC(L"/Mac")) {
        return ChangeSlashToPlatform(filepath.GetPtr() + 1);
    }
    return ChangeSlashToPlatform(filepath.GetPtr());
#elif _FXM_PLATFORM_  == _FXM_PLATFORM_WINDOWS_
    if (filepath.GetAt(0) != '/') {
        return ChangeSlashToPlatform(filepath.GetPtr());
    }
    if (filepath.GetAt(1) == '/') {
        return ChangeSlashToPlatform(filepath.GetPtr() + 1);
    }
    if (filepath.GetAt(2) == '/') {
        CFX_WideString result;
        result += filepath.GetAt(1);
        result += ':';
        result += ChangeSlashToPlatform(filepath.GetPtr() + 2);
        return result;
    }
    CFX_WideString result;
    result += '\\';
    result += ChangeSlashToPlatform(filepath.GetPtr());
    return result;
#else
    return filepath;
#endif
}
FX_BOOL CPDF_FileSpec::GetFileName(CFX_WideString &csFileName) const
{
    if (m_pObj == NULL) {
        return FALSE;
    }
    if (m_pObj->GetType() == PDFOBJ_DICTIONARY) {
        CPDF_Dictionary* pDict = (CPDF_Dictionary*)m_pObj;
        csFileName = pDict->GetUnicodeText(FX_BSTRC("UF"));
        if (csFileName.IsEmpty()) {
            csFileName = CFX_WideString::FromLocal(pDict->GetString(FX_BSTRC("F")));
        }
        if (pDict->GetString(FX_BSTRC("FS")) == FX_BSTRC("URL")) {
            return TRUE;
        }
        if (csFileName.IsEmpty()) {
            if (pDict->KeyExist(FX_BSTRC("DOS"))) {
                csFileName = CFX_WideString::FromLocal(pDict->GetString(FX_BSTRC("DOS")));
            } else if (pDict->KeyExist(FX_BSTRC("Mac"))) {
                csFileName = CFX_WideString::FromLocal(pDict->GetString(FX_BSTRC("Mac")));
            } else if (pDict->KeyExist(FX_BSTRC("Unix"))) {
                csFileName = CFX_WideString::FromLocal(pDict->GetString(FX_BSTRC("Unix")));
            } else {
                return FALSE;
            }
        }
    } else {
        csFileName = CFX_WideString::FromLocal(m_pObj->GetString());
    }
    csFileName = FILESPEC_DecodeFileName(csFileName);
    return TRUE;
}
CPDF_FileSpec::CPDF_FileSpec()
{
    m_pObj = CPDF_Dictionary::Create();
    if (m_pObj != NULL) {
        ((CPDF_Dictionary*)m_pObj)->SetAtName(FX_BSTRC("Type"), FX_BSTRC("Filespec"));
    }
}
FX_BOOL CPDF_FileSpec::IsURL() const
{
    if (m_pObj == NULL) {
        return FALSE;
    }
    if (m_pObj->GetType() != PDFOBJ_DICTIONARY) {
        return FALSE;
    }
    return ((CPDF_Dictionary*)m_pObj)->GetString(FX_BSTRC("FS")) == FX_BSTRC("URL");
}
#if _FXM_PLATFORM_  == _FXM_PLATFORM_APPLE_ || _FXM_PLATFORM_  == _FXM_PLATFORM_WINDOWS_
static CFX_WideString ChangeSlashToPDF(FX_LPCWSTR str)
{
    CFX_WideString result;
    while (*str) {
        if (*str == '\\' || *str == ':') {
            result += '/';
        } else {
            result += *str;
        }
        str++;
    }
    return result;
}
#endif
CFX_WideString FILESPEC_EncodeFileName(FX_WSTR filepath)
{
    if (filepath.GetLength() <= 1) {
        return CFX_WideString();
    }
#if _FXM_PLATFORM_  == _FXM_PLATFORM_WINDOWS_
    if (filepath.GetAt(1) == ':') {
        CFX_WideString result;
        result = '/';
        result += filepath.GetAt(0);
        if (filepath.GetAt(2) != '\\') {
            result += '/';
        }
        result += ChangeSlashToPDF(filepath.GetPtr() + 2);
        return result;
    }
    if (filepath.GetAt(0) == '\\' && filepath.GetAt(1) == '\\') {
        return ChangeSlashToPDF(filepath.GetPtr() + 1);
    }
    if (filepath.GetAt(0) == '\\') {
        CFX_WideString result;
        result = '/';
        result += ChangeSlashToPDF(filepath.GetPtr());
        return result;
    }
    return ChangeSlashToPDF(filepath.GetPtr());
#elif _FXM_PLATFORM_  == _FXM_PLATFORM_APPLE_
    if (filepath.Left(sizeof("Mac") - 1) == FX_WSTRC(L"Mac")) {
        CFX_WideString result;
        result = '/';
        result += ChangeSlashToPDF(filepath.GetPtr());
        return result;
    }
    return ChangeSlashToPDF(filepath.GetPtr());
#else
    return filepath;
#endif
}
CPDF_Stream* CPDF_FileSpec::GetFileStream() const
{
    if (m_pObj == NULL) {
        return NULL;
    }
    FX_INT32 iType = m_pObj->GetType();
    if (iType == PDFOBJ_STREAM) {
        return (CPDF_Stream*)m_pObj;
    } else if (iType == PDFOBJ_DICTIONARY) {
        CPDF_Dictionary *pEF = ((CPDF_Dictionary*)m_pObj)->GetDict(FX_BSTRC("EF"));
        if (pEF == NULL) {
            return NULL;
        }
        return pEF->GetStream(FX_BSTRC("F"));
    }
    return NULL;
}
static void FPDFDOC_FILESPEC_SetFileName(CPDF_Object *pObj, FX_WSTR wsFileName, FX_BOOL bURL)
{
    ASSERT(pObj != NULL);
    CFX_WideString wsStr;
    if (bURL) {
        wsStr = wsFileName;
    } else {
        wsStr = FILESPEC_EncodeFileName(wsFileName);
    }
    FX_INT32 iType = pObj->GetType();
    if (iType == PDFOBJ_STRING) {
        pObj->SetString(CFX_ByteString::FromUnicode(wsStr));
    } else if (iType == PDFOBJ_DICTIONARY) {
        CPDF_Dictionary* pDict = (CPDF_Dictionary*)pObj;
        pDict->SetAtString(FX_BSTRC("F"), CFX_ByteString::FromUnicode(wsStr));
        pDict->SetAtString(FX_BSTRC("UF"), PDF_EncodeText(wsStr));
    }
}
void CPDF_FileSpec::SetFileName(FX_WSTR wsFileName, FX_BOOL bURL)
{
    ASSERT(m_pObj != NULL);
    if (m_pObj->GetType() == PDFOBJ_DICTIONARY && bURL) {
        ((CPDF_Dictionary*)m_pObj)->SetAtName(FX_BSTRC("FS"), "URL");
    }
    FPDFDOC_FILESPEC_SetFileName(m_pObj, wsFileName, bURL);
}
void CPDF_FileSpec::SetEmbeddedFile(CPDF_IndirectObjects *pDocument, IFX_FileRead *pFile, FX_WSTR file_path)
{
    ASSERT(m_pObj != NULL && pDocument != NULL && pFile != NULL);
    FX_INT32 iLength = file_path.GetLength();
    FX_INT32 iPos = iLength - 1;
    while (iPos > -1) {
        FX_WCHAR ch = file_path.GetAt(iPos);
        if (ch == '\\' || ch == '/' || ch == ':') {
            iPos ++;
            break;
        }
        iPos --;
    }
    CFX_WideString csFileName = file_path.Right(iLength - iPos);
    csFileName.TrimRight();
    csFileName.TrimLeft();
    if (csFileName.IsEmpty()) {
        csFileName = L"Embedded File";
    }
    FPDFDOC_FILESPEC_SetFileName(m_pObj, csFileName, FALSE);
    CPDF_Stream *pStream = FX_NEW CPDF_Stream(pFile, NULL, 0, (FX_DWORD)pFile->GetSize(), FX_NEW CPDF_Dictionary, 0);
    if (pStream == NULL) {
        return;
    }
    pDocument->AddIndirectObject(pStream);
    FX_INT32 iType = m_pObj->GetType();
    if (iType == PDFOBJ_STREAM) {
        m_pObj = pStream;
    } else if (iType == PDFOBJ_DICTIONARY) {
        CPDF_Dictionary *pEF = ((CPDF_Dictionary*)m_pObj)->GetDict(FX_BSTRC("EF"));
        if (pEF == NULL) {
            pEF = CPDF_Dictionary::Create();
            if (pEF == NULL) {
                return;
            }
            ((CPDF_Dictionary*)m_pObj)->SetAt(FX_BSTRC("EF"), pEF);
        }
        pEF->SetAtReference(FX_BSTRC("F"), pDocument, pStream);
    }
}
