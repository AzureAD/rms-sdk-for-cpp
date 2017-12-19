#ifndef _FX_BASIC_H_
#define _FX_BASIC_H_
#ifndef _FX_SYSTEM_H_
#include "fx_system.h"
#endif
#ifndef _FX_MEMORY_H_
#include "fx_memory.h"
#endif
#ifndef _FX_STRING_H_
#include "fx_string.h"
#endif
#ifndef _FX_STREAM_H_
#include "fx_stream.h"
#endif
class CFX_BinaryBuf : public CFX_Object
{
public:
    CFX_BinaryBuf();
    CFX_BinaryBuf(FX_STRSIZE size);

    ~CFX_BinaryBuf();

    void					Clear();

    FX_BOOL					EstimateSize(FX_STRSIZE size, FX_STRSIZE alloc_step = 0);

    FX_BOOL					AppendBlock(const void* pBuf, FX_STRSIZE size);

    FX_BOOL					AppendFill(FX_BYTE byte, FX_STRSIZE count);

    void					AppendString(FX_BSTR str)
    {
        AppendBlock(str.GetPtr(), str.GetLength());
    }

    inline void				AppendByte(FX_BYTE byte)
    {
        if (m_AllocSize <= m_DataSize) {
            ExpandBuf(100);
        }
        m_pBuffer[m_DataSize++] = byte;
    }

    FX_BOOL					InsertBlock(FX_STRSIZE pos, const void* pBuf, FX_STRSIZE size);

    void					AttachData(void* pBuf, FX_STRSIZE size);

    FX_BOOL					CopyData(const void* pBuf, FX_STRSIZE size);

    void					TakeOver(CFX_BinaryBuf& other);

    void					Delete(int start_index, int count);

    FX_LPBYTE				GetBuffer() const
    {
        return m_pBuffer;
    }

    FX_STRSIZE				GetSize() const
    {
        return m_DataSize;
    }

    CFX_ByteStringC			GetByteString() const;

    void					DetachBuffer();
protected:

    FX_STRSIZE				m_AllocStep;

    FX_LPBYTE				m_pBuffer;

    FX_STRSIZE				m_DataSize;

    FX_STRSIZE				m_AllocSize;

    FX_BOOL					ExpandBuf(FX_STRSIZE size);
};
class CFX_ByteTextBuf : public CFX_BinaryBuf
{
public:

    void					operator = (FX_BSTR str);

    void					AppendChar(int ch)
    {
        AppendByte((FX_BYTE)ch);
    }

    CFX_ByteTextBuf&		operator << (int i);

    CFX_ByteTextBuf&		operator << (FX_DWORD i);

    CFX_ByteTextBuf&		operator << (double f);

    CFX_ByteTextBuf&		operator << (FX_BSTR lpsz);

    CFX_ByteTextBuf&		operator << (const CFX_ByteTextBuf& buf);

    FX_STRSIZE				GetLength() const
    {
        return m_DataSize;
    }
};
class CFX_WideTextBuf : public CFX_BinaryBuf
{
public:

    void					operator = (FX_LPCWSTR lpsz);

    void					operator = (FX_WSTR str);

    FX_BOOL					AppendChar(FX_WCHAR wch);

    CFX_WideTextBuf&		operator << (int i);

    CFX_WideTextBuf&		operator << (double f);

    CFX_WideTextBuf&		operator << (FX_LPCWSTR lpsz);

    CFX_WideTextBuf&		operator << (FX_WSTR str);
    CFX_WideTextBuf&		operator << (const CFX_WideString &str);

    CFX_WideTextBuf&		operator << (const CFX_WideTextBuf& buf);

    FX_STRSIZE				GetLength() const
    {
        return m_DataSize / sizeof(FX_WCHAR);
    }

    FX_LPWSTR				GetBuffer() const
    {
        return (FX_LPWSTR)m_pBuffer;
    }

    void					Delete(int start_index, int count)
    {
        CFX_BinaryBuf::Delete(start_index * sizeof(FX_WCHAR), count * sizeof(FX_WCHAR));
    }

    CFX_WideStringC			GetWideString() const;
};
class IFX_BufferArchive
{
public:
    IFX_BufferArchive(FX_STRSIZE size);


    virtual void			Clear();


    FX_BOOL					Flush();


    FX_INT32				AppendBlock(const void* pBuf, size_t size);

    FX_INT32				AppendByte(FX_BYTE byte);

    FX_INT32				AppendDWord(FX_DWORD i);


    FX_INT32				AppendInt64(FX_INT64 i);

    FX_INT32				AppendString(FX_BSTR lpsz);

protected:

    virtual	FX_BOOL			DoWork(const void* pBuf, size_t size) = 0;

    FX_STRSIZE				m_BufSize;

    FX_LPBYTE				m_pBuffer;

    FX_STRSIZE				m_Length;
};
class CFX_FileBufferArchive : public IFX_BufferArchive, public CFX_Object
{
public:
    CFX_FileBufferArchive(FX_STRSIZE size = 32768);
    ~CFX_FileBufferArchive();
    virtual void			Clear();

    FX_BOOL					AttachFile(IFX_StreamWrite *pFile, FX_BOOL bTakeover = FALSE);

    FX_BOOL					AttachFile(FX_LPCWSTR filename);

    FX_BOOL					AttachFile(FX_LPCSTR filename);
private:

    virtual FX_BOOL			DoWork(const void* pBuf, size_t size);

    IFX_StreamWrite			*m_pFile;

