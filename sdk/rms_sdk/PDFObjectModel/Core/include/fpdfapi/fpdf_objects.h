/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#ifndef _FPDF_OBJECTS_
#define _FPDF_OBJECTS_
#ifndef _FXCRT_EXTENSION_
#include "../fxcrt/fx_ext.h"
#endif
class CPDF_Document;
class CPDF_IndirectObjects;
class CPDF_Null;
class CPDF_Boolean;
class CPDF_Number;
class CPDF_String;
class CPDF_Stream;
class CPDF_StreamAcc;
class CPDF_StreamFilter;
class CPDF_Array;
class CPDF_Dictionary;
class CPDF_Reference;
class IPDF_DocParser;
class IFX_FileRead;
class CPDF_CryptoHandler;
#define PDFOBJ_INVALID		0
#define	PDFOBJ_BOOLEAN		1
#define PDFOBJ_NUMBER		2
#define PDFOBJ_STRING		3
#define PDFOBJ_NAME			4
#define PDFOBJ_ARRAY		5
#define PDFOBJ_DICTIONARY	6
#define PDFOBJ_STREAM		7
#define PDFOBJ_NULL			8
#define PDFOBJ_REFERENCE	9
typedef IFX_FileStream* (*FPDF_LPFCloneStreamCallback)(CPDF_Stream *pStream, FX_LPVOID pUserData);
class CPDF_Object : public CFX_Object
{
public:

    int						GetType() const
    {
        return m_Type;
    }

    FX_DWORD				GetObjNum() const
    {
        return m_ObjNum;
    }
    FX_DWORD				GetGenNum() const
    {
        return m_GenNum;
    }

    FX_BOOL					IsIdentical(CPDF_Object* pObj) const;

    CPDF_Object*			Clone(FX_BOOL bDirect = FALSE) const;

    CPDF_Object*			CloneRef(CPDF_IndirectObjects* pObjs) const;

    CPDF_Object*			GetDirect() const;

    void					Release();

    CFX_ByteString			GetString() const;

    CFX_ByteStringC			GetConstString() const;

    CFX_WideString			GetUnicodeText(CFX_CharMap* pCharMap = NULL, const CFX_ByteString& encoding = "") const;

    FX_FLOAT				GetNumber() const;

    FX_FLOAT				GetNumber16() const;

    FX_INT32				GetInteger() const;

    FX_INT64				GetInteger64() const;

    CPDF_Dictionary*		GetDict() const;

    CPDF_Array*				GetArray() const;

    void					SetString(const CFX_ByteString& str);

    void					SetUnicodeText(FX_LPCWSTR pUnicodes, int len = -1, FX_BOOL bUTF8 = FALSE);

    int						GetDirectType() const;

    FX_BOOL					IsModified() const
    {
        return FALSE;
    }

    FX_BOOL					IsNewCreated() const
    {
        return m_bNewCreated;
    }
protected:
    FX_DWORD				m_Type;
    CPDF_Object()
    {
        m_ObjNum = 0;
        m_bNewCreated = TRUE;
    }

    FX_DWORD 				m_ObjNum;

    FX_DWORD				m_GenNum;

    FX_BOOL					m_bNewCreated;

    void					Destroy();


    ~CPDF_Object() {}
    friend class			CPDF_IndirectObjects;
    friend class			CPDF_Parser;
    friend class			CPDF_SyntaxParser;
private:
    CPDF_Object(const CPDF_Object& src) {}
    CPDF_Object* CloneInternal(FX_BOOL bDirect, CFX_MapPtrToPtr* visited) const;
};
class CPDF_Boolean : public CPDF_Object
{
public:

    static CPDF_Boolean*	Create(FX_BOOL value)
    {
        return FX_NEW CPDF_Boolean(value);
    }

    CPDF_Boolean()
    {
        m_Type = PDFOBJ_BOOLEAN;
        m_bValue = FALSE;
    }

    CPDF_Boolean(FX_BOOL value)
    {
        m_Type = PDFOBJ_BOOLEAN;
        m_bValue = value;
    }

