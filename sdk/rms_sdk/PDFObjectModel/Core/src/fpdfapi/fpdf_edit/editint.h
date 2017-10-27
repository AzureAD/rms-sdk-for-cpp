/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#ifndef _FPDF_EDITINT_
#define _FPDF_EDITINT_
class CPDF_ObjectStream : public CFX_Object
{
public:
    CPDF_ObjectStream();

    FX_BOOL				Start();

    FX_INT32			CompressIndirectObject(FX_DWORD dwObjNum, const CPDF_Object *pObj, CPDF_Creator* pCreator);
    FX_INT32			CompressIndirectObject(FX_DWORD dwObjNum, FX_LPCBYTE pBuffer, FX_DWORD dwSize);

    FX_FILESIZE			End(CPDF_Creator* pCreator);

    CFX_DWordArray		m_ObjNumArray;

    CFX_ByteTextBuf		m_Buffer;
    FX_DWORD			m_dwObjNum;
    FX_INT32			m_index;
protected:

    CFX_DWordArray		m_OffsetArray;
};
class CPDF_XRefStream : public CFX_Object
{
public:

    CPDF_XRefStream();

    FX_BOOL				Start();

    FX_INT32			CompressIndirectObject(FX_DWORD dwObjNum, const CPDF_Object *pObj, CPDF_Creator *pCreator);

    FX_INT32			CompressIndirectObject(FX_DWORD dwObjNum, FX_LPCBYTE pBuffer, FX_DWORD dwSize, CPDF_Creator *pCreator);

    FX_BOOL				End(CPDF_Creator *pCreator, FX_BOOL bEOF = FALSE);
    FX_BOOL				AddObjectNumberToIndexArray(FX_DWORD objnum, FX_BOOL bSort = FALSE);
    FX_BOOL				EndXRefStream(CPDF_Creator* pCreator);
    FX_BOOL				IsObjectStreamEmpty()
    {
        return m_ObjStream.m_ObjNumArray.GetSize() == 0;
    }

    CFX_DWordArray		m_IndexArray;

    FX_FILESIZE			m_PrevOffset;
    FX_DWORD			m_dwTempObjNum;

    CFX_ByteArray			m_1TypeArray;
    CFX_FileSizeArray		m_2FieldArray;
    CFX_Int32Array			m_3FieldArray;
protected:
    FX_INT32			EndObjectStream(CPDF_Creator *pCreator, FX_BOOL bEOF = TRUE);
    FX_BOOL				GenerateXRefStream(CPDF_Creator* pCreator, FX_BOOL bEOF);
    FX_INT32			m_iSeg;
    CPDF_ObjectStream	m_ObjStream;
    CFX_ByteTextBuf		m_Buffer;
};
class CPDF_WrapperCreator : public IPDF_WrapperCreator, public CPDF_Creator
{
public:
    CPDF_WrapperCreator(CPDF_Document* pWrapperDoc, FX_DWORD dwWrapperOffset);
    ~CPDF_WrapperCreator() {}
    virtual void		Release()
    {
        delete this;
    }
    virtual void		SetWrapperData(FX_BSTR bsType, FX_INT32 iVersion, FX_BSTR bsApplication, FX_BSTR bsURI, FX_BSTR bsDescription);
    virtual FX_BOOL		Create(IFX_StreamWrite* pFile);
protected:
    FX_DWORD			m_dwWrapperOffset;
    CFX_ByteString		m_bsType;
    FX_INT32			m_iVersion;
    CFX_ByteString		m_bsApplication;
    CFX_ByteString		m_bsURI;
    CFX_ByteString		m_bsDescription;
    FX_INT32			WriteDoc_Stage5(IFX_Pause *pPause);
};
class CPDF_UnencryptedWrapperCreator : public IPDF_UnencryptedWrapperCreator, public CPDF_Creator
{
public:
    CPDF_UnencryptedWrapperCreator(CPDF_Document* pWrapperDoc) : CPDF_Creator(pWrapperDoc)
        , m_fVersion(0.f)
        , m_pPayload(NULL)
    {
    }
    ~CPDF_UnencryptedWrapperCreator() {}
    virtual void		Release()
    {
        delete this;
    }
    virtual void		SetPayloadInfo(FX_WSTR wsSubType, FX_WSTR wsFileName, FX_WSTR wsDescription = FX_WSTRC(L""), FX_FLOAT version_num = 0.f)
    {
        m_wsSubType = wsSubType;
        m_wsFileName = wsFileName;
        m_wsDescription = wsDescription;
        m_fVersion = version_num;
    }
    virtual void		SetPayLoad(IFX_FileRead* pPayload)
    {
        m_pPayload = pPayload;
    }
    virtual FX_BOOL		Create(IFX_FileWrite* pFile, FX_DWORD flags = 0);
    virtual	FX_INT32	Continue(IFX_Pause *pPause = NULL);
protected:
    CFX_WideString		m_wsSubType;
    CFX_WideString		m_wsFileName;
    CFX_WideString		m_wsDescription;
    FX_FLOAT			m_fVersion;
    IFX_FileRead*		m_pPayload;
    FX_INT32			SetWrapperData();
};
#endif