    FX_BOOL					m_bTakeover;
};
struct CFX_CharMap {

    static CFX_CharMap*		GetDefaultMapper(FX_INT32 codepage = 0);


    CFX_WideString	(*m_GetWideString)(CFX_CharMap* pMap, const CFX_ByteString& bstr);

    CFX_ByteString	(*m_GetByteString)(CFX_CharMap* pMap, const CFX_WideString& wstr);

    FX_INT32		(*m_GetCodePage)();
};
class CFX_UTF8Decoder
{
public:
    CFX_UTF8Decoder(): m_PendingBytes(0), m_PendingChar(0) { }

    void			Clear();

    void			Input(FX_BYTE byte);

    void			AppendChar(FX_DWORD ch);

    void			ClearStatus()
    {
        m_PendingBytes = 0;
    }

    CFX_WideStringC	GetResult() const
    {
        return m_Buffer.GetWideString();
    }
protected:

    int				m_PendingBytes;

    FX_DWORD		m_PendingChar;

    CFX_WideTextBuf	m_Buffer;
};
class CFX_UTF8Encoder
{
public:
    CFX_UTF8Encoder()
    {
        m_UTF16First = 0;
    }

    void			Input(FX_WCHAR unicode);

    void			AppendStr(FX_BSTR str)
    {
        m_UTF16First = 0;
        m_Buffer << str;
    }

    CFX_ByteStringC	GetResult() const
    {
        return m_Buffer.GetByteString();
    }
protected:

    CFX_ByteTextBuf	m_Buffer;

    FX_DWORD		m_UTF16First;
};
CFX_ByteString FX_UrlEncode(const CFX_WideString& wsUrl);
CFX_WideString FX_UrlDecode(const CFX_ByteString& bsUrl);
CFX_ByteString FX_EncodeURI(const CFX_WideString& wsURI);
CFX_WideString FX_DecodeURI(const CFX_ByteString& bsURI);
class CFX_BasicArray : public CFX_Object
{
protected:
    CFX_BasicArray(int unit_size);

    ~CFX_BasicArray();
    CFX_BasicArray(const CFX_BasicArray& other);

    CFX_BasicArray& operator=(const CFX_BasicArray& other);

    FX_BOOL			SetSize(int nNewSize, int nGrowBy);

    FX_BOOL			Append(const CFX_BasicArray& src);

    FX_BOOL			Copy(const CFX_BasicArray& src);

    FX_LPBYTE		InsertSpaceAt(int nIndex, int nCount);

    FX_BOOL			RemoveAt(int nIndex, int nCount);

    FX_BOOL			InsertAt(int nStartIndex, const CFX_BasicArray* pNewArray);

    const void*		GetDataPtr(int index) const;
protected:

    FX_LPBYTE		m_pData;

    int				m_nSize;

    int				m_nMaxSize;

    int				m_nGrowBy;

    int				m_nUnitSize;
};
template<class TYPE>
class CFX_ArrayTemplate : public CFX_BasicArray
{
public:
    enum ErrorType {
        invalidArraySize,
        memoryAllocationError,
        indexOutOfRange
    };
    CFX_ArrayTemplate() : CFX_BasicArray(sizeof(TYPE)) {}
    CFX_ArrayTemplate(const CFX_ArrayTemplate& other) : CFX_BasicArray(other) {}

    void FX_Error(ErrorType error, FX_INT32 badIndex = 0) const
    {
        const char *errorMsg[] = {
            "Invalid array size",
            "Memory allocation error",
            "Invalid index:"
        };
        fprintf(stderr, "%s\n", errorMsg[error]);
        if(error == indexOutOfRange) {
            fprintf(stderr, "%i\n", badIndex);
        }
        abort();
    }

    int			GetSize() const
    {
        return m_nSize;
    }

    int			GetUpperBound() const
    {
        return m_nSize - 1;
    }

    FX_BOOL		SetSize(int nNewSize, int nGrowBy = -1)
    {
        return CFX_BasicArray::SetSize(nNewSize, nGrowBy);
    }

    void		RemoveAll()
    {
        SetSize(0, -1);
    }

    const TYPE	GetAt(int nIndex) const
    {
        if (nIndex < 0 || nIndex >= m_nSize) {
            FX_Error(indexOutOfRange);
        }
        return ((const TYPE*)m_pData)[nIndex];
    }

    FX_BOOL		SetAt(int nIndex, TYPE newElement)
    {
        if (nIndex < 0 || nIndex >= m_nSize) {
            return FALSE;
        }
        ((TYPE*)m_pData)[nIndex] = newElement;
        return TRUE;
    }

    TYPE&		ElementAt(int nIndex)
    {
        if (nIndex < 0 || nIndex >= m_nSize) {
            FX_Error(indexOutOfRange);
        }
        return ((TYPE*)m_pData)[nIndex];
    }

    const TYPE*	GetData() const
    {
        return (const TYPE*)m_pData;
    }

    TYPE*		GetData()
    {
        return (TYPE*)m_pData;
    }

    FX_BOOL		SetAtGrow(int nIndex, TYPE newElement)
    {
        if (nIndex < 0) {
            return FALSE;
        }
        if (nIndex >= m_nSize)
            if (!SetSize(nIndex + 1, -1)) {
                return FALSE;
            }
        ((TYPE*)m_pData)[nIndex] = newElement;
        return TRUE;
    }