    FX_BOOL					Identical(CPDF_Boolean* pOther) const
    {
        return m_bValue == pOther->m_bValue;
    }
protected:

    FX_BOOL					m_bValue;
    friend class			CPDF_Object;
};
class CPDF_Number : public CPDF_Object
{
public:

    static CPDF_Number*		Create(FX_INT32 value)
    {
        return FX_NEW CPDF_Number(value);
    }

    static CPDF_Number*		Create(FX_INT64 value)
    {
        return FX_NEW CPDF_Number(value);
    }

    static CPDF_Number*		Create(FX_FLOAT value)
    {
        return FX_NEW CPDF_Number(value);
    }

    static CPDF_Number*		Create(FX_BSTR str)
    {
        return FX_NEW CPDF_Number(str);
    }

    static CPDF_Number*		Create(FX_BOOL bInteger, void* pData)
    {
        return FX_NEW CPDF_Number(bInteger, pData);
    }

    CPDF_Number(): m_Integer(0)
    {
        m_Type = PDFOBJ_NUMBER;
    }

    CPDF_Number(FX_BOOL bInteger, void* pData);

    CPDF_Number(FX_INT32 value);

    CPDF_Number(FX_INT64 value);

    CPDF_Number(FX_FLOAT value);

    CPDF_Number(FX_BSTR str);

    FX_BOOL					Identical(CPDF_Number* pOther) const;

    CFX_ByteString			GetString() const;

    CFX_ByteString			GetString64() const;

    void					SetString(FX_BSTR str);

    void					SetString64(FX_BSTR str);

    FX_BOOL					IsInteger() const
    {
        return m_bInteger;
    }

    FX_INT32				GetInteger() const
    {
        return m_bInteger ? m_Integer : (FX_INT32)m_Float;
    }

    FX_INT64				GetInteger64() const;

    FX_FLOAT				GetNumber() const
    {
        return m_bInteger ? (FX_FLOAT)m_Integer : m_Float;
    }

    void					SetNumber(FX_FLOAT value);

    FX_FLOAT			GetNumber16() const
    {
        return GetNumber();
    }

    FX_FLOAT				GetFloat() const
    {
        return m_bInteger ? (FX_FLOAT)m_Integer : m_Float;
    }
protected:

    FX_BOOL					m_bInteger;

    union {

        FX_INT32			m_Integer;

        FX_FLOAT			m_Float;

        struct {

            FX_INT32		m_LoInteger;

            FX_INT32		m_HiInteger;
        } m_Integer64;
    };
    friend class			CPDF_Object;
};
class CPDF_String : public CPDF_Object
{
public:

    static CPDF_String*		Create(const CFX_ByteString& str, FX_BOOL bHex = FALSE)
    {
        return FX_NEW CPDF_String(str, bHex);
    }
    static CPDF_String*		Create(const CFX_WideString& str)
    {
        return FX_NEW CPDF_String(str);
    }
    CPDF_String()
    {
        m_Type = PDFOBJ_STRING;
        m_bHex = FALSE;
    }
    CPDF_String(const CFX_ByteString& str, FX_BOOL bHex = FALSE) : m_String(str)
    {
        m_Type = PDFOBJ_STRING;
        m_bHex = bHex;
    }
    CPDF_String(const CFX_WideString& str);

    CFX_ByteString&			GetString()
    {
        return m_String;
    }

    FX_BOOL					Identical(CPDF_String* pOther) const
    {
        return m_String == pOther->m_String;
    }

    FX_BOOL					IsHex() const
    {
        return m_bHex;
    }
protected:

    CFX_ByteString			m_String;

    FX_BOOL					m_bHex;
    friend class			CPDF_Object;
};
class CPDF_Name : public CPDF_Object
{
public:

    static CPDF_Name*		Create(const CFX_ByteString& str)
    {
        return FX_NEW CPDF_Name(str);
    }

