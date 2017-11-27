/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#include "../../include/fpdfapi/fpdf_serial.h"
#include "../../include/fpdfdoc/fpdf_doc.h"
#include "../../include/fdrm/fx_crypt.h"
#include "editint.h"
namespace
{

const char kFoxitWrapperName[] = "FoxitWrapper";

}
#define PDF_OBJECTSTREAM_MAXLENGTH	(256 * 1024)
#define PDF_XREFSTREAM_MAXSIZE		10000
extern void FlateEncode(const FX_BYTE* src_buf, FX_DWORD src_data, FX_LPBYTE& dest_buf, FX_DWORD& dest_size);
extern void FlateEncode(FX_LPCBYTE src_buf, FX_DWORD src_size, int predictor, int Colors, int BitsPerComponent, int Columns,
                        FX_LPBYTE& dest_buf, FX_DWORD& dest_size);
extern FX_BOOL IsSignatureDict(CPDF_Dictionary* pDict, FX_DWORD objnum, CPDF_Object* pCurContainer);
FX_INT32 PDF_CreatorAppendObject(CPDF_Creator* pCreator, const CPDF_Object* pObj, CFX_FileBufferArchive *pFile, FX_FILESIZE& offset, const CFX_WordArray* pObjVersion)
{
    FX_INT32 len = 0;
    if (pObj == NULL) {
        if (pFile->AppendString(FX_BSTRC(" null")) < 0) {
            return -1;
        }
        offset += 5;
        return 1;
    }
    switch (pObj->GetType()) {
        case PDFOBJ_NULL:
            if (pFile->AppendString(FX_BSTRC(" null")) < 0) {
                return -1;
            }
            offset += 5;
            break;
        case PDFOBJ_BOOLEAN:
        case PDFOBJ_NUMBER:
            if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                return -1;
            }
            if ((len = pFile->AppendString(pObj->GetString())) < 0) {
                return -1;
            }
            offset += len + 1;
            break;
        case PDFOBJ_STRING: {
                CFX_ByteString str = pObj->GetString();
                FX_BOOL bHex = ((CPDF_String*)pObj)->IsHex();
                if ((len = pFile->AppendString(PDF_EncodeString(str, bHex))) < 0) {
                    return -1;
                }
                offset += len;
                break;
            }
        case PDFOBJ_NAME: {
                if (pFile->AppendString(FX_BSTRC("/")) < 0) {
                    return -1;
                }
                CFX_ByteString str = pObj->GetString();
                if ((len = pFile->AppendString(PDF_NameEncode(str))) < 0) {
                    return -1;
                }
                offset += len + 1;
                break;
            }
        case PDFOBJ_REFERENCE: {
                if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                CPDF_Reference* p = (CPDF_Reference*)pObj;
                FX_DWORD dwRefObjNum = p->GetRefObjNum();
                if ((len = pFile->AppendDWord(dwRefObjNum)) < 0) {
                    return -1;
                }
                if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                FX_INT32 wRefObjVersion = p->GetRefObjVersion();
                if ((len += pFile->AppendDWord(wRefObjVersion)) < 0) {
                    return -1;
                }
                if (pFile->AppendString(FX_BSTRC(" R ")) < 0) {
                    return -1;
                }
                offset += len + 5;
                break;
            }
        case PDFOBJ_ARRAY: {
                if (pFile->AppendString(FX_BSTRC("[")) < 0) {
                    return -1;
                }
                offset += 1;
                CPDF_Array* p = (CPDF_Array*)pObj;
                for (FX_DWORD i = 0; i < p->GetCount(); i ++) {
                    CPDF_Object* pElement = p->GetElement(i);
                    if (pElement->GetObjNum()) {
                        if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_DWORD dwElementObjNum = pElement->GetObjNum();
                        if ((len = pFile->AppendDWord(dwElementObjNum)) < 0) {
                            return -1;
                        }
                        if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_WORD wElementObjVersion = 0;
                        if (pObjVersion && pObjVersion->GetSize() > static_cast<int>(dwElementObjNum)) {
                            wElementObjVersion = (*pObjVersion)[dwElementObjNum];
                        }
                        if ((len += pFile->AppendDWord(wElementObjVersion)) < 0) {
                            return -1;
                        }
                        if (pFile->AppendString(FX_BSTRC(" R")) < 0) {
                            return -1;
                        }
                        offset += len + 4;
                    } else {
                        if (PDF_CreatorAppendObject(pCreator, pElement, pFile, offset, pObjVersion) < 0) {
                            return -1;
                        }
                    }
                }
                if (pFile->AppendString(FX_BSTRC("]")) < 0) {
                    return -1;
                }
                offset += 1;
                break;
            }
        case PDFOBJ_DICTIONARY: {
                if (pFile->AppendString(FX_BSTRC("<<")) < 0) {
                    return -1;
                }
                offset += 2;
                CPDF_Dictionary* p = (CPDF_Dictionary*)pObj;
                FX_POSITION pos = p->GetStartPos();
                while (pos) {
                    CFX_ByteString key;
                    CPDF_Object* pValue = p->GetNextElement(pos, key);
                    if (pFile->AppendString(FX_BSTRC("/")) < 0) {
                        return -1;
                    }
                    if ((len = pFile->AppendString(PDF_NameEncode(key))) < 0) {
                        return -1;
                    }
                    offset += len + 1;
                    if (pValue->GetObjNum()) {
                        if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_DWORD dwValueObjNum = pValue->GetObjNum();
                        if ((len = pFile->AppendDWord(dwValueObjNum)) < 0) {
                            return -1;
                        }
                        if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_WORD wValueObjVersion = 0;
                        if (pObjVersion && pObjVersion->GetSize() > static_cast<int>(dwValueObjNum)) {
                            wValueObjVersion = (*pObjVersion)[dwValueObjNum];
                        }
                        if ((len += pFile->AppendDWord(wValueObjVersion)) < 0) {
                            return -1;
                        }
                        if (pFile->AppendString(FX_BSTRC(" R")) < 0) {
                            return -1;
                        }
                        offset += len + 4;
                    } else {
                        if (PDF_CreatorAppendObject(pCreator, pValue, pFile, offset, pObjVersion) < 0) {
                            return -1;
                        }
                    }
                }
                if (pFile->AppendString(FX_BSTRC(">>")) < 0) {
                    return -1;
                }
                offset += 2;
                break;
            }
        case PDFOBJ_STREAM: {
                CPDF_Stream* p = (CPDF_Stream*)pObj;
                if (PDF_CreatorAppendObject(pCreator, p->GetDict(), pFile, offset, pObjVersion) < 0) {
                    return -1;
                }
                if (pFile->AppendString(FX_BSTRC("stream\r\n")) < 0) {
                    return -1;
                }
                offset += 8;
                CPDF_StreamAcc acc;
                acc.LoadAllData(p, TRUE);
                if (pFile->AppendBlock(acc.GetData(), acc.GetSize()) < 0) {
                    return -1;
                }
                offset += acc.GetSize();
                if ((len = pFile->AppendString(FX_BSTRC("\r\nendstream"))) < 0) {
                    return -1;
                }
                offset += len;
                break;
            }
        default:
            FXSYS_assert(FALSE);
            break;
    }
    return 1;
}
FX_FILESIZE PDF_CreatorWriteTrailer(CPDF_Creator* pCreator, CPDF_Document* pDocument, CFX_FileBufferArchive* pFile, CPDF_Array* pIDArray, FX_BOOL bCompress)
{
    FX_FILESIZE offset = 0;
    FX_INT32 len = 0;
    FXSYS_assert(pDocument && pFile);
    CPDF_Parser *pParser = (CPDF_Parser*)pDocument->GetParser();
    CPDF_Dictionary* p = pParser ? pParser->GetTrailer() : NULL;
    if (p) {
        FX_POSITION pos = p->GetStartPos();
        while (pos) {
            CFX_ByteString key;
            CPDF_Object* pValue = p->GetNextElement(pos, key);
            if (key == FX_BSTRC("Encrypt") || key == FX_BSTRC("Size") || key == FX_BSTRC("Filter") ||
                    key == FX_BSTRC("Index") || key == FX_BSTRC("Length") || key == FX_BSTRC("Prev") ||
                    key == FX_BSTRC("W") || key == FX_BSTRC("XRefStm") || key == FX_BSTRC("Type") || key == FX_BSTRC("ID")) {
                continue;
            }
            if (bCompress && key == FX_BSTRC("DecodeParms")) {
                continue;
            }
            if (pFile->AppendString((FX_BSTRC("/"))) < 0) {
                return -1;
            }
            if ((len = pFile->AppendString(PDF_NameEncode(key))) < 0) {
                return -1;
            }
            offset += len + 1;
            if (pValue->GetObjNum()) {
                if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                FX_DWORD dwValueObjNum = pValue->GetObjNum();
                if ((len = pFile->AppendDWord(dwValueObjNum)) < 0) {
                    return -1;
                }
                if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                FX_WORD wValueObjVersion = pCreator->GetObjectVersion(dwValueObjNum);
                if ((len += pFile->AppendDWord(wValueObjVersion)) < 0) {
                    return -1;
                }
                if (pFile->AppendString(FX_BSTRC(" R ")) < 0) {
                    return -1;
                }
                offset += len + 5;
            } else {
                if (PDF_CreatorAppendObject(pCreator, pValue, pFile, offset, NULL) < 0) {
                    return -1;
                }
            }
        }
        if (pIDArray) {
            if (pFile->AppendString((FX_BSTRC("/ID"))) < 0) {
                return -1;
            }
            offset += 3;
            if (PDF_CreatorAppendObject(pCreator, pIDArray, pFile, offset, NULL) < 0) {
                return -1;
            }
        }
        return offset;
    }
    if (pFile->AppendString(FX_BSTRC("\r\n/Root ")) < 0) {
        return -1;
    }
    FX_DWORD dwRootObjNum = pDocument->GetRoot()->GetObjNum();
    if ((len = pFile->AppendDWord(dwRootObjNum)) < 0) {
        return -1;
    }
    FX_WORD wRootObjVersion = static_cast<FX_WORD>(pParser ? pParser->GetObjectVersion(dwRootObjNum) : 0);
    if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
        return -1;
    }
    if ((len += pFile->AppendDWord(wRootObjVersion)) < 0) {
        return -1;
    }
    if (pFile->AppendString(FX_BSTRC(" R\r\n")) < 0) {
        return -1;
    }
    offset += len + 13;
    if (pDocument->GetInfo()) {
        if (pFile->AppendString(FX_BSTRC("/Info ")) < 0) {
            return -1;
        }
        FX_DWORD dwInfoObjNum = pDocument->GetInfo()->GetObjNum();
        if ((len = pFile->AppendDWord(dwInfoObjNum)) < 0) {
            return -1;
        }
        FX_WORD wInfoObjVersion = static_cast<FX_WORD>(pParser ? pParser->GetObjectVersion(dwInfoObjNum) : 0);
        if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
            return -1;
        }
        if ((len += pFile->AppendDWord(wInfoObjVersion)) < 0) {
            return -1;
        }
        if (pFile->AppendString(FX_BSTRC(" R\r\n")) < 0) {
            return -1;
        }
        offset += len + 11;
    }
    if (pIDArray) {
        if (pFile->AppendString((FX_BSTRC("/ID"))) < 0) {
            return -1;
        }
        offset += 3;
        if (PDF_CreatorAppendObject(pCreator, pIDArray, pFile, offset, NULL) < 0) {
            return -1;
        }
    }
    return offset;
}
FX_INT32 PDF_CreatorWriteEncrypt(const CPDF_Dictionary* pEncryptDict, FX_DWORD dwObjNum, FX_WORD wObjVersion, CFX_FileBufferArchive *pFile)
{
    if (!pEncryptDict) {
        return 0;
    }
    FXSYS_assert(pFile);
    FX_FILESIZE offset = 0;
    FX_INT32 len = 0;
    if (pFile->AppendString(FX_BSTRC("/Encrypt")) < 0) {
        return -1;
    }
    offset += 8;
    if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
        return -1;
    }
    if ((len = pFile->AppendDWord(dwObjNum)) < 0) {
        return -1;
    }
    if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
        return -1;
    }
    if ((len += pFile->AppendDWord(wObjVersion)) < 0) {
        return -1;
    }
    if (pFile->AppendString(FX_BSTRC(" R ")) < 0) {
        return -1;
    }
    offset += len + 5;
    return offset;
}
FX_BOOL PDF_GenerateFileID(FX_DWORD dwSeed1, FX_DWORD dwSeed2, FX_LPDWORD pBuffer)
{
    if (!pBuffer) {
        return FALSE;
    }
    FX_LPVOID pContext = FX_Random_MT_Start(dwSeed1);
    FX_INT32 i = 0;
    for (i = 0; i < 2; i++) {
        *pBuffer++ = FX_Random_MT_Generate(pContext);
    }
    FX_Random_MT_Close(pContext);
    pContext = FX_Random_MT_Start(dwSeed2);
    for (i = 0; i < 2; i++) {
        *pBuffer++ = FX_Random_MT_Generate(pContext);
    }
    FX_Random_MT_Close(pContext);
    return TRUE;
}
class CPDF_FlateEncoder
{
public:
    CPDF_FlateEncoder();
    ~CPDF_FlateEncoder();
    FX_BOOL		Initialize(CPDF_Stream* pStream, FX_BOOL bFlateEncode, FX_BOOL bSrcMetadateFlateEncode, FX_BOOL bStandardSecurity = FALSE, FX_BOOL isMetadata = FALSE, FX_BOOL bEncryptMetadata = FALSE);
    FX_BOOL		Initialize(FX_LPCBYTE pBuffer, FX_DWORD size, FX_BOOL bFlateEncode, FX_BOOL bXRefStream = FALSE);
    void		CloneDict();
    FX_LPBYTE			m_pData;
    FX_DWORD			m_dwSize;
    CPDF_Dictionary*	m_pDict;
    FX_BOOL				m_bCloned;
    FX_BOOL				m_bNewData;
    CPDF_StreamAcc*		m_pAcc;
};
CPDF_FlateEncoder::CPDF_FlateEncoder()
{
    m_pData = NULL;
    m_dwSize = 0;
    m_pDict = NULL;
    m_bCloned = FALSE;
    m_bNewData = FALSE;
    m_pAcc = FX_NEW CPDF_StreamAcc;
}
void CPDF_FlateEncoder::CloneDict()
{
    if (!m_bCloned) {
        m_pDict = (CPDF_Dictionary*)m_pDict->Clone();
        m_bCloned = TRUE;
    }
}
FX_BOOL CPDF_FlateEncoder::Initialize(CPDF_Stream* pStream, FX_BOOL bFlateEncode, FX_BOOL bSrcMetadateFlateEncode, FX_BOOL bStandardSecurity, FX_BOOL isMetadata, FX_BOOL bEncryptMetadata)
{
    CPDF_Dictionary* pDict = pStream ? pStream->GetDict() : NULL;
    if (!pDict) {
        return FALSE;
    }
    m_pAcc->LoadAllData(pStream, TRUE);
    if (pDict->KeyExist("Filter") || !bFlateEncode) {
        if (pDict->KeyExist("Filter") && !bFlateEncode) {
            CPDF_StreamAcc destAcc;
            destAcc.LoadAllData(pStream);
            m_dwSize = destAcc.GetSize();
            m_pData = (FX_LPBYTE)destAcc.DetachData();
            m_pDict = (CPDF_Dictionary*)pDict->Clone();
            if(!isMetadata) {
                m_pDict->RemoveAt(FX_BSTRC("Filter"));
            }
            m_bNewData = TRUE;
            m_bCloned = TRUE;
        } else {
            m_pData = (FX_LPBYTE)m_pAcc->GetData();
            m_dwSize = m_pAcc->GetSize();
            m_pDict = pStream->GetDict();
        }
        if(isMetadata) {
            if(bEncryptMetadata || !bStandardSecurity) {
                m_pDict->RemoveAt(FX_BSTRC("Filter"));
            }
            if(bStandardSecurity && !bEncryptMetadata) {
                CPDF_Array* pArray = CPDF_Array::Create();
                pArray->AddName(FX_BSTRC("Crypt"));
                m_pDict->SetAt(FX_BSTRC("Filter"), pArray);
                bSrcMetadateFlateEncode = FALSE;
            }
            if(bEncryptMetadata) {
                bSrcMetadateFlateEncode = FALSE;
            }
        } else {
            return TRUE;
        }
        if(!bSrcMetadateFlateEncode) {
            return TRUE;
        }
    }
    if (m_pData && m_pData != m_pAcc->GetData()) {
        FX_Free(m_pData);
    }
    m_pData = NULL;
    if (m_pDict && pStream && m_pDict != pStream->GetDict()) {
        m_pDict->Release();
    }
    m_pDict = NULL;
    m_dwSize = 0;
    m_bNewData = TRUE;
    m_bCloned = TRUE;
    ::FlateEncode(m_pAcc->GetData(), m_pAcc->GetSize(), m_pData, m_dwSize);
    m_pDict = (CPDF_Dictionary*)pDict->Clone();
    m_pDict->SetAtInteger("Length", m_dwSize);
    m_pDict->SetAtName("Filter", "FlateDecode");
    m_pDict->RemoveAt("DecodeParms");
    return TRUE;
}
FX_BOOL CPDF_FlateEncoder::Initialize(FX_LPCBYTE pBuffer, FX_DWORD size, FX_BOOL bFlateEncode, FX_BOOL bXRefStream)
{
    if (!bFlateEncode) {
        m_pData = (FX_LPBYTE)pBuffer;
        m_dwSize = size;
        return TRUE;
    }
    m_bNewData = TRUE;
    if (bXRefStream) {
        ::FlateEncode(pBuffer, size, 12, 1, 8, 7, m_pData, m_dwSize);
    } else {
        ::FlateEncode(pBuffer, size, m_pData, m_dwSize);
    }
    return TRUE;
}
CPDF_FlateEncoder::~CPDF_FlateEncoder()
{
    if (m_bCloned && m_pDict) {
        m_pDict->Release();
    }
    if (m_bNewData && m_pData) {
        FX_Free(m_pData);
    }
    if  (m_pAcc) {
        delete m_pAcc;
    }
}
class CPDF_Encryptor
{
public:
    CPDF_Encryptor();
    ~CPDF_Encryptor();
    FX_BOOL		Initialize(CPDF_CryptoHandler* pHandler, int objnum, FX_DWORD gennum, FX_LPBYTE src_data, FX_DWORD src_size);
    FX_LPBYTE			m_pData;
    FX_DWORD			m_dwSize;
    FX_BOOL				m_bNewBuf;
};
CPDF_Encryptor::CPDF_Encryptor()
{
    m_pData = NULL;
    m_dwSize = 0;
    m_bNewBuf = FALSE;
}
FX_BOOL CPDF_Encryptor::Initialize(CPDF_CryptoHandler* pHandler, int objnum, FX_DWORD gennum, FX_LPBYTE src_data, FX_DWORD src_size)
{
    if (src_size == 0) {
        return TRUE;
    }
    if (pHandler == NULL) {
        m_pData = (FX_LPBYTE)src_data;
        m_dwSize = src_size;
        m_bNewBuf = FALSE;
        return TRUE;
    }
    m_dwSize = pHandler->EncryptGetSize(objnum, 0, src_data, src_size);
    m_pData = FX_Alloc(FX_BYTE, m_dwSize);
    if(!m_pData) {
        return FALSE;
    }
    pHandler->EncryptContent(objnum, gennum, src_data, src_size, m_pData, m_dwSize);
    m_bNewBuf = TRUE;
    return TRUE;
}
CPDF_Encryptor::~CPDF_Encryptor()
{
    if (m_bNewBuf) {
        FX_Free(m_pData);
    }
}
#define FPDFAPI_DEFLATEINPUTBUFSIZE		32768
class CPDF_EncodeEncryptor
{
public:
    CPDF_EncodeEncryptor();
    ~CPDF_EncodeEncryptor();
    FX_BOOL				Initialize(CPDF_Stream* pStream, FX_BOOL bFlateEncode, CPDF_ProgressiveEncryptHandler* pHandler, FX_DWORD objnum, FX_DWORD gennum);
    IFX_FileStream*		m_pTempFile;
    FX_BOOL				m_bOwnerFile;
    FX_DWORD			m_Pos;
    CPDF_Dictionary*	m_pDict;
    CPDF_ProgressiveEncryptHandler*	m_pHandler;
};
CPDF_EncodeEncryptor::CPDF_EncodeEncryptor()
    : m_pTempFile(NULL)
    , m_bOwnerFile(FALSE)
    , m_Pos(0)
    , m_pDict(NULL)
    , m_pHandler(NULL)
{
}
CPDF_EncodeEncryptor::~CPDF_EncodeEncryptor()
{
    if (m_pTempFile) {
        if (m_bOwnerFile) {
            m_pTempFile->Release();
        } else {
            m_pHandler->ReleaseTempFile(m_pTempFile);
        }
    }
    if (m_pDict) {
        m_pDict->Release();
    }
}
FX_BOOL CPDF_EncodeEncryptor::Initialize(CPDF_Stream* pStream, FX_BOOL bFlateEncode, CPDF_ProgressiveEncryptHandler* pHandler, FX_DWORD objnum, FX_DWORD gennum)
{
    if (!pHandler || !pStream) {
        return FALSE;
    }
    m_pHandler = pHandler;
    FX_BOOL bFilterExist = pStream->GetDict()->KeyExist(FX_BSTRC("Filter"));
    FX_BOOL bRaw = bFilterExist && !bFlateEncode;
    void* context = pHandler->EncryptStart(objnum, gennum, pStream->GetRawSize(), (!bFilterExist && bFlateEncode));
    if (!context) {
        return FALSE;
    }
    m_pTempFile = pHandler->GetTempFile();
    if (!m_pTempFile) {
        m_pTempFile = FX_CreateMemoryStream();
        m_bOwnerFile = TRUE;
    }
    m_Pos = m_pTempFile->GetSize();
    CPDF_StreamFilter* pStreamFilter = pStream->GetStreamFilter(!bRaw);
    FX_BYTE buffer_in[FPDFAPI_DEFLATEINPUTBUFSIZE];
    FX_BOOL bCondition = TRUE;
    while (bCondition) {
        int read_len = pStreamFilter->ReadBlock(buffer_in, FPDFAPI_DEFLATEINPUTBUFSIZE);
        if (read_len == 0) {
            break;
        }
        pHandler->EncryptStream(context, buffer_in, read_len, m_pTempFile);
        if (read_len < FPDFAPI_DEFLATEINPUTBUFSIZE) {
            break;
        }
    }
    pHandler->EncryptFinish(context, m_pTempFile);
    m_pDict = (CPDF_Dictionary*)pStreamFilter->GetStream()->GetDict()->Clone();
    pHandler->UpdateFilter(m_pDict);
    m_pDict->SetAtInteger(FX_BSTRC("Length"), m_pTempFile->GetSize() - m_Pos);
    return TRUE;
}
CPDF_ObjectStream::CPDF_ObjectStream()
    : m_dwObjNum(0)
    , m_index(0)
{
}
FX_BOOL CPDF_ObjectStream::Start()
{
    m_ObjNumArray.RemoveAll();
    m_OffsetArray.RemoveAll();
    m_Buffer.Clear();
    m_dwObjNum = 0;
    m_index = 0;
    return TRUE;
}
FX_INT32 CPDF_ObjectStream::CompressIndirectObject(FX_DWORD dwObjNum, const CPDF_Object *pObj, CPDF_Creator* pCreator)
{
  FX_UNREFERENCED_PARAMETER(pCreator);
    m_ObjNumArray.Add(dwObjNum);
    m_OffsetArray.Add(m_Buffer.GetLength());
    m_Buffer << pObj;
    return 1;
}
FX_INT32 CPDF_ObjectStream::CompressIndirectObject(FX_DWORD dwObjNum, FX_LPCBYTE pBuffer, FX_DWORD dwSize)
{
    m_ObjNumArray.Add(dwObjNum);
    m_OffsetArray.Add(m_Buffer.GetLength());
    m_Buffer.AppendBlock(pBuffer, dwSize);
    return 1;
}
FX_FILESIZE CPDF_ObjectStream::End(CPDF_Creator* pCreator)
{
    FXSYS_assert(pCreator);
    if (m_ObjNumArray.GetSize() == 0) {
        return 0;
    }
    CFX_FileBufferArchive *pFile = &pCreator->m_File;
    CPDF_CryptoHandler *pHandler = pCreator->crypto_handler_;
    FX_FILESIZE ObjOffset = pCreator->m_Offset;
    if (!m_dwObjNum) {
        m_dwObjNum = ++pCreator->m_dwLastObjNum;
    }
    CFX_ByteTextBuf tempBuffer;
    FX_INT32 iCount = m_ObjNumArray.GetSize();
    for (FX_INT32 i = 0; i < iCount; i++) {
        tempBuffer << m_ObjNumArray.ElementAt(i) << FX_BSTRC(" ") << m_OffsetArray.ElementAt(i) << FX_BSTRC(" ");
    }
    FX_FILESIZE &offset = pCreator->m_Offset;
    FX_INT32 len = pFile->AppendDWord(m_dwObjNum);
    if (len < 0) {
        return -1;
    }
    offset += len;
    if ((len = pFile->AppendString(FX_BSTRC(" 0 obj\r\n<</Type /ObjStm /N "))) < 0) {
        return -1;
    }
    offset += len;
    if ((len = pFile->AppendDWord((FX_DWORD)iCount)) < 0) {
        return -1;
    }
    offset += len;
    if (pFile->AppendString(FX_BSTRC("/First ")) < 0) {
        return -1;
    }
    if ((len = pFile->AppendDWord((FX_DWORD)tempBuffer.GetLength())) < 0) {
        return -1;
    }
    if (pFile->AppendString(FX_BSTRC("/Length ")) < 0) {
        return -1;
    }
    offset += len + 15;
    if (!pCreator->m_bCompress && !pHandler) {
        if ((len = pFile->AppendDWord((FX_DWORD)(tempBuffer.GetLength() + m_Buffer.GetLength()))) < 0) {
            return -1;
        }
        offset += len;
        if ((len = pFile->AppendString(FX_BSTRC(">>stream\r\n"))) < 0) {
            return -1;
        }
        if (pFile->AppendBlock(tempBuffer.GetBuffer(), tempBuffer.GetLength()) < 0) {
            return -1;
        }
        if (pFile->AppendBlock(m_Buffer.GetBuffer(), m_Buffer.GetLength()) < 0) {
            return -1;
        }
        offset += len + tempBuffer.GetLength() + m_Buffer.GetLength();
    } else {
        tempBuffer << m_Buffer;
        CPDF_FlateEncoder encoder;
        encoder.Initialize(tempBuffer.GetBuffer(), tempBuffer.GetLength(), pCreator->m_bCompress);
        CPDF_Encryptor encryptor;
        encryptor.Initialize(pHandler, m_dwObjNum, 0, encoder.m_pData, encoder.m_dwSize);
        if ((len = pFile->AppendDWord(encryptor.m_dwSize)) < 0) {
            return -1;
        }
        offset += len;
        if (pCreator->m_bCompress) {
            if (pFile->AppendString(FX_BSTRC("/Filter /FlateDecode")) < 0) {
                return -1;
            }
            offset += 20;
        }
        if ((len = pFile->AppendString(FX_BSTRC(">>stream\r\n"))) < 0) {
            return -1;
        }
        if (pFile->AppendBlock(encryptor.m_pData, encryptor.m_dwSize) < 0) {
            return -1;
        }
        offset += len + encryptor.m_dwSize;
    }
    if ((len = pFile->AppendString(FX_BSTRC("\r\nendstream\r\nendobj\r\n"))) < 0) {
        return -1;
    }
    offset += len;
    return ObjOffset;
}
CPDF_XRefStream::CPDF_XRefStream()
    : m_PrevOffset(0)
    , m_dwTempObjNum(0)
    , m_iSeg(0)
{
}
FX_BOOL CPDF_XRefStream::Start()
{
    m_IndexArray.RemoveAll();
    m_Buffer.Clear();
    m_iSeg = 0;
    m_1TypeArray.RemoveAll();
    m_2FieldArray.RemoveAll();
    m_3FieldArray.RemoveAll();
    return TRUE;
}
FX_INT32 CPDF_XRefStream::CompressIndirectObject(FX_DWORD dwObjNum, const CPDF_Object *pObj, CPDF_Creator *pCreator)
{
    if (!pCreator) {
        return 0;
    }
    m_ObjStream.CompressIndirectObject(dwObjNum, pObj, pCreator);
    if (m_ObjStream.m_ObjNumArray.GetSize() < pCreator->m_ObjectStreamSize &&
            m_ObjStream.m_Buffer.GetLength() < PDF_OBJECTSTREAM_MAXLENGTH) {
        return 1;
    }
    return EndObjectStream(pCreator);
}
FX_INT32 CPDF_XRefStream::CompressIndirectObject(FX_DWORD dwObjNum, FX_LPCBYTE pBuffer, FX_DWORD dwSize, CPDF_Creator *pCreator)
{
    if (!pCreator) {
        return 0;
    }
    m_ObjStream.CompressIndirectObject(dwObjNum, pBuffer, dwSize);
    if (m_ObjStream.m_ObjNumArray.GetSize() < pCreator->m_ObjectStreamSize &&
            m_ObjStream.m_Buffer.GetLength() < PDF_OBJECTSTREAM_MAXLENGTH) {
        return 1;
    }
    return EndObjectStream(pCreator);
}
static void _AppendIndex0(CFX_ByteTextBuf& buffer, FX_DWORD objNum, FX_INT32 objNum_bit, FX_INT32 genNum = 0, FX_INT32 genNum_bit = 2)
{
  FX_UNREFERENCED_PARAMETER(genNum_bit);
  FX_UNREFERENCED_PARAMETER(genNum);
    buffer.AppendByte(0);
    for (FX_INT32 i = 0; i < objNum_bit; i++ ) {
        buffer.AppendByte(0);
    }
    if (objNum == 0) {
        buffer.AppendByte(0xFF);
        buffer.AppendByte(0xFF);
    } else {
        buffer.AppendByte(0);
        buffer.AppendByte(0);
    }
}
static void _AppendIndex1(CFX_ByteTextBuf& buffer, FX_FILESIZE offset, FX_INT32 offset_bit, FX_INT32 genNum = 0, FX_INT32 genNum_bit = 2)
{
  FX_UNREFERENCED_PARAMETER(genNum_bit);
    buffer.AppendByte(1);
    FX_INT32 i = 0;
    for (; i < offset_bit; i++ ) {
        FX_FILESIZE tmpoffset = offset;
        for (FX_INT32 j = offset_bit - i - 1; j > 0; j--) {
            tmpoffset = tmpoffset >> 8;
        }
        buffer.AppendByte((FX_BYTE)tmpoffset);
    }
    if (genNum) {
        FX_WORD wGenNum = (FX_WORD)genNum;
        buffer.AppendByte((FX_BYTE)(wGenNum >> 8));
        buffer.AppendByte((FX_BYTE)(wGenNum));
    } else {
        buffer.AppendByte(0);
        buffer.AppendByte(0);
    }
}
static void _AppendIndex2(CFX_ByteTextBuf& buffer,  FX_DWORD objNum, FX_INT32 objNum_bit, FX_INT32 index, FX_INT32 index_bit = 2)
{
  FX_UNREFERENCED_PARAMETER(index_bit);
    buffer.AppendByte(2);
    FX_INT32 i = 0;
    for (; i < objNum_bit; i++) {
        FX_DWORD tmpobjNum = objNum;
        for (FX_INT32 j = objNum_bit - i - 1; j > 0; j--) {
            tmpobjNum = tmpobjNum >> 8;
        }
        buffer.AppendByte((FX_BYTE)tmpobjNum);
    }
    buffer.AppendByte(FX_GETBYTEOFFSET8(index));
    buffer.AppendByte(FX_GETBYTEOFFSET0(index));
}
FX_INT32 CPDF_XRefStream::EndObjectStream(CPDF_Creator *pCreator, FX_BOOL bEOF)
{
    FX_FILESIZE objOffset = 0;
    if (bEOF) {
        objOffset = m_ObjStream.End(pCreator);
        if (objOffset < 0) {
            return -1;
        }
    }
    FX_DWORD &dwObjStmNum = m_ObjStream.m_dwObjNum;
    if (!dwObjStmNum) {
        dwObjStmNum = ++pCreator->m_dwLastObjNum;
    }
    FX_INT32 iSize = m_ObjStream.m_ObjNumArray.GetSize();
    FX_INT32 iSeg = m_IndexArray.GetSize() / 2;
    if (iSize == 0 && iSeg == 0) {
        return 1;
    }
    if (!(pCreator->m_dwFlags & FPDFCREATE_INCREMENTAL)) {
        if (m_dwTempObjNum == 0) {
            m_1TypeArray.Add(0);
            m_2FieldArray.Add(0);
            m_3FieldArray.Add(0xFFFF);
            m_dwTempObjNum++;
        }
        FX_DWORD end_num = 0;
        for (FX_INT32 i = 0; i < iSeg; i++) {
            FX_DWORD seg_end_num = m_IndexArray.GetAt(2 * i) + m_IndexArray.GetAt(2 * i + 1);
            if (seg_end_num > end_num) {
                end_num = seg_end_num;
            }
        }
        FX_DWORD index = 0;
        for (; m_dwTempObjNum < end_num; m_dwTempObjNum++) {
            FX_FILESIZE* offset = pCreator->m_ObjectOffset.GetPtrAt(m_dwTempObjNum);
            if (offset && *offset) {
                if (index >= static_cast<FX_DWORD>(iSize) || m_dwTempObjNum != m_ObjStream.m_ObjNumArray[index]) {
                    m_1TypeArray.Add(1);
                    m_2FieldArray.Add(*offset);
                    m_3FieldArray.Add(pCreator->GetObjectVersion(m_dwTempObjNum));
                } else {
                    m_1TypeArray.Add(2);
                    m_2FieldArray.Add(dwObjStmNum);
                    m_3FieldArray.Add(index++);
                }
            } else {
                m_1TypeArray.Add(0);
                m_2FieldArray.Add(m_dwTempObjNum);
                m_3FieldArray.Add(0);
            }
        }
        if (iSize > 0 && bEOF) {
            pCreator->m_ObjectOffset.Add(dwObjStmNum, 1);
            pCreator->m_ObjectSize.Add(dwObjStmNum, 1);
            pCreator->m_ObjectOffset[dwObjStmNum] = objOffset;
        }
        m_iSeg = iSeg;
        if (bEOF) {
            m_ObjStream.Start();
        }
        return 1;
    }
    FX_INT32 &j = m_ObjStream.m_index;
    for (int i = m_iSeg; i < iSeg; i++) {
        FX_DWORD start = m_IndexArray.ElementAt(i * 2);
        FX_DWORD end = m_IndexArray.ElementAt(i * 2 + 1) + start;
        for (FX_DWORD m = start; m < end; m++) {
            if (j >= iSize || m != m_ObjStream.m_ObjNumArray.ElementAt(j)) {
                m_1TypeArray.Add(1);
                m_2FieldArray.Add(pCreator->m_ObjectOffset[m]);
                m_3FieldArray.Add(pCreator->GetObjectVersion(j));
            } else {
                m_1TypeArray.Add(2);
                m_2FieldArray.Add(dwObjStmNum);
                m_3FieldArray.Add(j++);
            }
        }
    }
    if (iSize > 0 && bEOF) {
        m_1TypeArray.Add(1);
        m_2FieldArray.Add(objOffset);
        m_3FieldArray.Add(0);
        m_IndexArray.Add(dwObjStmNum);
        m_IndexArray.Add(1);
        iSeg += 1;
    }
    m_iSeg = iSeg;
    if (bEOF) {
        m_ObjStream.Start();
    }
    return 1;
}
FX_BOOL CPDF_XRefStream::GenerateXRefStream(CPDF_Creator* pCreator, FX_BOOL bEOF)
{
    pCreator->m_MaxOffset = pCreator->m_MaxOffset > pCreator->m_Offset ? pCreator->m_MaxOffset : pCreator->m_Offset;
    FX_FILESIZE offset_tmp = pCreator->m_Offset;
    FX_DWORD objnum = ++pCreator->m_dwLastObjNum;
    CFX_FileBufferArchive *pFile = &pCreator->m_File;
    FX_BOOL bIncremental = (pCreator->m_dwFlags & FPDFCREATE_INCREMENTAL) != 0;
    if (bIncremental) {
        AddObjectNumberToIndexArray(objnum);
    } else {
        for (; m_dwTempObjNum < pCreator->m_dwLastObjNum; m_dwTempObjNum++) {
            FX_FILESIZE* offset = pCreator->m_ObjectOffset.GetPtrAt(m_dwTempObjNum);
            if (offset && *offset) {
                m_1TypeArray.Add(1);
                m_2FieldArray.Add(*offset);
                m_3FieldArray.Add(pCreator->GetObjectVersion(m_dwTempObjNum));
            } else {
                m_1TypeArray.Add(0);
                m_2FieldArray.Add(m_dwTempObjNum);
                m_3FieldArray.Add(0);
            }
        }
    }
    m_1TypeArray.Add(1);
    m_2FieldArray.Add(offset_tmp);
    m_3FieldArray.Add(0);
    FX_INT32 field2W = pCreator->m_MaxOffset > 0xFFFFFFFFUL ? 5 : 4;
    FX_INT32 field3W = 2;
    for (FX_INT32 index = 0; index < m_1TypeArray.GetSize(); index++) {
        switch(m_1TypeArray[index]) {
            case 0:
                _AppendIndex0(m_Buffer, m_2FieldArray[index], field2W, m_3FieldArray[index]);
                break;
            case 1:
                _AppendIndex1(m_Buffer, m_2FieldArray[index], field2W, m_3FieldArray[index]);
                break;
            case 2:
                _AppendIndex2(m_Buffer, m_2FieldArray[index], field2W, m_3FieldArray[index]);
                break;
            default:
                ;
        }
    }
    FX_FILESIZE &offset = pCreator->m_Offset;
    FX_INT32 len = pFile->AppendDWord(objnum);
    if (len < 0) {
        return FALSE;
    }
    offset += len;
    if ((len = pFile->AppendString(FX_BSTRC(" 0 obj\r\n<</Type /XRef/W[1 "))) < 0) {
        return FALSE;
    }
    offset += len;
    if ((len = pFile->AppendDWord(field2W)) < 0) {
        return FALSE;
    }
    offset += len;
    if ((len = pFile->AppendString(FX_BSTRC(" "))) < 0) {
        return FALSE;
    }
    offset += len;
    if ((len = pFile->AppendDWord(field3W)) < 0) {
        return FALSE;
    }
    offset += len;
    if ((len = pFile->AppendString(FX_BSTRC("]/Index["))) < 0) {
        return FALSE;
    }
    offset += len;
    if (!bIncremental) {
        if ((len = pFile->AppendDWord(0)) < 0) {
            return FALSE;
        }
        if ((len = pFile->AppendString(FX_BSTRC(" "))) < 0) {
            return FALSE;
        }
        offset += len + 1;
        if ((len = pFile->AppendDWord(objnum + 1)) < 0) {
            return FALSE;
        }
        offset += len;
    } else {
        FX_INT32 iSeg = m_IndexArray.GetSize() / 2;
        for (FX_INT32 i = 0; i < iSeg; i++) {
            if ((len = pFile->AppendDWord(m_IndexArray.ElementAt(i * 2))) < 0) {
                return FALSE;
            }
            if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                return FALSE;
            }
            offset += len + 1;
            if ((len = pFile->AppendDWord(m_IndexArray.ElementAt(i * 2 + 1))) < 0) {
                return FALSE;
            }
            if (pFile->AppendString(FX_BSTRC(" ")) < 0) {
                return FALSE;
            }
            offset += len + 1;
        }
    }
    if (pFile->AppendString(FX_BSTRC("]/Size ")) < 0) {
        return FALSE;
    }
    if ((len = pFile->AppendDWord(objnum + 1)) < 0) {
        return FALSE;
    }
    offset += len + 7;
    if (m_PrevOffset > 0) {
        if (pFile->AppendString(FX_BSTRC("/Prev ")) < 0) {
            return -1;
        }
        FX_FILESIZE tmpPrevOffset = m_PrevOffset;
        if (pCreator->m_pParser->IsParsingLinearized()) {
            tmpPrevOffset = pCreator->m_pParser->GetLinearizedFirstPageXRefOffset();
        }
        FX_CHAR offset_buf[20];
        FXSYS_memset32(offset_buf, 0, sizeof(offset_buf));
        FXSYS_i64toa(tmpPrevOffset, offset_buf, 10);
        FX_INT32 bufLen = (FX_INT32)FXSYS_strlen(offset_buf);
        if (pFile->AppendBlock(offset_buf, bufLen) < 0) {
            return -1;
        }
        offset += bufLen + 6;
    }
    FX_BOOL bPredictor = TRUE;
    CPDF_FlateEncoder encoder;
    encoder.Initialize(m_Buffer.GetBuffer(), m_Buffer.GetLength(), pCreator->m_bCompress, bPredictor);
    if (pCreator->m_bCompress) {
        if (pFile->AppendString(FX_BSTRC("/Filter /FlateDecode")) < 0) {
            return FALSE;
        }
        offset += 20;
        if (bPredictor) {
            CFX_ByteString sDecodeParams;
            sDecodeParams.Format("/DecodeParms<</Columns 7/Predictor 12>>");
            if ((len = pFile->AppendString(sDecodeParams)) < 0) {
                return FALSE;
            }
            offset += len;
        }
    }
    if (pFile->AppendString(FX_BSTRC("/Length ")) < 0) {
        return FALSE;
    }
    if ((len = pFile->AppendDWord(encoder.m_dwSize)) < 0) {
        return FALSE;
    }
    offset += len + 8;
    if (bEOF) {
        if ((len = PDF_CreatorWriteTrailer(pCreator, pCreator->m_pDocument, pFile, pCreator->m_pIDArray, pCreator->m_bCompress)) < 0) {
            return -1;
        }
        offset += len;
        if (pCreator->m_pEncryptDict) {
            FX_DWORD dwEncryptObjNum = pCreator->m_pEncryptDict->GetObjNum();
            if (dwEncryptObjNum == 0) {
                dwEncryptObjNum = pCreator->m_dwEnryptObjNum;
            }
            FX_WORD wEncryptObjVersion = pCreator->GetObjectVersion(dwEncryptObjNum);
            if ((len = PDF_CreatorWriteEncrypt(pCreator->m_pEncryptDict, dwEncryptObjNum, wEncryptObjVersion, pFile)) < 0) {
                return -1;
            }
            offset += len;
        }
    }
    if ((len = pFile->AppendString(FX_BSTRC(">>stream\r\n"))) < 0) {
        return FALSE;
    }
    offset += len;
    if (pFile->AppendBlock(encoder.m_pData, encoder.m_dwSize) < 0) {
        return FALSE;
    }
    if ((len = pFile->AppendString(FX_BSTRC("\r\nendstream\r\nendobj\r\n"))) < 0) {
        return FALSE;
    }
    offset += encoder.m_dwSize + len;
    m_PrevOffset = offset_tmp;
    return TRUE;
}
FX_BOOL CPDF_XRefStream::End(CPDF_Creator *pCreator, FX_BOOL bEOF )
{
    if (EndObjectStream(pCreator, bEOF) < 0) {
        return FALSE;
    }
    return GenerateXRefStream(pCreator, bEOF);
}
FX_BOOL CPDF_XRefStream::EndXRefStream(CPDF_Creator* pCreator)
{
    if (!(pCreator->m_dwFlags & FPDFCREATE_INCREMENTAL)) {
        m_1TypeArray.Add(0);
        m_2FieldArray.Add(0);
        m_3FieldArray.Add(0xFFFF);
        for (FX_DWORD i = 1; i < pCreator->m_dwLastObjNum + 1; i++) {
            FX_FILESIZE* offset = pCreator->m_ObjectOffset.GetPtrAt(i);
            if (offset && *offset) {
                m_1TypeArray.Add(1);
                m_2FieldArray.Add(*offset);
                m_3FieldArray.Add(pCreator->GetObjectVersion(i));
            } else {
                m_1TypeArray.Add(0);
                m_2FieldArray.Add(i);
                m_3FieldArray.Add(0);
            }
        }
    } else {
        FX_INT32 iSeg = m_IndexArray.GetSize() / 2;
        for (int i = 0; i < iSeg; i++) {
            FX_DWORD start = m_IndexArray.ElementAt(i * 2);
            FX_DWORD end = m_IndexArray.ElementAt(i * 2 + 1) + start;
            for (FX_DWORD j = start; j < end; j++) {
                m_1TypeArray.Add(1);
                m_2FieldArray.Add(pCreator->m_ObjectOffset[j]);
                m_3FieldArray.Add(pCreator->GetObjectVersion(i));
            }
        }
    }
    return GenerateXRefStream(pCreator, FALSE);
}
FX_BOOL CPDF_XRefStream::AddObjectNumberToIndexArray(FX_DWORD objnum, FX_BOOL bSort)
{
    FX_INT32 iSize = m_IndexArray.GetSize();
    if (iSize == 0) {
        m_IndexArray.Add(objnum);
        m_IndexArray.Add(1);
    } else {
        if(bSort) {
            FX_INT32 nInsertAt = iSize;
            FX_INT32 nEntries = iSize / 2;
            for(int i = nEntries - 1; i >= 0; i--) {
                if(m_IndexArray[2 * i] < objnum) {
                    nInsertAt = 2 * (i + 1);
                    break;
                }
            }
            FX_DWORD startobjnum = m_IndexArray[nInsertAt - 2];
            FX_DWORD count = m_IndexArray[nInsertAt - 1];
            if (objnum == startobjnum + count) {
                m_IndexArray[nInsertAt - 1] = count + 1;
            } else {
                m_IndexArray.InsertAt(nInsertAt, objnum, 1);
                m_IndexArray.InsertAt(nInsertAt + 1, 1);
            }
        } else {
            FXSYS_assert(iSize > 1);
            FX_DWORD startobjnum = m_IndexArray.ElementAt(iSize - 2);
            FX_INT32 iCount = m_IndexArray.ElementAt(iSize - 1);
            if (objnum == startobjnum + iCount) {
                m_IndexArray[iSize - 1] = iCount + 1;
            } else {
                m_IndexArray.Add(objnum);
                m_IndexArray.Add(1);
            }
        }
    }
    return TRUE;
}
CPDF_Creator::CPDF_Creator(CPDF_Document* pDoc)
{
    m_pDocument = pDoc;
    if (pDoc) {
        m_pParser = (CPDF_Parser*)pDoc->m_pParser;
    } else {
        m_pParser = NULL;
    }
    m_bCompress = TRUE;
    if (m_pParser) {
        m_pEncryptDict = m_pParser->GetEncryptDict();
        crypto_handler_ = m_pParser->GetCryptoHandler();
    } else {
        m_pEncryptDict = NULL;
        crypto_handler_ = NULL;
    }
    m_bSecurityChanged = FALSE;
    m_pMetadata = NULL;
    if(m_pParser && m_pParser->GetSecurityHandler()) {
        m_bEncryptMetadata = m_pParser->GetSecurityHandler()->IsMetadataEncrypted();
        m_bStandardSecurity = m_pParser->GetSecurityHandler()->GetStandardHandler() ? TRUE : FALSE;
    } else {
        m_bEncryptMetadata = FALSE;
        m_bStandardSecurity = FALSE;
    }
    m_Offset = 0;
    m_FilePos = 0;
    m_MaxOffset = 0;
    m_iStage = -1;
    m_dwFlags = 0;
    m_Pos = NULL;
    m_XrefStart = 0;
    m_pXRefStream = NULL;
    m_ObjectStreamSize = 200;
    if (m_pDocument) {
        m_dwLastObjNum = m_pDocument->GetLastObjNum();
    } else {
        m_dwLastObjNum = 0;
    }
    m_pIDArray = NULL;
    m_dwEnryptObjNum = 0;
    m_bNewCrypto = FALSE;
    m_pProgressiveEncrypt = NULL;
}
CPDF_Creator::~CPDF_Creator()
{
    ResetStandardSecurity();
    if (m_pProgressiveEncrypt) {
        delete m_pProgressiveEncrypt;
    }
    Clear();
}
static FX_BOOL _IsXRefNeedEnd(CPDF_XRefStream* pXRef, FX_DWORD flag)
{
    if (!(flag & FPDFCREATE_INCREMENTAL)) {
        return FALSE;
    }
    FX_INT32 iSize = pXRef->m_IndexArray.GetSize() / 2;
    FX_INT32 iCount = 0;
    for (FX_INT32 i = 0; i < iSize; i++) {
        iCount += pXRef->m_IndexArray.ElementAt(i * 2 + 1);
    }
    return (iCount >= PDF_XREFSTREAM_MAXSIZE);
}
FX_INT32 CPDF_Creator::WriteIndirectObjectToStream(FX_DWORD objnum, const CPDF_Object* pObj)
{
    if (!m_pXRefStream) {
        return 1;
    }
    if (m_pParser && m_pParser->m_ObjVersion.GetSize() > (FX_INT32)objnum && m_pParser->m_ObjVersion[objnum] > 0) {
        return 1;
    }
    if (pObj->GetType() == PDFOBJ_NUMBER || pObj->GetType() == PDFOBJ_BOOLEAN) {
        return 1;
    }
    CPDF_Dictionary *pDict = pObj->GetDict();
    if (pObj->GetType() == PDFOBJ_STREAM) {
        if (pDict && pDict->GetString(FX_BSTRC("Type")) == FX_BSTRC("XRef")) {
            return 0;
        }
        return 1;
    }
    if (pDict) {
        if (pDict == m_pDocument->m_pRootDict || pDict == m_pEncryptDict) {
            return 1;
        }
        if (IsSignatureDict(pDict, 0, NULL)) {
            return 1;
        }
        if (pDict->GetString(FX_BSTRC("Type")) == FX_BSTRC("Page")) {
            return 1;
        }
    }
    m_pXRefStream->AddObjectNumberToIndexArray(objnum);
    if (m_pXRefStream->CompressIndirectObject(objnum, pObj, this) < 0) {
        return -1;
    }
    if (!_IsXRefNeedEnd(m_pXRefStream, m_dwFlags)) {
        return 0;
    }
    if (!m_pXRefStream->End(this)) {
        return -1;
    }
    if (!m_pXRefStream->Start()) {
        return -1;
    }
    return 0;
}
FX_INT32 CPDF_Creator::WriteIndirectObjectToStream(FX_DWORD objnum, FX_LPCBYTE pBuffer, FX_DWORD dwSize)
{
    if (!m_pXRefStream) {
        return 1;
    }
    m_pXRefStream->AddObjectNumberToIndexArray(objnum);
    FX_INT32 iRet = m_pXRefStream->CompressIndirectObject(objnum, pBuffer, dwSize, this);
    if (iRet < 1) {
        return iRet;
    }
    if (!_IsXRefNeedEnd(m_pXRefStream, m_dwFlags)) {
        return 0;
    }
    if (!m_pXRefStream->End(this)) {
        return -1;
    }
    if (!m_pXRefStream->Start()) {
        return -1;
    }
    return 0;
}
FX_INT32 CPDF_Creator::AppendObjectNumberToXRef(FX_DWORD objnum)
{
    if (!m_pXRefStream) {
        return 1;
    }
    m_pXRefStream->AddObjectNumberToIndexArray(objnum);
    if (!_IsXRefNeedEnd(m_pXRefStream, m_dwFlags)) {
        return 0;
    }
    if (!m_pXRefStream->End(this)) {
        return -1;
    }
    if (!m_pXRefStream->Start()) {
        return -1;
    }
    return 0;
}
FX_INT32 CPDF_Creator::WriteStream(const CPDF_Object* pStream, FX_DWORD objnum, CPDF_CryptoHandler* pCrypto)
{
    CPDF_FlateEncoder encoder;
    encoder.Initialize((CPDF_Stream*)pStream, pStream == m_pMetadata ? FALSE : m_bCompress, m_pDocument->m_bMetaDataFlate, m_bStandardSecurity, pStream == m_pMetadata , m_bEncryptMetadata);
    CPDF_Encryptor encryptor;
    FX_DWORD gennum = GetObjectVersion(objnum);
    if(!encryptor.Initialize(pCrypto, objnum, gennum, encoder.m_pData, encoder.m_dwSize)) {
        return -1;
    }
    if ((FX_DWORD)encoder.m_pDict->GetInteger(FX_BSTRC("Length")) != encryptor.m_dwSize) {
        encoder.CloneDict();
        encoder.m_pDict->SetAtInteger(FX_BSTRC("Length"), encryptor.m_dwSize);
    }
    if (WriteDirectObj(objnum, encoder.m_pDict) < 0) {
        return -1;
    }
    int len = m_File.AppendString(FX_BSTRC("stream\r\n"));
    if (len < 0) {
        return -1;
    }
    m_Offset += len;
    if (m_File.AppendBlock(encryptor.m_pData, encryptor.m_dwSize) < 0) {
        return -1;
    }
    m_Offset += encryptor.m_dwSize;
    if ((len = m_File.AppendString(FX_BSTRC("\r\nendstream"))) < 0) {
        return -1;
    }
    m_Offset += len;
    return 1;
}
FX_INT32 CPDF_Creator::WriteStream(const CPDF_Object* pStream, FX_DWORD objnum)
{
    CPDF_EncodeEncryptor encodeEncryptor;
    FX_DWORD gennum = GetObjectVersion(objnum);
    FX_BOOL bFlate = (pStream == m_pMetadata ? FALSE : m_bCompress);
    if (!encodeEncryptor.Initialize((CPDF_Stream*)pStream, bFlate, m_pProgressiveEncrypt, objnum, gennum)) {
        return 0;
    }
    if (WriteDirectObj(objnum, encodeEncryptor.m_pDict) < 0) {
        return -1;
    }
    int len = m_File.AppendString(FX_BSTRC("stream\r\n"));
    if (len < 0) {
        return -1;
    }
    m_Offset += len;
    FX_DWORD length = (FX_DWORD)encodeEncryptor.m_pTempFile->GetSize() - encodeEncryptor.m_Pos;
    FX_DWORD size = FX_MIN(length - encodeEncryptor.m_Pos, 10240);
    FX_LPBYTE buffer = FX_Alloc(FX_BYTE, size);
    while (encodeEncryptor.m_Pos < length) {
        size = FX_MIN(size, length - encodeEncryptor.m_Pos);
        encodeEncryptor.m_pTempFile->ReadBlock(buffer, encodeEncryptor.m_Pos, size);
        len = m_File.AppendBlock(buffer, size);
        if (len < 0) {
            return -1;
        }
        m_Offset += len;
        encodeEncryptor.m_Pos += size;
    }
    FX_Free(buffer);
    if ((len = m_File.AppendString(FX_BSTRC("\r\nendstream"))) < 0) {
        return -1;
    }
    m_Offset += len;
    return 1;
}
CFX_ByteString	CPDF_Creator::GenerateFileVersion(FX_INT32 fileVersion)
{
    CFX_ByteString bsVersion;
    if (fileVersion > 9) {
        bsVersion = CFX_ByteString::FormatInteger(fileVersion / 10);
        bsVersion += '.';
    } else {
        bsVersion = FX_BSTRC("1.");
    }
    bsVersion += CFX_ByteString::FormatInteger(fileVersion % 10);
    return bsVersion;
}
FX_WORD CPDF_Creator::GetObjectVersion(FX_DWORD objNum)
{
    if (objNum == 0) {
        return 0;
    }
    if (!m_pParser) {
        return 0;
    }
    return static_cast<FX_WORD>(m_pParser->GetObjectVersion(objNum));
}
FX_INT32 CPDF_Creator::WriteIndirectObj(FX_DWORD objnum, const CPDF_Object* pObj)
{
    FX_DWORD oldnum = objnum;
    FX_INT32 len = m_File.AppendDWord(objnum);
    if (len < 0) {
        return -1;
    }
    m_Offset += len;
    if ((len = m_File.AppendString(FX_BSTRC(" "))) < 0) {
        return -1;
    }
    m_Offset += len;
    FX_WORD objVersion = GetObjectVersion(objnum);
    if ((len = m_File.AppendDWord(objVersion)) < 0) {
        return -1;
    }
    m_Offset += len;
    if ((len = m_File.AppendString(FX_BSTRC(" obj\r\n"))) < 0) {
        return -1;
    }
    m_Offset += len;
    if (pObj->GetType() == PDFOBJ_STREAM) {
        CPDF_CryptoHandler *pHandler = NULL;
        pHandler = (pObj == m_pMetadata && !m_bEncryptMetadata) ? NULL : crypto_handler_;
        int ret = 0;
        if (pHandler && m_pProgressiveEncrypt && m_pProgressiveEncrypt->SetCryptoHandler(pHandler)) {
            ret = WriteStream(pObj, oldnum);
        }
        if (ret < 0) {
            return -1;
        }
        if (ret == 0) if (WriteStream(pObj, oldnum, pHandler) < 0) {
                return -1;
            }
    } else {
        if (WriteDirectObj(objnum, pObj) < 0) {
            return -1;
        }
    }
    if ((len = m_File.AppendString(FX_BSTRC("\r\nendobj\r\n"))) < 0) {
        return -1;
    }
    m_Offset += len;
    if (AppendObjectNumberToXRef(objnum) < 0) {
        return -1;
    }
    return 0;
}
FX_INT32 CPDF_Creator::WriteIndirectObj(const CPDF_Object* pObj)
{
    FX_INT32 iRet = WriteIndirectObjectToStream(pObj->GetObjNum(), pObj);
    if (iRet < 1) {
        return iRet;
    }
    return WriteIndirectObj(pObj->GetObjNum(), pObj);
}
FX_INT32 CPDF_Creator::WriteDirectObj(FX_DWORD objnum, const CPDF_Object* pObj, FX_BOOL bEncrypt)
{
    FX_INT32 len = 0;
    if (pObj == NULL) {
        if (m_File.AppendString(FX_BSTRC(" null")) < 0) {
            return -1;
        }
        m_Offset += 5;
        return 1;
    }
    
    switch (pObj->GetType()) {
        case PDFOBJ_NULL:
            if (m_File.AppendString(FX_BSTRC(" null")) < 0) {
                return -1;
            }
            m_Offset += 5;
            break;
        case PDFOBJ_BOOLEAN:
        case PDFOBJ_NUMBER:
            if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                return -1;
            }
            if ((len = m_File.AppendString(pObj->GetString())) < 0) {
                return -1;
            }
            m_Offset += len + 1;
            break;
        case PDFOBJ_STRING: {
                CFX_ByteString str = pObj->GetString();
                FX_BOOL bHex = ((CPDF_String*)pObj)->IsHex();
                if (crypto_handler_ == NULL || !bEncrypt) {
                    CFX_ByteString content = PDF_EncodeString(str, bHex);
                    if ((len = m_File.AppendString(content)) < 0) {
                        return -1;
                    }
                    m_Offset += len;
                    break;
                }
                CPDF_Encryptor encryptor;
                FX_DWORD gennum = GetObjectVersion(objnum);
                encryptor.Initialize(crypto_handler_, objnum, gennum, (FX_LPBYTE)(FX_LPCSTR)str, str.GetLength());
                CFX_ByteString content = PDF_EncodeString(CFX_ByteString((FX_LPCSTR)encryptor.m_pData, encryptor.m_dwSize), bHex);
                if ((len = m_File.AppendString(content)) < 0) {
                    return -1;
                }
                m_Offset += len;
                break;
            }
        case PDFOBJ_STREAM: {
                CPDF_FlateEncoder encoder;
                encoder.Initialize((CPDF_Stream*)pObj, m_bCompress, m_pDocument->m_bMetaDataFlate, m_bStandardSecurity, pObj == m_pMetadata , m_bEncryptMetadata);
                CPDF_Encryptor encryptor;
                CPDF_CryptoHandler* pHandler = crypto_handler_;
                FX_DWORD gennum = GetObjectVersion(objnum);
                encryptor.Initialize(pHandler, objnum, gennum, encoder.m_pData, encoder.m_dwSize);
                if ((FX_DWORD)encoder.m_pDict->GetInteger(FX_BSTRC("Length")) != encryptor.m_dwSize) {
                    encoder.CloneDict();
                    encoder.m_pDict->SetAtInteger(FX_BSTRC("Length"), encryptor.m_dwSize);
                }
                if (WriteDirectObj(objnum, encoder.m_pDict) < 0) {
                    return -1;
                }
                if ((len = m_File.AppendString(FX_BSTRC("stream\r\n"))) < 0) {
                    return -1;
                }
                m_Offset += len;
                if (m_File.AppendBlock(encryptor.m_pData, encryptor.m_dwSize) < 0) {
                    return -1;
                }
                m_Offset += encryptor.m_dwSize;
                if ((len = m_File.AppendString(FX_BSTRC("\r\nendstream"))) < 0) {
                    return -1;
                }
                m_Offset += len;
                break;
            }
        case PDFOBJ_NAME: {
                if (m_File.AppendString(FX_BSTRC("/")) < 0) {
                    return -1;
                }
                CFX_ByteString str = pObj->GetString();
                if ((len = m_File.AppendString(PDF_NameEncode(str))) < 0) {
                    return -1;
                }
                m_Offset += len + 1;
                break;
            }
        case PDFOBJ_REFERENCE: {
                if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                CPDF_Reference* p = (CPDF_Reference*)pObj;
                FX_DWORD dwRefObjNum = p->GetRefObjNum();
                if ((len = m_File.AppendDWord(dwRefObjNum)) < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                FX_INT32 wRefObjVersion = p->GetRefObjVersion();
                if ((len += m_File.AppendDWord(wRefObjVersion)) < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(" R")) < 0) {
                    return -1;
                }
                m_Offset += len + 4;
                break;
            }
        case PDFOBJ_ARRAY: {
                if (m_File.AppendString(FX_BSTRC("[")) < 0) {
                    return -1;
                }
                m_Offset += 1;
                CPDF_Array* p = (CPDF_Array*)pObj;
                for (FX_DWORD i = 0; i < p->GetCount(); i ++) {
                    CPDF_Object* pElement = p->GetElement(i);
                    if (pElement->GetObjNum()) {
                        if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_DWORD dwElementObjNum = pElement->GetObjNum();
                        if ((len = m_File.AppendDWord(dwElementObjNum)) < 0) {
                            return -1;
                        }
                        if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_WORD wElementObjVersion = GetObjectVersion(dwElementObjNum);
                        if ((len += m_File.AppendDWord(wElementObjVersion)) < 0) {
                            return -1;
                        }
                        if (m_File.AppendString(FX_BSTRC(" R")) < 0) {
                            return -1;
                        }
                        m_Offset += len + 4;
                    } else {
                        if (WriteDirectObj(objnum, pElement) < 0) {
                            return -1;
                        }
                    }
                }
                if (m_File.AppendString(FX_BSTRC("]")) < 0) {
                    return -1;
                }
                m_Offset += 1;
                break;
            }
        case PDFOBJ_DICTIONARY: {
                if (crypto_handler_ == NULL || pObj == m_pEncryptDict) {
                    return PDF_CreatorAppendObject(this, pObj, &m_File, m_Offset, m_pParser ? &m_pParser->m_ObjVersion : NULL);
                }
                if (m_File.AppendString(FX_BSTRC("<<")) < 0) {
                    return -1;
                }
                m_Offset += 2;
                CPDF_Dictionary* p = (CPDF_Dictionary*)pObj;
                FX_BOOL bSignDict = IsSignatureDict(p, 0, NULL);
                FX_POSITION pos = p->GetStartPos();
                while (pos) {
                    FX_BOOL bSignValue = FALSE;
                    CFX_ByteString key;
                    CPDF_Object* pValue = p->GetNextElement(pos, key);
                    if (m_File.AppendString(FX_BSTRC("/")) < 0) {
                        return -1;
                    }
                    if ((len = m_File.AppendString(PDF_NameEncode(key))) < 0) {
                        return -1;
                    }
                    m_Offset += len + 1;
                    if (bSignDict && key == FX_BSTRC("Contents")) {
                        bSignValue = TRUE;
                    }
                    if (pValue->GetObjNum()) {
                        if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_DWORD dwValueObjNum = pValue->GetObjNum();
                        if ((len = m_File.AppendDWord(dwValueObjNum)) < 0) {
                            return -1;
                        }
                        if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                            return -1;
                        }
                        FX_WORD wValueObjVersion = GetObjectVersion(dwValueObjNum);
                        if ((len += m_File.AppendDWord(wValueObjVersion)) < 0) {
                            return -1;
                        }
                        if (m_File.AppendString(FX_BSTRC(" R ")) < 0) {
                            return -1;
                        }
                        m_Offset += len + 5;
                    } else {
                        if (WriteDirectObj(objnum, pValue, !bSignValue) < 0) {
                            return -1;
                        }
                    }
                }
                if (m_File.AppendString(FX_BSTRC(">>")) < 0) {
                    return -1;
                }
                m_Offset += 2;
                break;
            }
    }
    return 1;
}
FX_INT32 CPDF_Creator::WriteOldIndirectObject(FX_DWORD objnum)
{
    if(m_pParser->m_V5Type[objnum] == 0 || m_pParser->m_V5Type[objnum] == 255) {
        return 0;
    }
    FX_LPVOID valuetemp = NULL;
    FX_BOOL bExistInMap = m_pDocument->m_IndirectObjs.Lookup((FX_LPVOID)(FX_UINTPTR)objnum, valuetemp);
    m_ObjectOffset[objnum] = m_Offset;
    m_MaxOffset = m_MaxOffset < m_Offset ? m_Offset : m_MaxOffset;
    FX_BOOL bObjStm = (m_pParser->m_V5Type[objnum] == 2) && m_pEncryptDict && !m_pXRefStream;
    if(m_pParser->m_bVersionUpdated || m_bSecurityChanged || bExistInMap || bObjStm || m_pParser->m_bRebuildXRef) {
        CPDF_Object* pObj = m_pDocument->GetIndirectObject(objnum);
        if (pObj == NULL) {
            m_ObjectOffset[objnum] = 0;
            m_ObjectSize[objnum] = 0;
            return 0;
        }
        if (WriteIndirectObj(pObj)) {
            return -1;
        }
        if (!bExistInMap) {
            m_pDocument->ReleaseIndirectObject(objnum);
        }
    } else {
        FX_BYTE* pBuffer = NULL;
        if (m_pParser->m_V5Type[objnum] == 2) {
            int ret = 0;
            FX_DWORD size;
            m_pParser->GetIndirectBinary(objnum, pBuffer, size);
            if (pBuffer == NULL) {
                return 0;
            }
            if (m_pXRefStream && (ret = WriteIndirectObjectToStream(objnum, pBuffer, size)) <= 0) {
                if (ret < 0) {
                    FX_Free(pBuffer);
                    return -1;
                }
            } else {
                FX_INT32 len = m_File.AppendDWord(objnum);
                if (len < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                FX_WORD objVersion = GetObjectVersion(objnum);
                if ((len += m_File.AppendDWord(objVersion)) < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(" obj ")) < 0) {
                    return -1;
                }
                m_Offset += len + 6;
                if (m_File.AppendBlock(pBuffer, size) < 0) {
                    return -1;
                }
                m_Offset += size;
                if (m_File.AppendString(FX_BSTRC("\r\nendobj\r\n")) < 0) {
                    return -1;
                }
                m_Offset += 10;
            }
        } else {
            FX_DWORD buffSize = 0, leftSize = 0;
            FX_FILESIZE posSave = 0;
            while(!pBuffer || leftSize) {
                FX_BOOL bRet = m_pParser->GetIndirectBinary(objnum, pBuffer, buffSize, leftSize, posSave);
                if (pBuffer == NULL || !bRet) {
                    return 0;
                }
                if (m_File.AppendBlock(pBuffer, buffSize) < 0) {
                    FX_Free(pBuffer);
                    return -1;
                }
                m_Offset += buffSize;
            }
            if(AppendObjectNumberToXRef(objnum) < 0) {
                FX_Free(pBuffer);
                return -1;
            }
        }
        FX_Free(pBuffer);
    }
    return 1;
}
FX_INT32 CPDF_Creator::WriteOldObjs(IFX_Pause *pPause)
{
    FX_DWORD nOldSize = m_pParser->m_CrossRef.GetSize();
    FX_DWORD objnum = (FX_DWORD)(FX_UINTPTR)m_Pos;
    for(; objnum < nOldSize; objnum ++) {
        FX_INT32 iRet = WriteOldIndirectObject(objnum);
        if (!iRet) {
            continue;
        }
        if (iRet < 0) {
            return iRet;
        }
        m_ObjectSize[objnum] = (FX_DWORD)(m_Offset - m_ObjectOffset[objnum]);
        if (pPause && pPause->NeedToPauseNow()) {
            m_Pos = (FX_LPVOID)(FX_UINTPTR)(objnum + 1);
            return 1;
        }
    }
    return 0;
}
FX_INT32 CPDF_Creator::WriteNewObjs(FX_BOOL bIncremental, IFX_Pause *pPause)
{
  FX_UNREFERENCED_PARAMETER(bIncremental);
    FX_INT32 iCount = m_NewObjNumArray.GetSize();
    FX_INT32 index = (FX_INT32)(FX_UINTPTR)m_Pos;
    while (index < iCount) {
        FX_DWORD objnum = m_NewObjNumArray.ElementAt(index);
        CPDF_Object *pObj = NULL;
        m_pDocument->m_IndirectObjs.Lookup((FX_LPVOID)(FX_UINTPTR)objnum, (FX_LPVOID&)pObj);
        if (NULL == pObj) {
            ++index;
            continue;
        }
        m_ObjectOffset[objnum] = m_Offset;
        if (WriteIndirectObj(pObj)) {
            return -1;
        }
        m_ObjectSize[objnum] = (FX_DWORD)(m_Offset - m_ObjectOffset[objnum]);
        index++;
        if (pPause && pPause->NeedToPauseNow()) {
            m_Pos = (FX_POSITION)(FX_UINTPTR)index;
            return 1;
        }
    }
    return 0;
}
void CPDF_Creator::InitOldObjNumOffsets()
{
    if (!m_pParser) {
        return;
    }
    FX_DWORD dwStart = 0;
    FX_DWORD dwEnd = m_pParser->GetLastObjNum();
    while (dwStart <= dwEnd) {
        while (dwStart <= dwEnd && ((m_pParser->m_V5Type[dwStart] == 0 || m_pParser->m_V5Type[dwStart] == 255))) {
            dwStart++;
        }
        if (dwStart > dwEnd) {
            break;
        }
        FX_DWORD j = dwStart;
        while (j <= dwEnd && m_pParser->m_V5Type[j] != 0 && m_pParser->m_V5Type[j] != 255) {
            j++;
        }
        m_ObjectOffset.Add(dwStart, j - dwStart);
        m_ObjectSize.Add(dwStart, j - dwStart);
        dwStart = j;
    }
}
void CPDF_Creator::InitNewObjNumOffsets()
{
    FX_BOOL bIncremental = (m_dwFlags & FPDFCREATE_INCREMENTAL) != 0;
    FX_BOOL bNoOriginal = (m_dwFlags & FPDFCREATE_NO_ORIGINAL) != 0;
    FX_DWORD nOldSize = m_pParser ? m_pParser->m_CrossRef.GetSize() : 0;
    FX_POSITION pos = m_pDocument->m_IndirectObjs.GetStartPosition();
    while (pos) {
        size_t key = 0;
        CPDF_Object* pObj;
        m_pDocument->m_IndirectObjs.GetNextAssoc(pos, (FX_LPVOID&)key, (FX_LPVOID&)pObj);
        FX_DWORD objnum = (FX_DWORD)key;
        if (pObj->GetObjNum() == -1) {
            continue;
        }
        if (bIncremental) {
            if (!pObj->IsModified()) {
                continue;
            }
        } else {
            if (objnum < nOldSize && m_pParser && m_pParser->m_V5Type[objnum] != 0) {
                continue;
            }
        }
        AppendNewObjNum(objnum);
    }
    FX_INT32 iCount = m_NewObjNumArray.GetSize();
    if (iCount == 0) {
        return;
    }
    FX_INT32 i = 0;
    FX_DWORD dwStartObjNum = 0;
    FX_BOOL bCrossRefValid = m_pParser && m_pParser->GetLastXRefOffset() > 0;
    while (i < iCount) {
        dwStartObjNum = m_NewObjNumArray.ElementAt(i);
        if ((bIncremental && (bNoOriginal || bCrossRefValid)) || !m_ObjectOffset.GetPtrAt(dwStartObjNum)) {
            break;
        }
        i++;
    }
    if (i >= iCount) {
        return;
    }
    FX_DWORD dwLastObjNum = dwStartObjNum;
    i++;
    FX_BOOL bNewStart = FALSE;
    for (; i < iCount; i++) {
        FX_DWORD dwCurObjNum = m_NewObjNumArray.ElementAt(i);
        FX_BOOL bExist = (dwCurObjNum < nOldSize && m_ObjectOffset.GetPtrAt(dwCurObjNum) != NULL);
        if (bExist || dwCurObjNum - dwLastObjNum > 1) {
            if (!bNewStart) {
                m_ObjectOffset.Add(dwStartObjNum, dwLastObjNum - dwStartObjNum + 1);
                m_ObjectSize.Add(dwStartObjNum, dwLastObjNum - dwStartObjNum + 1);
            }
            dwStartObjNum = dwCurObjNum;
        }
        if (bNewStart) {
            dwStartObjNum = dwCurObjNum;
        }
        bNewStart = bExist;
        dwLastObjNum = dwCurObjNum;
    }
    m_ObjectOffset.Add(dwStartObjNum, dwLastObjNum - dwStartObjNum + 1);
    m_ObjectSize.Add(dwStartObjNum, dwLastObjNum - dwStartObjNum + 1);
}
void CPDF_Creator::AppendNewObjNum(FX_DWORD objbum)
{
    FX_INT32 iStart = 0, iFind = 0;
    FX_INT32 iEnd = m_NewObjNumArray.GetUpperBound();
    while (iStart <= iEnd) {
        FX_INT32 iMid = (iStart + iEnd) / 2;
        FX_DWORD dwMid = m_NewObjNumArray.ElementAt(iMid);
        if (objbum < dwMid) {
            iEnd = iMid - 1;
        } else {
            if (iMid == iEnd) {
                iFind = iMid + 1;
                break;
            }
            FX_DWORD dwNext = m_NewObjNumArray.ElementAt(iMid + 1);
            if (objbum < dwNext) {
                iFind = iMid + 1;
                break;
            }
            iStart = iMid + 1;
        }
    }
    m_NewObjNumArray.InsertAt(iFind, objbum);
}
FX_INT32 CPDF_Creator::WriteDoc_Stage1(IFX_Pause *pPause)
{
    FXSYS_assert(m_iStage > -1 || m_iStage < 20);
    if (m_iStage == 0) {
        if (m_pParser == NULL) {
            m_dwFlags &= ~FPDFCREATE_INCREMENTAL;
        }
        if (m_bSecurityChanged && (m_dwFlags & FPDFCREATE_NO_ORIGINAL) == 0) {
            m_dwFlags &= ~FPDFCREATE_INCREMENTAL;
        }
        CPDF_Dictionary* pDict = m_pDocument->GetRoot();
        m_pMetadata = pDict ? pDict->GetElementValue(FX_BSTRC("Metadata")) : NULL;
        if (m_dwFlags & FPDFCREATE_OBJECTSTREAM) {
            m_pXRefStream = FX_NEW CPDF_XRefStream;
            m_pXRefStream->Start();
            if ((m_dwFlags & FPDFCREATE_INCREMENTAL) != 0 && m_pParser) {
                FX_FILESIZE prev = m_pParser->GetLastXRefOffset();
                m_pXRefStream->m_PrevOffset = prev;
            }
        }
        m_iStage = 10;
    }
    if (m_iStage == 10) {
        if ((m_dwFlags & FPDFCREATE_INCREMENTAL) == 0) {
            if (m_File.AppendString(FX_BSTRC("%PDF-")) < 0) {
                return -1;
            }
            m_Offset += 5;
            int version = m_pDocument->m_FileVersion;
            if (version == 0)	{
                version = 17;
            }
            CFX_ByteString	bsVersion = GenerateFileVersion(version);
            int len = m_File.AppendString(FX_BSTR(bsVersion));
            if (len < 0) {
                return -1;
            }
            m_Offset += len;
            if ((len = m_File.AppendString(FX_BSTRC("\r\n%\xA1\xB3\xC5\xD7\r\n"))) < 0) {
                return -1;
            }
            m_Offset += len;
            InitOldObjNumOffsets();
            m_iStage = 20;
        } else {
            IFX_FileRead* pSrcFile = m_pParser->GetFileAccess();
            m_Offset = pSrcFile->GetSize();
            m_FilePos = m_Offset;
            m_iStage = 15;
        }
    }
    if (m_iStage == 15) {
        if ((m_dwFlags & FPDFCREATE_NO_ORIGINAL) == 0 && m_FilePos > 0) {
            IFX_FileRead* pSrcFile = m_pParser->GetFileAccess();
            FX_BYTE buffer[4096];
            FX_FILESIZE src_size = m_FilePos;
            while (src_size) {
                FX_DWORD block_size = src_size > 4096 ? 4096 : src_size;
                if (!pSrcFile->ReadBlock(buffer, m_Offset - src_size, block_size)) {
                    return -1;
                }
                if (m_File.AppendBlock(buffer, block_size) < 0) {
                    return -1;
                }
                src_size -= block_size;
                if (pPause && pPause->NeedToPauseNow()) {
                    m_FilePos = src_size;
                    return 1;
                }
            }
            m_Offset -= m_pParser->m_Syntax.GetHeaderOffset();
            if (m_Offset <= 0) {
                return -1;
            }
        }
        if ((m_dwFlags & FPDFCREATE_NO_ORIGINAL) == 0 && m_pParser->GetLastXRefOffset() == 0) {
            InitOldObjNumOffsets();
            FX_DWORD dwEnd = m_pParser->GetLastObjNum();
            FX_BOOL bObjStm = (m_dwFlags & FPDFCREATE_OBJECTSTREAM) != 0;
            for (FX_DWORD objnum = 0; objnum <= dwEnd; objnum++) {
                if (m_pParser->m_V5Type[objnum] == 0 || m_pParser->m_V5Type[objnum] == 255) {
                    continue;
                }
                m_ObjectOffset[objnum] = m_pParser->m_CrossRef[objnum];
                if (bObjStm) {
                    m_pXRefStream->AddObjectNumberToIndexArray(objnum);
                }
            }
            if (bObjStm) {
                m_pXRefStream->EndXRefStream(this);
                m_pXRefStream->Start();
            }
        }
        m_iStage = 20;
    }
    InitNewObjNumOffsets();
    return m_iStage;
}
FX_INT32 CPDF_Creator::WriteDoc_Stage2(IFX_Pause *pPause)
{
    FXSYS_assert(m_iStage >= 20 || m_iStage < 30);
    if (m_iStage == 20) {
        if ((m_dwFlags & FPDFCREATE_INCREMENTAL) == 0 && m_pParser) {
            m_Pos = (FX_LPVOID)(FX_UINTPTR)0;
            m_iStage = 21;
        } else {
            m_iStage = 25;
        }
    }
    if (m_iStage == 21) {
        FX_INT32 iRet = WriteOldObjs(pPause);
        if (iRet) {
            return iRet;
        }
        m_iStage = 25;
    }
    if (m_iStage == 25) {
        m_Pos = (FX_LPVOID)(FX_UINTPTR)0;
        m_iStage = 26;
    }
    if (m_iStage == 26) {
        FX_INT32 iRet = WriteNewObjs((m_dwFlags & FPDFCREATE_INCREMENTAL) != 0, pPause);
        if (iRet) {
            return iRet;
        }
        m_iStage = 27;
    }
    if (m_iStage == 27) {
        if (NULL != m_pEncryptDict && 0 == m_pEncryptDict->GetObjNum()) {
            m_dwLastObjNum += 1;
            FX_FILESIZE saveOffset = m_Offset;
            if (WriteIndirectObj(m_dwLastObjNum, m_pEncryptDict) < 0) {
                return -1;
            }
            m_ObjectOffset.Add(m_dwLastObjNum, 1);
            m_ObjectOffset[m_dwLastObjNum] = saveOffset;
            m_ObjectSize.Add(m_dwLastObjNum, 1);
            m_ObjectSize[m_dwLastObjNum] = m_Offset - saveOffset;
            m_dwEnryptObjNum = m_dwLastObjNum;
            if (m_dwFlags & FPDFCREATE_INCREMENTAL) {
                m_NewObjNumArray.Add(m_dwLastObjNum);
            }
        }
        m_iStage = 80;
    }
    return m_iStage;
}
FX_INT32 CPDF_Creator::WriteDoc_Stage3(IFX_Pause *pPause)
{
    FXSYS_assert(m_iStage >= 80 || m_iStage < 90);
    FX_DWORD dwLastObjNum = m_dwLastObjNum;
    if (m_iStage == 80) {
        m_XrefStart = m_Offset;
        if (m_dwFlags & FPDFCREATE_OBJECTSTREAM) {
            m_pXRefStream->End(this, TRUE);
            m_XrefStart = m_pXRefStream->m_PrevOffset;
            m_iStage = 90;
        } else if ((m_dwFlags & FPDFCREATE_INCREMENTAL) == 0 || !m_pParser->IsXRefStream()) {
            if (m_File.AppendString(FX_BSTRC("xref\r\n")) < 0) {
                return -1;
            }
            m_Pos = (FX_LPVOID)(FX_UINTPTR)0;
            if ((m_dwFlags & FPDFCREATE_INCREMENTAL) == 0 || m_pParser->GetLastXRefOffset() == 0) {
                m_iStage = 81;
            } else {
                m_iStage = 82;
            }
        } else {
            m_iStage = 90;
        }
    }
    if (m_iStage == 81) {
        CFX_ByteString str;
        FX_DWORD i = (FX_DWORD)(FX_UINTPTR)m_Pos;
        if (i == 0) {
            str.Format("0 %d\r\n0000000000 65535 f\r\n", dwLastObjNum + 1);
            if (m_File.AppendBlock((FX_LPCSTR)str, str.GetLength()) < 0) {
                return -1;
            }
            i++;
        }
        while (i <= dwLastObjNum) {
            FX_FILESIZE sFileSize = m_ObjectOffset.GetPtrAt(i) ?  m_ObjectOffset[i] : 0;
            if (sFileSize == 0) {
                str.Format("0000000000 00000 f\r\n");
            } else if (sFileSize > 0xFFFFFFFFL) {
                FX_LPSTR lpBuffer = str.GetBuffer(20);
                FXSYS_i64toa(sFileSize, lpBuffer, 10);
                str.ReleaseBuffer();
                CFX_ByteString strGeneration;
                strGeneration.Format(" %05d", GetObjectVersion(i));
                str += strGeneration;
                str += " n\r\n";
            } else {
                str.Format("%010u", sFileSize);
                CFX_ByteString strGeneration;
                strGeneration.Format(" %05d", GetObjectVersion(i));
                str += strGeneration;
                str += " n\r\n";
            }
            if (m_File.AppendBlock((FX_LPCSTR)str, str.GetLength()) < 0) {
                return -1;
            }
            if (i++ > dwLastObjNum) {
                break;
            }
            if (pPause && pPause->NeedToPauseNow()) {
                m_Pos = (FX_LPVOID)(FX_UINTPTR)i;
                return 1;
            }
        }
        m_iStage = 90;
    }
    if (m_iStage == 82) {
        CFX_ByteString str;
        FX_INT32 iCount = m_NewObjNumArray.GetSize();
        if (iCount == 0) {
            if (m_File.AppendString(FX_BSTRC("0 0\r\n")) < 0) {
                return -1;
            }
        } else {
            FX_INT32 i = (FX_INT32)(FX_UINTPTR)m_Pos;
            while (i < iCount) {
                FX_INT32 j = i;
                FX_DWORD objnum = m_NewObjNumArray.ElementAt(i);
                while (j < iCount) {
                    if (++j == iCount) {
                        break;
                    }
                    FX_DWORD dwCurrent = m_NewObjNumArray.ElementAt(j);
                    if (dwCurrent - objnum > 1) {
                        break;
                    }
                    objnum = dwCurrent;
                }
                objnum = m_NewObjNumArray.ElementAt(i);
                if (objnum == 1) {
                    str.Format("0 %d\r\n0000000000 65535 f\r\n", j - i + 1);
                } else {
                    str.Format("%d %d\r\n", objnum, j - i);
                }
                if (m_File.AppendBlock((FX_LPCSTR)str, str.GetLength()) < 0) {
                    return -1;
                }
                while (i < j) {
                    objnum = m_NewObjNumArray.ElementAt(i++);
                    FX_FILESIZE sFileSize = m_ObjectOffset[objnum];
                    if (sFileSize > 0xFFFFFFFFL) {
                        FX_LPSTR lpBuffer = str.GetBuffer(20);
                        FXSYS_i64toa(sFileSize, lpBuffer, 10);
                        str.ReleaseBuffer();
                        str += " 00000 n\r\n";
                    } else {
                        str.Format("%010u 00000 n\r\n", sFileSize);
                    }
                    if (m_File.AppendBlock((FX_LPCSTR)str, str.GetLength()) < 0) {
                        return -1;
                    }
                }
                if (pPause && (i % 100) == 0 && pPause->NeedToPauseNow()) {
                    m_Pos = (FX_LPVOID)(FX_UINTPTR)i;
                    return 1;
                }
            }
        }
        m_iStage = 90;
    }
    return m_iStage;
}
static FX_INT32 _OutPutIndex(CFX_FileBufferArchive* pFile, FX_FILESIZE offset, FX_BOOL bNeed8Bytes = FALSE)
{
  FX_UNREFERENCED_PARAMETER(bNeed8Bytes);
    FXSYS_assert(pFile);
    FX_INT32 sizeOfOffset = sizeof(offset);
    if (sizeOfOffset > 4 && bNeed8Bytes) {
        if (pFile->AppendByte(FX_GETBYTEOFFSET56(offset)) < 0) {
            return -1;
        }
        if (pFile->AppendByte(FX_GETBYTEOFFSET48(offset)) < 0) {
            return -1;
        }
        if (pFile->AppendByte(FX_GETBYTEOFFSET40(offset)) < 0) {
            return -1;
        }
        if (pFile->AppendByte(FX_GETBYTEOFFSET32(offset)) < 0) {
            return -1;
        }
    }
    if (pFile->AppendByte(FX_GETBYTEOFFSET24(offset)) < 0) {
        return -1;
    }
    if (pFile->AppendByte(FX_GETBYTEOFFSET16(offset)) < 0) {
        return -1;
    }
    if (pFile->AppendByte(FX_GETBYTEOFFSET8(offset)) < 0) {
        return -1;
    }
    if (pFile->AppendByte(FX_GETBYTEOFFSET0(offset)) < 0) {
        return -1;
    }
    if (pFile->AppendByte(0) < 0) {
        return -1;
    }
    return 0;
}
FX_INT32 CPDF_Creator::WriteDoc_Stage4(IFX_Pause *pPause)
{
  FX_UNREFERENCED_PARAMETER(pPause);
    FXSYS_assert(m_iStage >= 90);
    if ((m_dwFlags & FPDFCREATE_OBJECTSTREAM) == 0) {
        FX_BOOL bXRefStream = (m_dwFlags & FPDFCREATE_INCREMENTAL) != 0 && m_pParser->IsXRefStream();
        if (!bXRefStream) {
            if (m_File.AppendString(FX_BSTRC("trailer\r\n<<")) < 0) {
                return -1;
            }
        } else {
            if (m_File.AppendDWord(m_pDocument->m_LastObjNum + 1) < 0) {
                return -1;
            }
            if (m_File.AppendString(FX_BSTRC(" 0 obj\r\n<<")) < 0) {
                return -1;
            }
        }
        if (m_pParser) {
            CPDF_Dictionary* p = m_pParser->m_pTrailer;
            FX_POSITION pos = p->GetStartPos();
            while (pos) {
                CFX_ByteString key;
                CPDF_Object* pValue = p->GetNextElement(pos, key);
                if (key == FX_BSTRC("Encrypt") || key == FX_BSTRC("Size") || key == FX_BSTRC("Filter") ||
                        key == FX_BSTRC("Index") || key == FX_BSTRC("Length") || key == FX_BSTRC("Prev") ||
                        key == FX_BSTRC("W") || key == FX_BSTRC("XRefStm") || key == FX_BSTRC("ID")) {
                    continue;
                }
                if (m_File.AppendString((FX_BSTRC("/"))) < 0) {
                    return -1;
                }
                if (m_File.AppendString(PDF_NameEncode(key)) < 0) {
                    return -1;
                }
                if (pValue->GetObjNum()) {
                    if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                        return -1;
                    }
                    FX_DWORD dwValueObjNum = pValue->GetObjNum();
                    if (m_File.AppendDWord(dwValueObjNum) < 0) {
                        return -1;
                    }
                    if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                        return -1;
                    }
                    FX_WORD wValueObjVersion = GetObjectVersion(dwValueObjNum);
                    if (m_File.AppendDWord(wValueObjVersion) < 0) {
                        return -1;
                    }
                    if (m_File.AppendString(FX_BSTRC(" R ")) < 0) {
                        return -1;
                    }
                } else {
                    FX_FILESIZE offset = 0;
                    if (PDF_CreatorAppendObject(this, pValue, &m_File, offset, NULL) < 0) {
                        return -1;
                    }
                }
            }
        } else {
            if (m_File.AppendString(FX_BSTRC("\r\n/Root ")) < 0) {
                return -1;
            }
            FX_DWORD dwRootObjNum = m_pDocument->m_pRootDict->GetObjNum();
            if (m_File.AppendDWord(dwRootObjNum) < 0) {
                return -1;
            }
            if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                return -1;
            }
            FX_WORD wRootObjVersion = GetObjectVersion(dwRootObjNum);
            if (m_File.AppendDWord(wRootObjVersion) < 0) {
                return -1;
            }
            if (m_File.AppendString(FX_BSTRC(" R\r\n")) < 0) {
                return -1;
            }
            if (bXRefStream)
                if (m_File.AppendString(FX_BSTRC("/Type /XRef ")) < 0) {
                    return -1;
                }
            if (m_pDocument->m_pInfoDict) {
                if (m_File.AppendString(FX_BSTRC("/Info ")) < 0) {
                    return -1;
                }
                FX_DWORD dwInfoObjNum = m_pDocument->m_pInfoDict->GetObjNum();
                if (m_File.AppendDWord(dwInfoObjNum) < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                    return -1;
                }
                FX_WORD wInfoObjVersion = GetObjectVersion(dwInfoObjNum);
                if (m_File.AppendDWord(wInfoObjVersion) < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(" R\r\n")) < 0) {
                    return -1;
                }
            }
        }
        if (m_pEncryptDict) {
            if (m_File.AppendString(FX_BSTRC("/Encrypt")) < 0) {
                return -1;
            }
            FX_DWORD dwObjNum = m_pEncryptDict->GetObjNum();
            if (dwObjNum == 0) {
                dwObjNum = m_pDocument->GetLastObjNum() + 1;
            }
            if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                return -1;
            }
            if (m_File.AppendDWord(dwObjNum) < 0) {
                return -1;
            }
            if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
                return -1;
            }
            FX_WORD wEncryptVersion = GetObjectVersion(dwObjNum);
            if (m_File.AppendDWord(wEncryptVersion) < 0) {
                return -1;
            }
            if (m_File.AppendString(FX_BSTRC(" R ")) < 0) {
                return -1;
            }
        }
        FX_DWORD dwLastObjNum = m_dwLastObjNum;
        if (m_File.AppendString(FX_BSTRC("/Size ")) < 0) {
            return -1;
        }
        if (m_File.AppendDWord(dwLastObjNum + (bXRefStream ? 2 : 1)) < 0) {
            return -1;
        }
        if ((m_dwFlags & FPDFCREATE_INCREMENTAL) != 0) {
            FX_FILESIZE prev = m_pParser->IsParsingLinearized() ? m_pParser->GetLinearizedFirstPageXRefOffset() : m_pParser->GetLastXRefOffset();
            if (prev < 0) {
                prev = 0;
            }
            if (m_File.AppendString(FX_BSTRC("/Prev ")) < 0) {
                return -1;
            }
            FX_CHAR offset_buf[20];
            FXSYS_memset32(offset_buf, 0, sizeof(offset_buf));
            FXSYS_i64toa(prev, offset_buf, 10);
            if (m_File.AppendBlock(offset_buf, FXSYS_strlen(offset_buf)) < 0) {
                return -1;
            }
        }
        if (m_pIDArray) {
            if (m_File.AppendString((FX_BSTRC("/ID"))) < 0) {
                return -1;
            }
            FX_FILESIZE offset = 0;
            if (PDF_CreatorAppendObject(this, m_pIDArray, &m_File, offset, NULL) < 0) {
                return -1;
            }
        }
        if (!bXRefStream) {
            if (m_File.AppendString(FX_BSTRC(">>")) < 0) {
                return -1;
            }
        } else {
            if(m_Offset > 0xFFFFFFFFUL) {
                if (m_File.AppendString(FX_BSTRC("/W[0 8 1]/Index[")) < 0) {
                    return -1;
                }
            } else {
                if (m_File.AppendString(FX_BSTRC("/W[0 4 1]/Index[")) < 0) {
                    return -1;
                }
            }
            if ((m_dwFlags & FPDFCREATE_INCREMENTAL) != 0 && m_pParser && m_pParser->GetLastXRefOffset() == 0) {
                FX_DWORD i = 0;
                for (i = 0; i < dwLastObjNum; i++) {
                    if (!m_ObjectOffset.GetPtrAt(i)) {
                        continue;
                    }
                    if (m_File.AppendDWord(i) < 0) {
                        return -1;
                    }
                    if (m_File.AppendString(FX_BSTRC(" 1 ")) < 0) {
                        return -1;
                    }
                }
                if (m_File.AppendString(FX_BSTRC("]/Length ")) < 0) {
                    return -1;
                }
                if (m_File.AppendDWord(dwLastObjNum * 5) < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(">>stream\r\n")) < 0) {
                    return -1;
                }
                for (i = 0; i < dwLastObjNum; i++) {
                    FX_FILESIZE* offset = m_ObjectOffset.GetPtrAt(i);
                    if (!offset) {
                        continue;
                    }
                    _OutPutIndex(&m_File, *offset, m_Offset > 0xFFFFFFFFUL);
                }
            } else {
                int count = m_NewObjNumArray.GetSize();
                FX_INT32 i = 0;
                for (i = 0; i < count; i++) {
                    FX_DWORD objnum = m_NewObjNumArray.ElementAt(i);
                    if (m_File.AppendDWord(objnum) < 0) {
                        return -1;
                    }
                    if (m_File.AppendString(FX_BSTRC(" 1 ")) < 0) {
                        return -1;
                    }
                }
                if (m_File.AppendString(FX_BSTRC("]/Length ")) < 0) {
                    return -1;
                }
                if (m_File.AppendDWord(count * 5) < 0) {
                    return -1;
                }
                if (m_File.AppendString(FX_BSTRC(">>stream\r\n")) < 0) {
                    return -1;
                }
                for (i = 0; i < count; i++) {
                    FX_DWORD objnum = m_NewObjNumArray.ElementAt(i);
                    FX_FILESIZE offset = m_ObjectOffset[objnum];
                    _OutPutIndex(&m_File, offset, m_Offset > 0xFFFFFFFFUL);
                }
            }
            if (m_File.AppendString(FX_BSTRC("\r\nendstream\r\nendobj")) < 0) {
                return -1;
            }
        }
    }
    if (m_File.AppendString(FX_BSTRC("\r\nstartxref\r\n")) < 0) {
        return -1;
    }
    FX_CHAR offset_buf[20];
    FXSYS_memset32(offset_buf, 0, sizeof(offset_buf));
    FXSYS_i64toa(m_XrefStart, offset_buf, 10);
    if (m_File.AppendBlock(offset_buf, FXSYS_strlen(offset_buf)) < 0) {
        return -1;
    }
    if (m_File.AppendString(FX_BSTRC("\r\n%%EOF\r\n")) < 0) {
        return -1;
    }
    if (!m_File.Flush()) {
        return -1;
    }
    return m_iStage = 100;
}
void CPDF_Creator::Clear()
{
    if (m_pXRefStream) {
        delete m_pXRefStream;
        m_pXRefStream = NULL;
    }
    m_File.Clear();
    m_NewObjNumArray.RemoveAll();
    if (m_pIDArray) {
        m_pIDArray->Release();
        m_pIDArray = NULL;
    }
}
FX_BOOL CPDF_Creator::Create(FX_LPCSTR filename, FX_DWORD flags)
{
    if (!m_pDocument || !m_File.AttachFile(filename)) {
        return FALSE;
    }
    FX_BOOL bRet = Create(flags);
    if (!bRet || !(flags & FPDFCREATE_PROGRESSIVE)) {
        Clear();
    }
    return bRet;
}
FX_BOOL CPDF_Creator::Create(FX_LPCWSTR filename, FX_DWORD flags)
{
    if (!m_pDocument || !m_File.AttachFile(filename)) {
        return FALSE;
    }
    FX_BOOL bRet = Create(flags);
    if (!bRet || !(flags & FPDFCREATE_PROGRESSIVE)) {
        Clear();
    }
    return bRet;
}
FX_BOOL CPDF_Creator::Create(IFX_StreamWrite* pFile, FX_DWORD flags)
{
    if (!m_pDocument || !pFile) {
        return FALSE;
    }
    if (!m_File.AttachFile(pFile, FALSE)) {
        return FALSE;
    }
    return Create(flags);
}
FX_BOOL CPDF_Creator::Create(FX_DWORD flags)
{
    m_iStage = 0;
    m_Offset = 0;
    m_dwLastObjNum = m_pDocument->GetLastObjNum();
    m_ObjectOffset.Clear();
    m_ObjectSize.Clear();
    m_NewObjNumArray.RemoveAll();
    InitID();
    if (m_pParser && m_pParser->m_SortedOffset.GetSize() > 0) {
        CFX_FileSizeArray &sortedOffsets = m_pParser->m_SortedOffset;
        FX_FILESIZE maxOffset = sortedOffsets.GetAt(sortedOffsets.GetSize() - 1);
        unsigned char tempValue = 0x02;
        FX_FILESIZE limitSizeOfV4 = ((FX_FILESIZE)0x540be3ff) | (((FX_INT64)tempValue) << 32);
        FX_INT32 sizeOfFileSize = sizeof(FX_FILESIZE);
        if (sizeOfFileSize > 4 && maxOffset > limitSizeOfV4) {
            flags |= FPDFCREATE_OBJECTSTREAM;
        }
        FX_UNREFERENCED_PARAMETER(maxOffset);
        FX_UNREFERENCED_PARAMETER(limitSizeOfV4);
    }
    m_dwFlags = flags;
    if (flags & FPDFCREATE_PROGRESSIVE) {
        return TRUE;
    }
    return Continue(NULL) > -1;
}
void CPDF_Creator::InitID(FX_BOOL bDefault )
{
    CPDF_Array* pOldIDArray = m_pParser ? m_pParser->GetIDArray() : NULL;
    FX_BOOL bNewId = !m_pIDArray;
    if (!m_pIDArray) {
        FX_LPDWORD pBuffer = NULL;
        m_pIDArray = CPDF_Array::Create();
        CPDF_Object* pID1 = pOldIDArray ? pOldIDArray->GetElement(0) : NULL;
        if (pID1) {
            m_pIDArray->Add(pID1->Clone());
        } else {
            pBuffer = FX_Alloc(FX_DWORD, 4);
            PDF_GenerateFileID((FX_DWORD)(FX_UINTPTR)this, m_dwLastObjNum, pBuffer);
            CFX_ByteStringC bsBuffer((FX_LPCBYTE)pBuffer, 4 * sizeof(FX_DWORD));
            m_pIDArray->Add(CPDF_String::Create(bsBuffer, TRUE), m_pDocument);
        }
        if (pBuffer) {
            FX_Free(pBuffer);
        }
    }
    if (!bDefault) {
        return;
    }
    if (pOldIDArray) {
        CPDF_Object* pID2 = pOldIDArray->GetElement(1);
        if ((m_dwFlags & FPDFCREATE_INCREMENTAL) && m_pEncryptDict && pID2) {
            m_pIDArray->Add(pID2->Clone());
            return;
        }
        FX_LPDWORD pBuffer = FX_Alloc(FX_DWORD, 4);
        PDF_GenerateFileID((FX_DWORD)(FX_UINTPTR)this, m_dwLastObjNum, pBuffer);
        CFX_ByteStringC bsBuffer((FX_LPCBYTE)pBuffer, 4 * sizeof(FX_DWORD));
        m_pIDArray->Add(CPDF_String::Create(bsBuffer, TRUE), m_pDocument);
        FX_Free(pBuffer);
        return;
    }
    m_pIDArray->Add(m_pIDArray->GetElement(0)->Clone());
    if (m_pEncryptDict && !pOldIDArray && m_pParser && bNewId) {
        if (m_pEncryptDict->GetString(FX_BSTRC("Filter")) == FX_BSTRC("Standard")) {
            CPDF_StandardSecurityHandler handler;
            CFX_ByteString user_pass = m_pParser->GetPassword();
            FX_DWORD flag = PDF_ENCRYPT_CONTENT;
            handler.OnCreate(m_pEncryptDict, m_pIDArray, (FX_LPCBYTE)user_pass, user_pass.GetLength(), flag);
            if (crypto_handler_ && m_bNewCrypto) {
                delete crypto_handler_;
            }
            crypto_handler_ = FX_NEW CPDF_StandardCryptoHandler;
            crypto_handler_->Init(m_pEncryptDict, &handler);
            m_bNewCrypto = TRUE;
            m_bSecurityChanged = TRUE;
        }
    }
}
FX_INT32 CPDF_Creator::Continue(IFX_Pause *pPause)
{
    if (m_iStage < 0) {
        return m_iStage;
    }
    FX_INT32 iRet = 0;
    while (m_iStage < 100) {
        if (m_iStage < 20) {
            iRet = WriteDoc_Stage1(pPause);
        } else if (m_iStage < 30) {
            iRet = WriteDoc_Stage2(pPause);
        } else if (m_iStage < 90) {
            iRet = WriteDoc_Stage3(pPause);
        } else {
            iRet = WriteDoc_Stage4(pPause);
        }
        if (iRet < m_iStage) {
            break;
        }
    }
    if (iRet < 1 || m_iStage == 100) {
        m_iStage = -1;
        Clear();
        return iRet > 99 ? 0 : (iRet < 1 ? -1 : iRet);
    }
    return m_iStage;
}
FX_BOOL CPDF_Creator::SetFileVersion(FX_INT32 fileVersion )
{
    return m_pDocument->SetFileVersion(fileVersion);
}
void CPDF_Creator::RemoveSecurity()
{
    ResetStandardSecurity();
    m_bSecurityChanged = TRUE;
    m_pEncryptDict = NULL;
    crypto_handler_ = NULL;
}
void CPDF_Creator::SetProgressiveEncryptHandler(CPDF_ProgressiveEncryptHandler* pHandler)
{
    if (m_pProgressiveEncrypt) {
        delete m_pProgressiveEncrypt;
    }
    m_pProgressiveEncrypt = pHandler;
}
void CPDF_Creator::ResetStandardSecurity()
{
    if (m_bNewCrypto && crypto_handler_) {
        delete crypto_handler_;
        crypto_handler_ = NULL;
    }
    m_bNewCrypto = FALSE;
    if (!m_bStandardSecurity) {
        return;
    }
    if (m_pEncryptDict) {
        m_pEncryptDict = NULL;
    }
    m_bStandardSecurity = FALSE;
}
void CPDF_Creator::SetCustomSecurity(CPDF_Dictionary* pEncryptDict, CPDF_CryptoHandler* pCryptoHandler,
                                     FX_BOOL bEncryptMetadata)
{
    ResetStandardSecurity();
    m_bSecurityChanged = TRUE;
    m_pEncryptDict = pEncryptDict;
    crypto_handler_ = pCryptoHandler;
    m_bEncryptMetadata = bEncryptMetadata;
}
CPDF_WrapperCreator::CPDF_WrapperCreator(CPDF_Document* pWrapperDoc, FX_DWORD dwWrapperOffset)
    : CPDF_Creator(pWrapperDoc)
    , m_dwWrapperOffset(dwWrapperOffset)
    , m_bsType(FX_BSTRC(kFoxitWrapperName))
    , m_iVersion(1)
{
}
void CPDF_WrapperCreator::SetWrapperData(FX_BSTR bsType, FX_INT32 iVersion, FX_BSTR bsApplication, FX_BSTR bsURI, FX_BSTR bsDescription)
{
    m_bsType = bsType.IsEmpty() ? FX_BSTRC(kFoxitWrapperName) : bsType;
    m_iVersion = iVersion < 1 ? 1 : iVersion;
    m_bsApplication = bsApplication;
    m_bsURI = bsURI;
    m_bsDescription = bsDescription;
}
FX_BOOL CPDF_WrapperCreator::Create(IFX_StreamWrite* pFile)
{
    if (!pFile) {
        return FALSE;
    }
    if (!m_File.AttachFile(pFile)) {
        return FALSE;
    }
    m_dwFlags = 0;
    m_Offset = m_dwWrapperOffset;
    m_iStage = 0;
    InitID();
    FX_INT32 iRet = 0;
    while (m_iStage < 100) {
        if (m_iStage < 20) {
            iRet = WriteDoc_Stage1(NULL);
        } else if (m_iStage < 30) {
            iRet = WriteDoc_Stage2(NULL);
        } else if (m_iStage < 90) {
            iRet = WriteDoc_Stage3(NULL);
        } else {
            iRet = WriteDoc_Stage5(NULL);
        }
        if (iRet < m_iStage) {
            break;
        }
    }
    if (m_iStage < 100 || iRet < m_iStage) {
        Clear();
        return FALSE;
    }
    return TRUE;
}
FX_INT32 CPDF_WrapperCreator::WriteDoc_Stage5(IFX_Pause *pPause)
{
  FX_UNREFERENCED_PARAMETER(pPause);
    FXSYS_assert(m_iStage >= 90);
    if (m_File.AppendString(FX_BSTRC("trailer\r\n<<")) < 0) {
        return -1;
    }
    if (m_File.AppendString(FX_BSTRC("\r\n/Root ")) < 0) {
        return -1;
    }
    FX_DWORD dwRootObjNum = m_pDocument->GetRoot()->GetObjNum();
    if (m_File.AppendDWord(dwRootObjNum) < 0) {
        return -1;
    }
    if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
        return -1;
    }
    FX_WORD wRootObjVersion = GetObjectVersion(dwRootObjNum);
    if (m_File.AppendDWord(wRootObjVersion) < 0) {
        return -1;
    }
    if (m_File.AppendString(FX_BSTRC(" R")) < 0) {
        return -1;
    }
    if (m_pDocument->GetInfo()) {
        if (m_File.AppendString(FX_BSTRC(" /Info ")) < 0) {
            return -1;
        }
        FX_DWORD dwInfoObjNum = m_pDocument->GetInfo()->GetObjNum();
        if (m_File.AppendDWord(dwInfoObjNum) < 0) {
            return -1;
        }
        if (m_File.AppendString(FX_BSTRC(" ")) < 0) {
            return -1;
        }
        FX_WORD wInfoObjVersion = GetObjectVersion(dwInfoObjNum);
        if (m_File.AppendDWord(wInfoObjVersion) < 0) {
            return -1;
        }
        if (m_File.AppendString(FX_BSTRC(" R")) < 0) {
            return -1;
        }
    }
    if (m_File.AppendString(FX_BSTRC(" /Size ")) < 0) {
        return -1;
    }
    if (m_File.AppendDWord(m_pDocument->GetLastObjNum() + 1) < 0) {
        return -1;
    }
    if (m_File.AppendString(FX_BSTRC(" /Wrapper <</Type /")) < 0) {
        return -1;
    }
    if (m_File.AppendString(m_bsType) < 0) {
        return -1;
    }
    if (m_iVersion > 1) {
        if (m_File.AppendString(FX_BSTRC(" /Version ")) < 0) {
            return -1;
        }
        if (m_File.AppendDWord((FX_DWORD)m_iVersion) < 0) {
            return -1;
        }
    }
    if (!m_bsApplication.IsEmpty()) {
        if (m_File.AppendString(FX_BSTRC(" /Application (")) < 0) {
            return -1;
        }
        if (m_File.AppendString(m_bsApplication) < 0) {
            return -1;
        }
        if (m_File.AppendString(FX_BSTRC(")")) < 0) {
            return -1;
        }
    }
    if (!m_bsURI.IsEmpty()) {
        if (m_File.AppendString(FX_BSTRC(" /URI (")) < 0) {
            return -1;
        }
        if (m_File.AppendString(m_bsURI) < 0) {
            return -1;
        }
        if (m_File.AppendString(FX_BSTRC(")")) < 0) {
            return -1;
        }
    }
    if (!m_bsDescription.IsEmpty()) {
        if (m_File.AppendString(FX_BSTRC(" /Description (")) < 0) {
            return -1;
        }
        if (m_File.AppendString(m_bsDescription) < 0) {
            return -1;
        }
        if (m_File.AppendString(FX_BSTRC(")")) < 0) {
            return -1;
        }
    }
    if (m_File.AppendString(FX_BSTRC(">> /WrapperOffset ")) < 0) {
        return -1;
    }
    if (m_File.AppendDWord(m_dwWrapperOffset) < 0) {
        return -1;
    }
    if (m_pEncryptDict) {
        FX_DWORD dwEncryptObjNum = m_pEncryptDict->GetObjNum();
        FX_WORD wEncryptObjVersion = GetObjectVersion(dwEncryptObjNum);
        if (PDF_CreatorWriteEncrypt(m_pEncryptDict, dwEncryptObjNum, wEncryptObjVersion, &m_File) < 0) {
            return -1;
        }
    }
    if (m_pIDArray) {
        if (m_File.AppendString((FX_BSTRC("/ID"))) < 0) {
            return -1;
        }
        FX_FILESIZE offset = 0;
        if (PDF_CreatorAppendObject(this, m_pIDArray, &m_File, offset, NULL) < 0) {
            return -1;
        }
    }
    if (m_File.AppendString(FX_BSTRC(">>")) < 0) {
        return -1;
    }
    if (m_File.AppendString(FX_BSTRC("\r\nstartxref\r\n")) < 0) {
        return -1;
    }
    CFX_BinaryBuf startOffset_buf;
    FX_CHAR offset_buf[20];
    FXSYS_memset32(offset_buf, 0, sizeof(offset_buf));
    FXSYS_i64toa(m_XrefStart, offset_buf, 10);
    if (m_File.AppendBlock(offset_buf, FXSYS_strlen(offset_buf)) < 0) {
        return -1;
    }
    if (m_File.AppendString(FX_BSTRC("\r\n%%EOF\r\n")) < 0) {
        return -1;
    }
    m_File.Flush();
    return m_iStage = 100;
}
IPDF_WrapperCreator* FPDF_WrapperCreator_Create(CPDF_Document* pWrapperDoc, FX_DWORD dwWrapperOffset)
{
    return FX_NEW CPDF_WrapperCreator(pWrapperDoc, dwWrapperOffset);
}
void GetCheckSum(IFX_FileRead* pFile, CFX_ByteString &bsCheckSum)
{
    const FX_INT32 STREAM_BLOCK_SIZE = 1024 * 1024;
    size_t len = pFile->GetSize();
    FX_BYTE buf[16] = {0};
    FX_LPBYTE pBuffer = NULL;
    if (len > STREAM_BLOCK_SIZE) {
        FX_BYTE content[128] = {0};
        CRYPT_MD5Start(content);
        FX_INT32 nBlock = len / STREAM_BLOCK_SIZE;
        FX_INT32 nLeftCache = len % STREAM_BLOCK_SIZE;
        pBuffer = FX_Alloc(FX_BYTE, STREAM_BLOCK_SIZE);
        FX_INT32 i = 0;
        for (; i < nBlock; i++) {
            FXSYS_memset32(pBuffer, 0, STREAM_BLOCK_SIZE);
            pFile->ReadBlock(pBuffer, i * STREAM_BLOCK_SIZE, STREAM_BLOCK_SIZE);
            CRYPT_MD5Update(content, pBuffer, STREAM_BLOCK_SIZE);
        }
        pFile->ReadBlock(pBuffer, i * STREAM_BLOCK_SIZE, nLeftCache);
        CRYPT_MD5Update(content, pBuffer, nLeftCache);
        CRYPT_MD5Finish(content, buf);
    } else {
        pBuffer = FX_Alloc(FX_BYTE, len);
        FXSYS_memset32(pBuffer, 0, len);
        pFile->ReadBlock(pBuffer, len);
        CRYPT_MD5Generate(pBuffer, len, buf);
    }
    FX_Free(pBuffer);
    bsCheckSum = CFX_ByteString(buf, 16);
}
FX_BOOL CPDF_UnencryptedWrapperCreator::SetWrapperData()
{
    if (!m_pDocument || !m_pPayload) {
        return FALSE;
    }
    CPDF_Parser* pParser = (CPDF_Parser*)m_pDocument->GetParser();
    if (pParser && pParser->GetTrailer()) {
        pParser->GetTrailer()->RemoveAt(FX_BSTRC("Wrapper"));
        pParser->GetTrailer()->RemoveAt(FX_BSTRC("WrapperOffset"));
    }
    CPDF_Dictionary* pRootDict = m_pDocument->GetRoot();
    if (!pRootDict) {
        return FALSE;
    }
    CPDF_Dictionary* pCollection = pRootDict->GetDict(FX_BSTRC("Collection"));
    if (!pCollection) {
        pCollection = CPDF_Dictionary::Create();
        if (!pCollection) {
            return FALSE;
        }
        pRootDict->SetAt(FX_BSTRC("Collection"), pCollection);
    }
    if (!m_wsFileName.IsEmpty()) {
        pCollection->SetAtString(FX_BSTRC("D"), m_wsFileName);
    }
    pCollection->SetAtName(FX_BSTRC("View"), FX_BSTRC("H"));
    CPDF_Array* pAFArray = pRootDict->GetArray(FX_BSTRC("AF"));
    if (!pAFArray) {
        pAFArray = CPDF_Array::Create();
        if (!pAFArray) {
            return FALSE;
        }
        pRootDict->SetAt(FX_BSTRC("AF"), pAFArray);
    }
    CPDF_Dictionary* pFilespecDict = (CPDF_Dictionary*)pAFArray->GetElementValue(0);
    if (!pFilespecDict) {
        pFilespecDict = CPDF_Dictionary::Create();
        if (!pFilespecDict) {
            return FALSE;
        }
        m_pDocument->AddIndirectObject(pFilespecDict);
        pAFArray->AddReference(m_pDocument, pFilespecDict);
    }
    if (m_wsDescription.GetLength() != 0) {
        pFilespecDict->SetAtString(FX_BSTRC("Desc"), m_wsDescription);
    }
    CPDF_Dictionary* pEP = pFilespecDict->GetDict(FX_BSTRC("EP"));
    if (!pEP) {
        pEP = CPDF_Dictionary::Create();
        if (!pEP) {
            return FALSE;
        }
        pFilespecDict->SetAt(FX_BSTRC("EP"), pEP);
    }
    pEP->SetAtName(FX_BSTRC("Type"), FX_BSTRC("EncryptedPayload"));
    if (!m_wsSubType.IsEmpty()) {
        pEP->SetAtName(FX_BSTRC("Subtype"), m_wsSubType.UTF8Encode());
    }
    if (m_fVersion != 0.f) {
        pEP->SetAtNumber(FX_BSTRC("Version"), m_fVersion);
    }
    pFilespecDict->SetAtName(FX_BSTRC("AFRelationship"), FX_BSTRC("EncryptedPayload"));
    if (pFilespecDict->KeyExist(FX_BSTRC("EF"))) {
        CPDF_Dictionary* pEFDict = pFilespecDict->GetDict(FX_BSTRC("EF"));
        CPDF_Stream* pStream = (CPDF_Stream*)pEFDict->GetElementValue(FX_BSTRC("F"));
        if (pStream) {
            FX_DWORD dwNum = pStream->GetObjNum();
            m_pDocument->ReleaseIndirectObject(dwNum);
            m_pDocument->DeleteIndirectObject(dwNum);
        }
        pFilespecDict->RemoveAt(FX_BSTRC("EF"));
    }
    CPDF_FileSpec fileSpec(pFilespecDict);
    CFX_WideString wsFile = m_wsFileName;
    if (wsFile.IsEmpty()) {
        wsFile = pCollection->GetUnicodeText(FX_BSTRC("D"));
    }
    fileSpec.SetEmbeddedFile(m_pDocument, m_pPayload, wsFile);
    CPDF_Stream* pEmbedStream = fileSpec.GetFileStream();
    if (!pEmbedStream) {
        return FALSE;
    }
    CPDF_Dictionary* pEmbedDict = pEmbedStream->GetDict();
    if (!pEmbedDict) {
        return FALSE;
    }
    pEmbedDict->SetAtName(FX_BSTRC("Subtype"), FX_BSTRC("application/pdf"));
    CPDF_Dictionary* pParams = CPDF_Dictionary::Create();
    if (!pParams) {
        return FALSE;
    }
    pParams->SetAtInteger64(FX_BSTRC("Size"), m_pPayload->GetSize());
    CFX_ByteString bsCheckSum;
    GetCheckSum(m_pPayload, bsCheckSum);
    pParams->AddValue(FX_BSTRC("CheckSum"), CPDF_String::Create(bsCheckSum, TRUE));
    pEmbedDict->SetAt(FX_BSTRC("Params"), pParams);
    if (!pRootDict->KeyExist(FX_BSTRC("Names")) || !m_wsFileName.IsEmpty()) {
        if (pRootDict->KeyExist(FX_BSTRC("Names"))) {
            CPDF_Dictionary* pDict = pRootDict->GetDict(("Names"));
            CPDF_Dictionary* pEmbddedDict = pDict->GetDict(FX_BSTRC("EmbeddedFiles"));
            if (pEmbddedDict) {
                FX_DWORD dwObjNum = pEmbddedDict->GetObjNum();
                m_pDocument->ReleaseIndirectObject(dwObjNum);
                m_pDocument->DeleteIndirectObject(dwObjNum);
            }
            pRootDict->RemoveAt(FX_BSTRC("Names"));
        }
        CPDF_Dictionary* pParent = CPDF_Dictionary::Create();
        if (pParent == NULL) {
            return FALSE;
        }
        pRootDict->SetAt(FX_BSTRC("Names"), pParent);
        CPDF_NameTree pNameTree(pParent, FX_BSTRC("EmbeddedFiles"));
        pNameTree.SetValue(m_pDocument, m_wsFileName.UTF8Encode(), pFilespecDict);
    }
    return TRUE;
}
FX_BOOL CPDF_UnencryptedWrapperCreator::Create(IFX_FileWrite* pFile, FX_DWORD flags)
{
    if (!pFile) {
        return FALSE;
    }
    FX_BOOL bRet = SetWrapperData();
    if (!bRet) {
        return FALSE;
    }
    SetFileVersion(20);
    return CPDF_Creator::Create(pFile, flags);
}
FX_INT32 CPDF_UnencryptedWrapperCreator::Continue(IFX_Pause *pPause )
{
    return CPDF_Creator::Continue(pPause);
}
IPDF_UnencryptedWrapperCreator* FPDF_UnencryptedWrapperCreator_Create(CPDF_Document* pWrapperDoc)
{
    if (!pWrapperDoc) {
        return NULL;
    }
    return FX_NEW CPDF_UnencryptedWrapperCreator(pWrapperDoc);
}