    FX_BOOL		Add(TYPE newElement)
    {
        if (m_nSize < m_nMaxSize) {
            m_nSize ++;
        } else if (!SetSize(m_nSize + 1, -1)) {
            return FALSE;
        }
        ((TYPE*)m_pData)[m_nSize - 1] = newElement;
        return TRUE;
    }

    FX_BOOL		Append(const CFX_ArrayTemplate& src)
    {
        return CFX_BasicArray::Append(src);
    }

    FX_BOOL		Copy(const CFX_ArrayTemplate& src)
    {
        return CFX_BasicArray::Copy(src);
    }

    TYPE*		GetDataPtr(int index)
    {
        return (TYPE*)CFX_BasicArray::GetDataPtr(index);
    }

    TYPE*		AddSpace()
    {
        return (TYPE*)CFX_BasicArray::InsertSpaceAt(m_nSize, 1);
    }

    TYPE*		InsertSpaceAt(int nIndex, int nCount)
    {
        return (TYPE*)CFX_BasicArray::InsertSpaceAt(nIndex, nCount);
    }

    CFX_ArrayTemplate& operator=(const CFX_ArrayTemplate& src)
    {
        CFX_BasicArray::operator=(src);
        return *this;
    }

    const TYPE	operator[](int nIndex) const
    {
        if (nIndex < 0 || nIndex >= m_nSize) {
            *(volatile char*)0 = '\0';
        }
        return ((const TYPE*)m_pData)[nIndex];
    }

    TYPE&		operator[](int nIndex)
    {
        if (nIndex < 0 || nIndex >= m_nSize) {
            *(volatile char*)0 = '\0';
        }
        return ((TYPE*)m_pData)[nIndex];
    }

    FX_BOOL		InsertAt(int nIndex, TYPE newElement, int nCount = 1)
    {
        if (!InsertSpaceAt(nIndex, nCount)) {
            return FALSE;
        }
        while (nCount--) {
            ((TYPE*)m_pData)[nIndex++] = newElement;
        }
        return TRUE;
    }

    FX_BOOL		RemoveAt(int nIndex, int nCount = 1)
    {
        return CFX_BasicArray::RemoveAt(nIndex, nCount);
    }

    FX_BOOL		InsertAt(int nStartIndex, const CFX_BasicArray* pNewArray)
    {
        return CFX_BasicArray::InsertAt(nStartIndex, pNewArray);
    }

    int			Find(const TYPE& data, int iStart = 0) const
    {
        if (iStart < 0) {
            return -1;
        }
        for (; iStart < (int)m_nSize; iStart ++)
            if (((TYPE*)m_pData)[iStart] == data) {
                return iStart;
            }
        return -1;
    }
};
typedef CFX_ArrayTemplate<FX_BYTE>		CFX_ByteArray;
typedef CFX_ArrayTemplate<FX_WORD>		CFX_WordArray;
typedef CFX_ArrayTemplate<FX_DWORD>		CFX_DWordArray;
typedef CFX_ArrayTemplate<void*>		CFX_PtrArray;
typedef CFX_ArrayTemplate<FX_FILESIZE>	CFX_FileSizeArray;
typedef CFX_ArrayTemplate<FX_FLOAT>		CFX_FloatArray;
typedef CFX_ArrayTemplate<FX_INT32>		CFX_Int32Array;
template <class ObjectClass>
class CFX_ObjectArray : public CFX_BasicArray
{
public:
    CFX_ObjectArray() : CFX_BasicArray(sizeof(ObjectClass)) {}

    ~CFX_ObjectArray()
    {
        RemoveAll();
    }

    void			Add(const ObjectClass& data)
    {
        new ((void*)InsertSpaceAt(m_nSize, 1)) ObjectClass(data);
    }

    ObjectClass&	Add()
    {
        return *(ObjectClass*) new ((void*)InsertSpaceAt(m_nSize, 1)) ObjectClass();
    }

    void*			AddSpace()
    {
        return InsertSpaceAt(m_nSize, 1);
    }

    FX_INT32		Append(const CFX_ObjectArray& src, FX_INT32 nStart = 0, FX_INT32 nCount = -1)
    {
        if (nCount == 0) {
            return 0;
        }
        FX_INT32 size = src.GetSize();
        if (!size) {
            return 0;
        }
        FXSYS_assert(nStart > -1 && nStart < size);
        if (nCount < 0) {
            nCount = size;
        }
        if (nStart + nCount > size) {
            nCount = size - nStart;
        }
        if (nCount < 1) {
            return 0;
        }
        size = m_nSize;
        InsertSpaceAt(m_nSize, nCount);
        ObjectClass* pStartObj = (ObjectClass*)GetDataPtr(size);
        size = nStart + nCount;
        for (FX_INT32 i = nStart; i < size; i ++, pStartObj++) {
            new ((void*)pStartObj) ObjectClass(src[i]);
        }
        return nCount;
    }

    FX_INT32		Copy(const CFX_ObjectArray& src, FX_INT32 nStart = 0, FX_INT32 nCount = -1)
    {
        if (nCount == 0) {
            return 0;
        }
        FX_INT32 size = src.GetSize();
        if (!size) {
            return 0;
        }
        FXSYS_assert(nStart > -1 && nStart < size);
        if (nCount < 0) {
            nCount = size;
        }
        if (nStart + nCount > size) {
            nCount = size - nStart;
        }
        if (nCount < 1) {
            return 0;
        }
        RemoveAll();
        SetSize(nCount, -1);
        ObjectClass* pStartObj = (ObjectClass*)m_pData;
        size = nStart + nCount;
        for (FX_INT32 i = nStart; i < size; i ++, pStartObj++) {
            new ((void*)pStartObj) ObjectClass(src[i]);
        }
        return nCount;
    }