    static CPDF_Name*		Create(FX_BSTR str)
    {
        return FX_NEW CPDF_Name(str);
    }

    static CPDF_Name*		Create(FX_LPCSTR str)
    {
        return FX_NEW CPDF_Name(str);
    }

    CPDF_Name(const CFX_ByteString& str) : m_Name(str)
    {
        m_Type = PDFOBJ_NAME;
    }

    CPDF_Name(FX_BSTR str) : m_Name(str)
    {
        m_Type = PDFOBJ_NAME;
    }

    CPDF_Name(FX_LPCSTR str) : m_Name(str)
    {
        m_Type = PDFOBJ_NAME;
    }

    CFX_ByteString&			GetString()
    {
        return m_Name;
    }

    FX_BOOL					Identical(CPDF_Name* pOther) const
    {
        return m_Name == pOther->m_Name;
    }
protected:

    CFX_ByteString			m_Name;
    friend class			CPDF_Object;
};
class CPDF_Array : public CPDF_Object
{
public:

    static CPDF_Array*		Create()
    {
        return FX_NEW CPDF_Array();
    }

    CPDF_Array()
    {
        m_Type = PDFOBJ_ARRAY;
    }

    FX_DWORD				GetCount() const
    {
        return m_Objects.GetSize();
    }

    CPDF_Object*			GetElement(FX_DWORD index) const;

    CPDF_Object*			GetElementValue(FX_DWORD index) const;



    CFX_AffineMatrix		GetMatrix();

    CFX_FloatRect			GetRect();




    CFX_ByteString			GetString(FX_DWORD index) const;

    CFX_ByteStringC			GetConstString(FX_DWORD index) const;

    int						GetInteger(FX_DWORD index) const;

    FX_FLOAT				GetNumber(FX_DWORD index) const;

    CPDF_Dictionary*		GetDict(FX_DWORD index) const;

    CPDF_Stream*			GetStream(FX_DWORD index) const;

    CPDF_Array*				GetArray(FX_DWORD index) const;

    FX_FLOAT				GetFloat(FX_DWORD index) const
    {
        return GetNumber(index);
    }




    void					SetAt(FX_DWORD index, CPDF_Object* pObj, CPDF_IndirectObjects* pObjs = NULL);


    void					InsertAt(FX_DWORD index, CPDF_Object* pObj, CPDF_IndirectObjects* pObjs = NULL);

    void					RemoveAt(FX_DWORD index, FX_BOOL bRelease = TRUE);


    void					Add(CPDF_Object* pObj, CPDF_IndirectObjects* pObjs = NULL);



    void					AddNumber(FX_FLOAT f);

    void					AddInteger(int i);

    void					AddString(const CFX_ByteString& str);

    void					AddName(const CFX_ByteString& str);

    void					AddReference(CPDF_IndirectObjects* pDoc, FX_DWORD objnum);

    void					AddReference(CPDF_IndirectObjects* pDoc, CPDF_Object* obj)
    {
        AddReference(pDoc, obj->GetObjNum());
    }


    FX_FLOAT			GetNumber16(FX_DWORD index) const
    {
        return GetNumber(index);
    }

    void					AddNumber16(FX_FLOAT value)
    {
        AddNumber(value);
    }

    FX_BOOL					Identical(CPDF_Array* pOther) const;

    void					MoveData(CPDF_Array* pSrcArray);
protected:

    ~CPDF_Array();

    CFX_PtrArray			m_Objects;
    friend class			CPDF_Object;
};
class CPDF_Dictionary : public CPDF_Object
{
public:

    static CPDF_Dictionary*	Create()
    {
        return FX_NEW CPDF_Dictionary();
    }

    CPDF_Dictionary()
    {
        m_Type = PDFOBJ_DICTIONARY;
    }



    CPDF_Object*			GetElement(FX_BSTR key) const;

    CPDF_Object*			GetElementValue(FX_BSTR key) const;





    CFX_ByteString			GetString(FX_BSTR key) const;

    CFX_ByteStringC			GetConstString(FX_BSTR key) const;