    int				GetSize() const
    {
        return m_nSize;
    }

    ObjectClass&	operator[] (int index) const
    {
        FXSYS_assert(index < m_nSize);
        return *(ObjectClass*)CFX_BasicArray::GetDataPtr(index);
    }

    ObjectClass*	GetDataPtr(int index) const
    {
        return (ObjectClass*)CFX_BasicArray::GetDataPtr(index);
    }

    void			RemoveAt(int index)
    {
        FXSYS_assert(index < m_nSize);
        ((ObjectClass*)GetDataPtr(index))->~ObjectClass();
        CFX_BasicArray::RemoveAt(index, 1);
    }

    void			RemoveAll()
    {
        for (int i = 0; i < m_nSize; i ++) {
            ((ObjectClass*)GetDataPtr(i))->~ObjectClass();
        }
        CFX_BasicArray::SetSize(0, -1);
    }
};
typedef CFX_ObjectArray<CFX_ByteString> CFX_ByteStringArray;
typedef CFX_ObjectArray<CFX_WideString> CFX_WideStringArray;
class CFX_BaseSegmentedArray : public CFX_Object
{
public:
    CFX_BaseSegmentedArray(int unit_size = 1, int segment_units = 512, int index_size = 8);

    ~CFX_BaseSegmentedArray();

    void	SetUnitSize(int unit_size, int segment_units, int index_size = 8);

    void*	Add();

    void*	GetAt(int index) const;

    void	RemoveAll();

    void	Delete(int index, int count = 1);

    int		GetSize() const
    {
        return m_DataSize;
    }

    int		GetSegmentSize() const
    {
        return m_SegmentSize;
    }

    int		GetUnitSize() const
    {
        return m_UnitSize;
    }

    void*	Iterate(FX_BOOL (*callback)(void* param, void* pData), void* param) const;
private:

    int				m_UnitSize;

    short			m_SegmentSize;

    FX_BYTE			m_IndexSize;

    FX_BYTE			m_IndexDepth;

    int				m_DataSize;

    void*			m_pIndex;
    void**	GetIndex(int seg_index) const;
    void*	IterateIndex(int level, int& start, void** pIndex, FX_BOOL (*callback)(void* param, void* pData), void* param) const;
    void*	IterateSegment(FX_LPCBYTE pSegment, int count, FX_BOOL (*callback)(void* param, void* pData), void* param) const;
};
template <class ElementType>
class CFX_SegmentedArray : public CFX_BaseSegmentedArray
{
public:
    CFX_SegmentedArray(int segment_units, int index_size = 8)
        : CFX_BaseSegmentedArray(sizeof(ElementType), segment_units, index_size)
    {}

    void	Add(ElementType data)
    {
        *(ElementType*)CFX_BaseSegmentedArray::Add() = data;
    }

    ElementType& operator [] (int index)
    {
        return *(ElementType*)CFX_BaseSegmentedArray::GetAt(index);
    }
};
template <class DataType, int FixedSize>
class CFX_FixedBufGrow : public CFX_Object
{
public:
    CFX_FixedBufGrow() : m_pData(NULL)
    {
        FXSYS_memset32(m_Data, 0, sizeof(DataType)*FixedSize);
    }
    CFX_FixedBufGrow(int data_size) : m_pData(NULL)
    {
        if (data_size > FixedSize) {
            m_pData = FX_Alloc(DataType, data_size);
        } else {
            FXSYS_memset32(m_Data, 0, sizeof(DataType)*FixedSize);
        }
    }
    void SetDataSize(int data_size)
    {
        if (m_pData) {
            FX_Free(m_pData);
        }
        m_pData = NULL;
        if (data_size > FixedSize) {
            m_pData = FX_Alloc(DataType, data_size);
        } else {
            FXSYS_memset32(m_Data, 0, sizeof(DataType)*FixedSize);
        }
    }
    ~CFX_FixedBufGrow()
    {
        if (m_pData) {
            FX_Free(m_pData);
        }
    }
    operator DataType*()
    {
        return m_pData ? m_pData : m_Data;
    }
private:
    DataType		m_Data[FixedSize];
    DataType*		m_pData;
};
template <class DataType>
class CFX_TempBuf
{
public:
    CFX_TempBuf(int size)
    {
        m_pData = FX_Alloc(DataType, size);
    }
    ~CFX_TempBuf()
    {
        if (m_pData) {
            FX_Free(m_pData);
        }
    }
    DataType&	operator[](int i)
    {
        FXSYS_assert(m_pData != NULL);
        return m_pData[i];
    }
    operator DataType*() const
    {
        return m_pData;
    }
private:
    DataType*		m_pData;
};
class CFX_MapPtrToPtr : public CFX_Object
{
protected:

    struct CAssoc {

        CAssoc* pNext;

        void* key;

        void* value;
    };
public:
    CFX_MapPtrToPtr(int nBlockSize = 10);

    ~CFX_MapPtrToPtr();

    int GetCount() const
    {
        return m_nCount;
    }

    FX_BOOL IsEmpty() const
    {
        return m_nCount == 0;
    }

    FX_BOOL Lookup(void* key, void*& rValue) const;

    void* GetValueAt(void* key) const;

    void*& operator[](void* key);

    void SetAt(void* key, void* newValue)
    {
        (*this)[key] = newValue;
    }

    FX_BOOL RemoveKey(void* key);

    void RemoveAll();

    FX_POSITION GetStartPosition() const
    {
        return (m_nCount == 0) ? NULL : (FX_POSITION) - 1;
    }

    void GetNextAssoc(FX_POSITION& rNextPosition, void*& rKey, void*& rValue) const;

    FX_DWORD GetHashTableSize() const
    {
        return m_nHashTableSize;
    }

    void InitHashTable(FX_DWORD hashSize, FX_BOOL bAllocNow = TRUE);
protected:

    CAssoc** m_pHashTable;

    FX_DWORD m_nHashTableSize;

    int m_nCount;

    CAssoc* m_pFreeList;

    struct CFX_Plex* m_pBlocks;

    int m_nBlockSize;

    FX_DWORD HashKey(void* key) const;

    CAssoc* NewAssoc();

    void FreeAssoc(CAssoc* pAssoc);

    CAssoc* GetAssocAt(void* key, FX_DWORD& hash) const;

    CAssoc* GetCurrentAssoc(FX_POSITION& rNextPosition) const;

    FX_BOOL ExpandHashTable();
};
template <class KeyType, class ValueType>
class CFX_MapPtrTemplate : public CFX_MapPtrToPtr
{
public:
    CFX_MapPtrTemplate() : CFX_MapPtrToPtr(10) {}

    FX_BOOL	Lookup(KeyType key, ValueType& rValue) const
    {
        FX_LPVOID pValue = NULL;
        if (!CFX_MapPtrToPtr::Lookup((void*)(FX_UINTPTR)key, pValue)) {
            return FALSE;
        }
        rValue = (ValueType)(FX_UINTPTR)pValue;
        return TRUE;
    }

    ValueType& operator[](KeyType key)
    {
        return (ValueType&)CFX_MapPtrToPtr::operator []((void*)(FX_UINTPTR)key);
    }

    void SetAt(KeyType key, ValueType newValue)
    {
        CFX_MapPtrToPtr::SetAt((void*)(FX_UINTPTR)key, (void*)(FX_UINTPTR)newValue);
    }

    FX_BOOL	RemoveKey(KeyType key)
    {
        return CFX_MapPtrToPtr::RemoveKey((void*)(FX_UINTPTR)key);
    }

    void GetNextAssoc(FX_POSITION& rNextPosition, KeyType& rKey, ValueType& rValue) const
    {
        void* pKey = NULL;
        void* pValue = NULL;
        CFX_MapPtrToPtr::GetNextAssoc(rNextPosition, pKey, pValue);
        rKey = (KeyType)(FX_UINTPTR)pKey;
        rValue = (ValueType)(FX_UINTPTR)pValue;
    }
};
class CFX_CMapDWordToDWord : public CFX_Object
{
public:

    FX_BOOL			Lookup(FX_DWORD key, FX_DWORD& value) const;

    void			SetAt(FX_DWORD key, FX_DWORD value);

    void			EstimateSize(FX_DWORD size, FX_DWORD grow_by);

    FX_POSITION		GetStartPosition() const;

    void			GetNextAssoc(FX_POSITION& pos, FX_DWORD& key, FX_DWORD& value) const;
protected:

    CFX_BinaryBuf	m_Buffer;
};
class CFX_MapByteStringToPtr : public CFX_Object
{
protected:

    struct CAssoc {

        CAssoc* pNext;

        FX_DWORD nHashValue;

        CFX_ByteString key;

        void* value;
    };
public:
    CFX_MapByteStringToPtr(int nBlockSize = 10);

    int GetCount() const
    {
        return m_nCount;
    }

    FX_BOOL IsEmpty() const
    {
        return m_nCount == 0;
    }

    FX_BOOL Lookup(FX_BSTR key, void*& rValue) const;

    void*& operator[](FX_BSTR key);

    void SetAt(FX_BSTR key, void* newValue)
    {
        (*this)[key] = newValue;
    }

    FX_BOOL RemoveKey(FX_BSTR key);

    void RemoveAll();

    FX_POSITION GetStartPosition() const
    {
        return (m_nCount == 0) ? NULL : (FX_POSITION) - 1;
    }

    void GetNextAssoc(FX_POSITION& rNextPosition, CFX_ByteString& rKey, void*& rValue) const;

    FX_LPVOID		GetNextValue(FX_POSITION& rNextPosition) const;

    FX_DWORD GetHashTableSize() const
    {
        return m_nHashTableSize;
    }

    void InitHashTable(FX_DWORD hashSize, FX_BOOL bAllocNow = TRUE);

    FX_DWORD HashKey(FX_BSTR key) const;
protected:

    CAssoc** m_pHashTable;

    FX_DWORD m_nHashTableSize;

    int m_nCount;

    CAssoc* m_pFreeList;

    struct CFX_Plex* m_pBlocks;

    int m_nBlockSize;

    CAssoc* NewAssoc();

    void FreeAssoc(CAssoc* pAssoc);

    CAssoc* GetAssocAt(FX_BSTR key, FX_DWORD& hash) const;

    CAssoc* GetCurrentAssoc(FX_POSITION& rNextPosition) const;

    FX_BOOL ExpendHashTable();
public:

    ~CFX_MapByteStringToPtr();
};
class CFX_CMapByteStringToPtr : public CFX_Object
{
public:
    CFX_CMapByteStringToPtr();