    CFX_ByteString			GetString(FX_BSTR key, FX_BSTR default_str) const;

    CFX_ByteStringC			GetConstString(FX_BSTR key, FX_BSTR default_str) const;

    CFX_WideString			GetUnicodeText(FX_BSTR key, CFX_CharMap* pCharMap = NULL, FX_BSTR encodeing = "") const;

    FX_INT32				GetInteger(FX_BSTR key) const;

    FX_INT32				GetInteger(FX_BSTR key, FX_INT32 default_int) const;

    FX_INT64				GetInteger64(FX_BSTR key) const;

    FX_INT64				GetInteger64(FX_BSTR key, FX_INT64 default_int) const;

    FX_BOOL					GetBoolean(FX_BSTR key, FX_BOOL bDefault = FALSE) const;

    FX_FLOAT				GetNumber(FX_BSTR key) const;

    CPDF_Dictionary*		GetDict(FX_BSTR key) const;

    CPDF_Name*				GetName(FX_BSTR key) const;

    CPDF_Stream*			GetStream(FX_BSTR key) const;

    CPDF_Array*				GetArray(FX_BSTR key) const;

    CFX_FloatRect			GetRect(FX_BSTR key) const;

    CFX_AffineMatrix		GetMatrix(FX_BSTR key) const;

    FX_FLOAT				GetFloat(FX_BSTR key) const
    {
        return GetNumber(key);
    }


    FX_BOOL					KeyExist(FX_BSTR key) const;

    FX_POSITION				GetStartPos() const;

    CPDF_Object*			GetNextElement(FX_POSITION& pos, CFX_ByteString& key) const;

    void					SetAt(FX_BSTR key, CPDF_Object* pObj, CPDF_IndirectObjects* pObjs = NULL);



    void					SetAtName(FX_BSTR key, const CFX_ByteString& name);


    void					SetAtString(FX_BSTR key, const CFX_ByteString& string);
    void					SetAtString(FX_BSTR key, const CFX_WideString& wsStr);

    void					SetAtInteger(FX_BSTR key, FX_INT32 i);


    void					SetAtInteger64(FX_BSTR key, FX_INT64 i);

    void					SetAtNumber(FX_BSTR key, FX_FLOAT f);

    void					SetAtReference(FX_BSTR key, CPDF_IndirectObjects* pDoc, FX_DWORD objnum);

    void					SetAtReference(FX_BSTR key, CPDF_IndirectObjects* pDoc, CPDF_Object* obj)
    {
        SetAtReference(key, pDoc, obj->GetObjNum());
    }

    void					AddReference(FX_BSTR key, CPDF_IndirectObjects* pDoc, FX_DWORD objnum);

    void					AddReference(FX_BSTR key, CPDF_IndirectObjects* pDoc, CPDF_Object* obj)
    {
        AddReference(key, pDoc, obj->GetObjNum());
    }

    void					SetAtRect(FX_BSTR key, const CFX_FloatRect& rect);

    void					SetAtMatrix(FX_BSTR key, const CFX_AffineMatrix& matrix);

    void					SetAtBoolean(FX_BSTR key, FX_BOOL bValue);



    void					RemoveAt(FX_BSTR key, FX_BOOL bRelease = TRUE);


    void					ReplaceKey(FX_BSTR oldkey, FX_BSTR newkey);

    FX_BOOL					Identical(CPDF_Dictionary* pDict) const;

    int						GetCount() const
    {
        return m_Map.GetCount();
    }

    void					AddValue(FX_BSTR key, CPDF_Object* pObj);
protected:

    ~CPDF_Dictionary();

    CFX_MapByteStringToPtr	m_Map;

    friend class			CPDF_Object;
};
class CPDF_Stream : public CPDF_Object
{
public:

    static CPDF_Stream*		Create(FX_LPBYTE pData, size_t size, CPDF_Dictionary* pDict)
    {
        return FX_NEW CPDF_Stream(pData, size, pDict);
    }