    ~CFX_CMapByteStringToPtr();

    void			RemoveAll();

    FX_POSITION		GetStartPosition() const;

    void			GetNextAssoc(FX_POSITION& rNextPosition, CFX_ByteString& rKey, void*& rValue) const;

    FX_LPVOID		GetNextValue(FX_POSITION& rNextPosition) const;

    FX_BOOL			Lookup(FX_BSTR key, void*& rValue) const;

    void			SetAt(FX_BSTR key, void* value);

    void			RemoveKey(FX_BSTR key);

    int				GetCount() const;

    void			AddValue(FX_BSTR key, void* pValue);
protected:

    CFX_BaseSegmentedArray			m_Buffer;
};
class CFX_PtrList : public CFX_Object
{
protected:

    struct CNode {

        CNode* pNext;

        CNode* pPrev;

        void* data;
    };
public:
    CFX_PtrList(int nBlockSize = 10);

    FX_POSITION GetHeadPosition() const
    {
        return (FX_POSITION)m_pNodeHead;
    }

    FX_POSITION GetTailPosition() const
    {
        return (FX_POSITION)m_pNodeTail;
    }

    void*	GetNext(FX_POSITION& rPosition) const
    {
        CNode* pNode = (CNode*) rPosition;
        rPosition = (FX_POSITION) pNode->pNext;
        return pNode->data;
    }

    void*	GetPrev(FX_POSITION& rPosition) const
    {
        CNode* pNode = (CNode*) rPosition;
        rPosition = (FX_POSITION) pNode->pPrev;
        return pNode->data;
    }

    FX_POSITION	GetNextPosition(FX_POSITION pos) const
    {
        return ((CNode*)pos)->pNext;
    }

    FX_POSITION	GetPrevPosition(FX_POSITION pos) const
    {
        return ((CNode*)pos)->pPrev;
    }

    void*	GetAt(FX_POSITION rPosition) const
    {
        CNode* pNode = (CNode*) rPosition;
        return pNode ? pNode->data : NULL;
    }

    int		GetCount() const
    {
        return m_nCount;
    }

    FX_POSITION	AddTail(void* newElement);

    FX_POSITION AddHead(void* newElement);

    void	SetAt(FX_POSITION pos, void* newElement)
    {
        CNode* pNode = (CNode*) pos;
        pNode->data = newElement;
    }

    FX_POSITION InsertAfter(FX_POSITION pos, void* newElement);

    FX_POSITION Find(void* searchValue, FX_POSITION startAfter = NULL ) const;

    FX_POSITION FindIndex(int index) const;

    void	RemoveAt(FX_POSITION pos);

    void	RemoveAll();
protected:

    CNode* m_pNodeHead;

    CNode* m_pNodeTail;

    int m_nCount;

    CNode* m_pNodeFree;

    struct CFX_Plex* m_pBlocks;

    int m_nBlockSize;

    CNode* NewNode(CNode* pPrev, CNode* pNext);

    void FreeNode(CNode* pNode);
public:

    ~CFX_PtrList();
};
typedef void (*PD_CALLBACK_FREEDATA)(FX_LPVOID pData);
struct FX_PRIVATEDATA {

    void					FreeData();

    FX_LPVOID				m_pModuleId;

    FX_LPVOID				m_pData;

    PD_CALLBACK_FREEDATA	m_pCallback;

    FX_BOOL					m_bSelfDestruct;
};
class CFX_PrivateData
{
public:

    ~CFX_PrivateData();

    void					ClearAll();

    void					SetPrivateData(FX_LPVOID module_id, FX_LPVOID pData, PD_CALLBACK_FREEDATA callback);

    void					SetPrivateObj(FX_LPVOID module_id, CFX_DestructObject* pObj);

    FX_LPVOID				GetPrivateData(FX_LPVOID module_id);

    FX_BOOL					LookupPrivateData(FX_LPVOID module_id, FX_LPVOID &pData) const
    {
        if (!module_id) {
            return FALSE;
        }
        FX_DWORD nCount = m_DataList.GetSize();
        for (FX_DWORD n = 0; n < nCount; n ++) {
            if (m_DataList[n].m_pModuleId == module_id) {
                pData = m_DataList[n].m_pData;
                return TRUE;
            }
        }
        return FALSE;
    }

    FX_BOOL					RemovePrivateData(FX_LPVOID module_id);
protected:

    CFX_ArrayTemplate<FX_PRIVATEDATA>	m_DataList;

    void					AddData(FX_LPVOID module_id, FX_LPVOID pData, PD_CALLBACK_FREEDATA callback, FX_BOOL bSelfDestruct);
};
class CFX_BitStream : public CFX_Object
{
public:

    void				Init(FX_LPCBYTE pData, FX_DWORD dwSize);


    FX_DWORD			GetBits(FX_DWORD nBits);

    void				ByteAlign();

    FX_BOOL				IsEOF() const
    {
        return m_BitPos >= m_BitSize;
    }

    void				SkipBits(FX_DWORD nBits)
    {
        m_BitPos += nBits;
    }

    void				Rewind()
    {
        m_BitPos = 0;
    }
protected:

    FX_DWORD			m_BitPos;

    FX_DWORD			m_BitSize;