    CPDF_Stream(FX_LPBYTE pData, size_t size, CPDF_Dictionary* pDict);

    CPDF_Stream(IFX_FileRead* pFile, class CPDF_CryptoHandler* pCrypto,
                FX_FILESIZE offset, FX_FILESIZE size, CPDF_Dictionary* pDict, FX_DWORD gennum);

    CPDF_Dictionary*		GetDict() const
    {
        return m_pDict;
    }

    FX_BOOL					SetData(FX_LPCBYTE pData, size_t size, FX_BOOL bCompressed, FX_BOOL bKeepBuf);

    FX_BOOL					InitStream(FX_BYTE* pData, size_t size, CPDF_Dictionary* pDict);

    void					InitStream(IFX_FileRead *pFile, CPDF_Dictionary* pDict);

    FX_BOOL					Identical(CPDF_Stream* pOther) const;

    CPDF_StreamFilter*		GetStreamFilter(FX_BOOL bRaw = FALSE) const;



    FX_FILESIZE				GetRawSize() const
    {
        return m_dwSize;
    }

    FX_BOOL					ReadRawData(FX_FILESIZE start_pos, FX_LPBYTE pBuf, size_t buf_size) const;


    FX_BOOL					IsMemoryBased() const
    {
        return m_GenNum == (FX_DWORD) - 1;
    }

    CPDF_Stream*			Clone(FX_BOOL bDirect, FPDF_LPFCloneStreamCallback lpfCallback, FX_LPVOID pUserData) const;
protected:

    ~CPDF_Stream();

    CPDF_Dictionary*		m_pDict;

    FX_FILESIZE				m_dwSize;

    union {

        FX_LPBYTE			m_pDataBuf;

        IFX_FileRead*		m_pFile;
    };

    FX_BOOL					m_bKeepFile;

    FX_FILESIZE				m_FileOffset;

    CPDF_CryptoHandler*		crypto_handler_;

    void					InitStream(CPDF_Dictionary* pDict);
    friend class			CPDF_Object;
    friend class			CPDF_StreamAcc;
};
class CPDF_StreamAcc : public CFX_Object
{
public:

    CPDF_StreamAcc();

    ~CPDF_StreamAcc();

    FX_BOOL					LoadAllData(const CPDF_Stream* pStream, FX_BOOL bRawAccess = FALSE,
                                        FX_DWORD estimated_size = 0, FX_BOOL bImageAcc = FALSE);

    const CPDF_Stream*		GetStream() const
    {
        return m_pStream;
    }

    CPDF_Dictionary*		GetDict() const
    {
        return m_pStream ? m_pStream->GetDict() : NULL;
    }

    FX_LPCBYTE				GetData() const;

    FX_FILESIZE				GetSize() const;

    FX_LPBYTE				DetachData();

    const CFX_ByteString&	GetImageDecoder() const
    {
        return m_ImageDecoder;
    }

    const CPDF_Dictionary*	GetImageParam() const
    {
        return m_pImageParam;
    }
protected:

    FX_LPBYTE				m_pData;

    size_t					m_dwSize;

    FX_BOOL					m_bNewBuf;

    CFX_ByteString			m_ImageDecoder;

    CPDF_Dictionary*		m_pImageParam;

    const CPDF_Stream*		m_pStream;

    FX_LPBYTE				m_pSrcData;
};
CFX_DataFilter* FPDF_CreateFilter(FX_BSTR name, const CPDF_Dictionary* pParam, int width = 0, int height = 0);
#define FPDF_FILTER_BUFFER_SIZE		20480
class CPDF_StreamFilter : public CFX_Object
{
public:

    ~CPDF_StreamFilter();
    typedef enum { Finish, Error } ReadStatus;

    size_t				ReadBlock(FX_LPBYTE buffer, size_t size);

    FX_FILESIZE			GetSrcPos()  const
    {
        return m_SrcOffset;
    }

    const CPDF_Stream*	GetStream()  const
    {
        return m_pStream;
    }