    FX_LPCBYTE			m_pData;
};
template <class ObjClass> class CFX_CountRef : public CFX_Object
{
public:

    typedef CFX_CountRef<ObjClass> Ref;

    class CountedObj : public ObjClass
    {
    public:

        CountedObj(): m_RefCount(0) {}

        CountedObj(const CountedObj& src) : ObjClass(src) , m_RefCount(0) {}

        int			m_RefCount;
    };

    CFX_CountRef()
    {
        m_pObject = NULL;
    }

    CFX_CountRef(const Ref& ref)
    {
        m_pObject = ref.m_pObject;
        if (m_pObject) {
            m_pObject->m_RefCount ++;
        }
    }

    ~CFX_CountRef()
    {
        if (!m_pObject) {
            return;
        }
        m_pObject->m_RefCount --;
        if (m_pObject->m_RefCount <= 0) {
            delete m_pObject;
            m_pObject = NULL;
        }
    }

    ObjClass*			New()
    {
        if (m_pObject) {
            m_pObject->m_RefCount --;
            if (m_pObject->m_RefCount <= 0) {
                delete m_pObject;
            }
            m_pObject = NULL;
        }
        m_pObject = FX_NEW CountedObj;
        if (!m_pObject) {
            return NULL;
        }
        m_pObject->m_RefCount = 1;
        return m_pObject;
    }

    void				operator = (const Ref& ref)
    {
        if (ref.m_pObject) {
            ref.m_pObject->m_RefCount ++;
        }
        if (m_pObject) {
            m_pObject->m_RefCount --;
            if (m_pObject->m_RefCount <= 0) {
                delete m_pObject;
            }
        }
        m_pObject = ref.m_pObject;
    }

    void				operator = (void* p)
    {
        FXSYS_assert(p == 0);
        if (m_pObject == NULL) {
            return;
        }
        m_pObject->m_RefCount --;
        if (m_pObject->m_RefCount <= 0) {
            delete m_pObject;
        }
        m_pObject = NULL;
    }

    const ObjClass*		GetObject() const
    {
        return m_pObject;
    }

    operator			const ObjClass*() const
    {
        return m_pObject;
    }

    FX_BOOL				IsNull() const
    {
        return m_pObject == NULL;
    }

    FX_BOOL				NotNull() const
    {
        return m_pObject != NULL;
    }

    ObjClass*			GetModify()
    {
        if (m_pObject == NULL) {
            m_pObject = FX_NEW CountedObj;
            if (m_pObject) {
                m_pObject->m_RefCount = 1;
            }
        } else if (m_pObject->m_RefCount > 1) {
            m_pObject->m_RefCount --;
            CountedObj* pOldObject = m_pObject;
            m_pObject = NULL;
            m_pObject = FX_NEW CountedObj(*pOldObject);
            if (m_pObject) {
                m_pObject->m_RefCount = 1;
            }
        }
        return m_pObject;
    }

    void				SetNull()
    {
        if (m_pObject == NULL) {
            return;
        }
        m_pObject->m_RefCount --;
        if (m_pObject->m_RefCount <= 0) {
            delete m_pObject;
        }
        m_pObject = NULL;
    }

    FX_BOOL				operator == (const Ref& ref) const
    {
        return m_pObject == ref.m_pObject;
    }
    int					RefCount() const
    {
        return m_pObject ? m_pObject->m_RefCount : 0;
    }
    void				Incref()
    {
        if (m_pObject == NULL) {
            return;
        }
        m_pObject->m_RefCount++;
    }
    void				Decref()
    {
        if (m_pObject == NULL) {
            return;
        }
        m_pObject->m_RefCount--;
        if (m_pObject->m_RefCount <= 0) {
            delete m_pObject;
            m_pObject = NULL;
        }
    }
protected:

    CountedObj*			m_pObject;
};
class IFX_Pause
{
public:

    virtual FX_BOOL	NeedToPauseNow() = 0;
};
class CFX_DataFilter : public CFX_Object
{
public:

    virtual ~CFX_DataFilter();

    void			SetDestFilter(CFX_DataFilter* pFilter);

    FX_BOOL			IsEOF() const
    {
        return m_bEOF;
    }

    FX_FILESIZE		GetSrcPos() const
    {
        return m_SrcPos;
    }

    void			FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf);

    void			FilterFinish(CFX_BinaryBuf& dest_buf);

    FX_BOOL			IsExhaustBuffer() const
    {
        return m_bExhaustBuffer;
    }

    FX_BOOL			NeedNewSrc();

    FX_BOOL			Abort() const
    {
        return m_bAbort;
    }

    FX_BOOL			AbortAll();
    void			ResetStatistics();
protected:

    CFX_DataFilter();
    virtual void	v_FilterIn(FX_LPCBYTE src_buf, size_t src_size, CFX_BinaryBuf& dest_buf) = 0;
    virtual void	v_FilterFinish(CFX_BinaryBuf& dest_buf) = 0;
    virtual void	v_ResetStatistics() {};
    void			ReportEOF(FX_DWORD left_input);

    FX_BOOL			m_bEOF;

    FX_FILESIZE		m_SrcPos;

    CFX_DataFilter*	m_pDestFilter;

    FX_BOOL			m_bExhaustBuffer;

    CFX_BinaryBuf	m_FilterInBuffer;

    FX_BOOL			m_bAbort;
};
template<typename T>
class CFX_AutoRestorer
{
public:
    explicit CFX_AutoRestorer(T* location)
    {
        m_Location = location;
        m_OldValue = *location;
    }
    ~CFX_AutoRestorer()
    {
        *m_Location = m_OldValue;
    }
private:
    T* m_Location;
    T m_OldValue;
};
template <class T>
class CFX_SmartPointer
{
public:
    CFX_SmartPointer(T *pObj) : m_pObj(pObj) {}
    ~CFX_SmartPointer()
    {
        m_pObj->Release();
    }
    T*		Get(void)
    {
        return m_pObj;
    }
    T&		operator *(void)
    {
        return *m_pObj;
    }
    T*		operator ->(void)
    {
        return m_pObj;
    }
protected:
    T *m_pObj;
};
#define FX_DATALIST_LENGTH	1024
template<size_t unit>
class CFX_SortListArray : public CFX_Object
{
protected:

    struct DataList {

        FX_INT32	start;

        FX_INT32	count;
        FX_LPBYTE	data;
    };
public:

    CFX_SortListArray() : m_CurList(0) {}

    ~CFX_SortListArray()
    {
        Clear();
    }


    void			Clear()
    {
        for (FX_INT32 i = m_DataLists.GetUpperBound(); i >= 0; i--) {
            DataList list = m_DataLists.ElementAt(i);
            if (list.data) {
                FX_Free(list.data);
            }
        }
        m_DataLists.RemoveAll();
        m_CurList = 0;
    }

    void			Append(FX_INT32 nStart, FX_INT32 nCount)
    {
        if (nStart < 0) {
            return;
        }
        while (nCount > 0) {
            FX_INT32 temp_count = FX_MIN(nCount, FX_DATALIST_LENGTH);
            DataList list;
            list.data = FX_Alloc(FX_BYTE, temp_count * unit);
            if (!list.data) {
                break;
            }
            FXSYS_memset32(list.data, 0, temp_count * unit);
            list.start = nStart;
            list.count = temp_count;
            FX_BOOL ret = Append(list);
            if(ret) {
                nCount -= temp_count;
                nStart += temp_count;
            } else {
                if (list.data) {
                    FX_Free(list.data);
                }
            }
        }
    }

    FX_LPBYTE		GetAt(FX_INT32 nIndex)
    {
        if (nIndex < 0) {
            return NULL;
        }
        if (m_CurList < 0 || m_CurList >= m_DataLists.GetSize()) {
            return NULL;
        }
        DataList *pCurList = m_DataLists.GetDataPtr(m_CurList);
        if (!pCurList || nIndex < pCurList->start || nIndex >= pCurList->start + pCurList->count) {
            pCurList = NULL;
            FX_INT32 iStart = 0;
            FX_INT32 iEnd = m_DataLists.GetUpperBound();
            while (iStart <= iEnd) {
                FX_INT32 iMid = (iStart + iEnd) / 2;
                DataList* list = m_DataLists.GetDataPtr(iMid);
                if (nIndex < list->start) {
                    iEnd = iMid - 1;
                } else if (nIndex >= list->start + list->count) {
                    iStart = iMid + 1;
                } else {
                    pCurList = list;
                    m_CurList = iMid;
                    break;
                }
            }
        }
        return pCurList ? pCurList->data + (nIndex - pCurList->start) * unit : NULL;
    }
protected:
    FX_BOOL			Append(const DataList& list)
    {
        FX_INT32 iStart = 0;
        FX_INT32 iEnd = m_DataLists.GetUpperBound();
        FX_INT32 iFind = 0;
        while (iStart <= iEnd) {
            FX_INT32 iMid = (iStart + iEnd) / 2;
            DataList* cur_list = m_DataLists.GetDataPtr(iMid);
            if (list.start == cur_list->start) {
                return FALSE;
            } else if (list.start < cur_list->start + cur_list->count) {
                iEnd = iMid - 1;
            } else {
                if (iMid == iEnd) {
                    iFind = iMid + 1;
                    break;
                }
                DataList* next_list = m_DataLists.GetDataPtr(iMid + 1);
                if (list.start == next_list->start) {
                    return FALSE;
                } else if (list.start < next_list->start) {
                    iFind = iMid + 1;
                    break;
                } else {
                    iStart = iMid + 1;
                }
            }
        }
        m_DataLists.InsertAt(iFind, list);
        return TRUE;
    }
    FX_INT32		m_CurList;
    CFX_ArrayTemplate<DataList>	m_DataLists;
};
template<typename T1, typename T2>
class CFX_ListArrayTemplate : public CFX_Object
{
public:

    void			Clear()
    {
        m_Data.Clear();
    }

    void			Add(FX_INT32 nStart, FX_INT32 nCount)
    {
        m_Data.Append(nStart, nCount);
    }

    T2&				operator [] (FX_INT32 nIndex)
    {
        FX_LPBYTE data = m_Data.GetAt(nIndex);
        FXSYS_assert(data != NULL);
        return (T2&)(*(volatile T2*)data);
    }

    T2*				GetPtrAt(FX_INT32 nIndex)
    {
        return (T2*)m_Data.GetAt(nIndex);
    }
protected:
    T1			m_Data;
};
typedef CFX_ListArrayTemplate<CFX_SortListArray<sizeof(FX_FILESIZE)>, FX_FILESIZE>	CFX_FileSizeListArray;
typedef CFX_ListArrayTemplate<CFX_SortListArray<sizeof(FX_DWORD)>, FX_DWORD>		CFX_DWordListArray;
typedef enum {
    Ready,
    ToBeContinued,
    Found,
    NotFound,
    Failed,
    Done
} FX_ProgressiveStatus;
#define ProgressiveStatus	FX_ProgressiveStatus
#define FX_NAMESPACE_DECLARE(namespace, type)       namespace::type
#endif