    ReadStatus			GetStatus() const
    {
        return m_Status;
    }
protected:

    CPDF_StreamFilter()
    {
        m_pStream = NULL;
        m_pFilter = NULL;
        m_pBuffer = NULL;
        m_BufOffset = 0;
        m_SrcOffset = 0;
        m_pSrcBuffer = NULL;
        m_SrcBufferSize = 0;
        m_Status = Finish;
    }

    size_t				ReadLeftOver(FX_LPBYTE buffer, size_t buf_size);

    const CPDF_Stream*	m_pStream;

    CFX_DataFilter*		m_pFilter;

    CFX_BinaryBuf*		m_pBuffer;

    FX_FILESIZE			m_BufOffset;

    FX_FILESIZE			m_SrcOffset;

    FX_LPBYTE			m_pSrcBuffer;
    FX_UINT32			m_SrcBufferSize;

    ReadStatus			m_Status;
    friend class CPDF_Stream;
};
class CPDF_Null : public CPDF_Object
{
public:

    static CPDF_Null*		Create()
    {
        return FX_NEW CPDF_Null();
    }

    CPDF_Null()
    {
        m_Type = PDFOBJ_NULL;
    }
};
class CPDF_Reference : public CPDF_Object
{
public:

    static CPDF_Reference*	Create(CPDF_IndirectObjects* pDoc, int objnum, int objversion = 0)
    {
        return FX_NEW CPDF_Reference(pDoc, objnum, objversion);
    }

    CPDF_Reference(CPDF_IndirectObjects* pDoc, int objnum, int objversion = 0)
    {
        m_Type = PDFOBJ_REFERENCE;
        m_pObjList = pDoc;
        m_RefObjNum = objnum;
        m_RefObjVersion = objversion;
    }

    CPDF_IndirectObjects*	GetObjList() const
    {
        return m_pObjList;
    }

    FX_DWORD				GetRefObjNum() const
    {
        return m_RefObjNum;
    }

    FX_INT32				GetRefObjVersion() const
    {
        return m_RefObjVersion;
    }

    void					SetRef(CPDF_IndirectObjects* pDoc, FX_DWORD objnum, FX_INT32 objversion = 0);

    FX_BOOL					Identical(CPDF_Reference* pOther) const
    {
        return (m_RefObjNum == pOther->m_RefObjNum && m_RefObjVersion == pOther->m_RefObjVersion);
    }

    CPDF_Object*			GetDirectSafely(FX_LPVOID pParseContent = NULL);
protected:

    CPDF_IndirectObjects*	m_pObjList;

    FX_DWORD				m_RefObjNum;

    FX_INT32				m_RefObjVersion;
    friend class			CPDF_Object;
};
class CPDF_IndirectObjects : public CFX_Object
{
public:

    explicit CPDF_IndirectObjects(IPDF_DocParser* pParser);

    ~CPDF_IndirectObjects();

    CPDF_Object*			GetIndirectObject(FX_DWORD objnum, struct PARSE_CONTEXT* pContext = NULL);

    int						GetIndirectType(FX_DWORD objnum);

    FX_DWORD				AddIndirectObject(CPDF_Object* pObj);

    void					ReleaseIndirectObject(FX_DWORD objnum);

    void					DeleteIndirectObject(FX_DWORD objnum);

    void					InsertIndirectObject(FX_DWORD objnum, CPDF_Object* pObj);

    FX_DWORD				GetLastObjNum() const;

    FX_POSITION				GetStartPosition() const
    {
        return m_IndirectObjs.GetStartPosition();
    }

    void					GetNextAssoc(FX_POSITION& rPos, FX_DWORD& objnum, CPDF_Object*& pObject) const
    {
        m_IndirectObjs.GetNextAssoc(rPos, (void*&)objnum, (void*&)pObject);
    }
protected:

    CFX_MapPtrToPtr			m_IndirectObjs;

    IPDF_DocParser*			m_pParser;

    FX_DWORD				m_LastObjNum;
};
#endif
