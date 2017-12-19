#ifndef _FX_STRING_H_
#define _FX_STRING_H_
class CFX_ByteStringC;
class CFX_ByteString;
class CFX_WideStringC;
class CFX_WideString;
struct CFX_CharMap;
class CFX_BinaryBuf;
typedef int FX_STRSIZE;
class CFX_ByteStringC : public CFX_Object
{
public:

    CFX_ByteStringC()
    {
        m_Ptr = NULL;
        m_Length = 0;
    }

    CFX_ByteStringC(FX_LPCBYTE ptr, FX_STRSIZE size)
    {
        m_Ptr = ptr;
        m_Length = size;
    }

    CFX_ByteStringC(FX_LPCSTR ptr)
    {
        m_Ptr = (FX_LPCBYTE)ptr;
        m_Length = ptr ? (FX_STRSIZE)FXSYS_strlen(ptr) : 0;
    }

    explicit CFX_ByteStringC(const FX_CHAR& ch)
    {
        m_Ptr = (FX_LPCBYTE)&ch;
        m_Length = 1;
    }

    CFX_ByteStringC(FX_LPCSTR ptr, FX_STRSIZE len)
    {
        m_Ptr = (FX_LPCBYTE)ptr;
        if (len == -1) {
            m_Length = (FX_STRSIZE)FXSYS_strlen(ptr);
        } else {
            m_Length = len;
        }
    }

    CFX_ByteStringC(const CFX_ByteStringC& src)
    {
        m_Ptr = src.m_Ptr;
        m_Length = src.m_Length;
    }

    CFX_ByteStringC(const CFX_ByteString& src);

    CFX_ByteStringC& operator = (FX_LPCSTR src)
    {
        m_Ptr = (FX_LPCBYTE)src;
        m_Length = m_Ptr ? (FX_STRSIZE)FXSYS_strlen(src) : 0;
        return *this;
    }

    CFX_ByteStringC& operator = (const CFX_ByteStringC& src)
    {
        m_Ptr = src.m_Ptr;
        m_Length = src.m_Length;
        return *this;
    }

    CFX_ByteStringC& operator = (const CFX_ByteString& src);

    bool			operator == (const CFX_ByteStringC& str) const
    {
        return 	str.m_Length == m_Length && FXSYS_memcmp32(str.m_Ptr, m_Ptr, m_Length) == 0;
    }

    bool			operator != (const CFX_ByteStringC& str) const
    {
        return 	str.m_Length != m_Length || FXSYS_memcmp32(str.m_Ptr, m_Ptr, m_Length) != 0;
    }
#define FXBSTR_ID(c1, c2, c3, c4) ((c1 << 24) | (c2 << 16) | (c3 << 8) | (c4))

    FX_DWORD		GetID(FX_STRSIZE start_pos = 0) const;

    FX_LPCBYTE		GetPtr() const
    {
        return m_Ptr;
    }

    FX_LPCSTR		GetCStr() const
    {
        return (FX_LPCSTR)m_Ptr;
    }

    FX_STRSIZE		GetLength() const
    {
        return m_Length;
    }

    bool			IsEmpty() const
    {
        return m_Length == 0;
    }

    operator		FX_LPCBYTE() const
    {
        return m_Ptr;
    }

    FX_BYTE			GetAt(FX_STRSIZE index) const
    {
        return m_Ptr[index];
    }

    CFX_ByteStringC	Mid(FX_STRSIZE index, FX_STRSIZE count = -1) const
    {
        if (index < 0) {
            index = 0;
        }
        if (index > m_Length) {
            return CFX_ByteStringC();
        }
        if (count < 0 || count > m_Length - index) {
            count = m_Length - index;
        }
        return CFX_ByteStringC(m_Ptr + index, count);
    }
protected:

    FX_LPCBYTE		m_Ptr;

    FX_STRSIZE		m_Length;
private:

    void*			operator new (size_t) throw()
    {
        return NULL;
    }
};
typedef const CFX_ByteStringC& FX_BSTR;
#define FX_BSTRC(str) CFX_ByteStringC(str, sizeof str-1)
struct CFX_StringData {

    long		m_nRefs;

    FX_STRSIZE	m_nDataLength;

    FX_STRSIZE	m_nAllocLength;

    FX_CHAR		m_String[1];
};
class CFX_ByteString : public CFX_Object
{
public:

    CFX_ByteString()
    {
        m_pData = NULL;
    }

    CFX_ByteString(const CFX_ByteString& str);

    explicit CFX_ByteString(char ch);

    CFX_ByteString(FX_LPCSTR ptr, FX_STRSIZE len = -1);

    CFX_ByteString(FX_LPCBYTE ptr, FX_STRSIZE len);

    CFX_ByteString(FX_BSTR bstrc);

    CFX_ByteString(FX_BSTR bstrc1, FX_BSTR bstrc2);

    ~CFX_ByteString();

    static CFX_ByteString	FromUnicode(FX_LPCWSTR ptr, FX_STRSIZE len = -1);

    static CFX_ByteString	FromUnicode(const CFX_WideString& str);

    operator				FX_LPCSTR() const
    {
        return m_pData ? m_pData->m_String : "";
    }

    operator				FX_LPCBYTE() const
    {
        return m_pData ? (FX_LPCBYTE)m_pData->m_String : NULL;
    }

    FX_STRSIZE				GetLength() const
    {
        return m_pData ? m_pData->m_nDataLength : 0;
    }

    bool					IsEmpty() const
    {
        return !GetLength();
    }

    int						Compare(FX_BSTR str) const;


    bool					Equal(FX_BSTR str) const;


    bool					EqualNoCase(FX_BSTR str) const;

    bool					operator == (FX_LPCSTR str) const
    {
        return Equal(str);
    }

    bool					operator == (FX_BSTR str) const
    {
        return Equal(str);
    }

    bool					operator == (const CFX_ByteString& str) const;

    bool					operator != (FX_LPCSTR str) const
    {
        return !Equal(str);
    }

    bool					operator != (FX_BSTR str) const
    {
        return !Equal(str);
    }

    bool					operator != (const CFX_ByteString& str) const
    {
        return !operator==(str);
    }

    void					Empty();

    const CFX_ByteString&	operator = (FX_LPCSTR str);

    const CFX_ByteString&	operator = (FX_BSTR bstrc);

    const CFX_ByteString&	operator = (const CFX_ByteString& stringSrc);

    const CFX_ByteString&	operator = (const CFX_BinaryBuf& buf);

    void					Load(FX_LPCBYTE str, FX_STRSIZE len);

    const CFX_ByteString&	operator += (FX_CHAR ch);

    const CFX_ByteString&	operator += (FX_LPCSTR str);

    const CFX_ByteString&	operator += (const CFX_ByteString& str);

    const CFX_ByteString&	operator += (FX_BSTR bstrc);

    FX_BYTE					GetAt(FX_STRSIZE nIndex) const
    {
        return m_pData ? m_pData->m_String[nIndex] : 0;
    }

    FX_BYTE					operator[](FX_STRSIZE nIndex) const
    {
        return m_pData ? m_pData->m_String[nIndex] : 0;
    }

    void					SetAt(FX_STRSIZE nIndex, FX_CHAR ch);

    FX_STRSIZE				Insert(FX_STRSIZE index, FX_CHAR ch);

    FX_STRSIZE				Delete(FX_STRSIZE index, FX_STRSIZE count = 1);


    void					Format(FX_LPCSTR lpszFormat, ... );

    void					FormatV(FX_LPCSTR lpszFormat, va_list argList);


    void					Reserve(FX_STRSIZE len);

    FX_LPSTR				GetBuffer(FX_STRSIZE len);

    FX_LPSTR				LockBuffer();

    void					ReleaseBuffer(FX_STRSIZE len = -1);

    CFX_ByteString			Mid(FX_STRSIZE first) const;

    CFX_ByteString			Mid(FX_STRSIZE first, FX_STRSIZE count) const;

    CFX_ByteString			Left(FX_STRSIZE count) const;

    CFX_ByteString			Right(FX_STRSIZE count) const;

    FX_STRSIZE				Find(FX_BSTR lpszSub, FX_STRSIZE start = 0) const;

    FX_STRSIZE				Find(FX_CHAR ch, FX_STRSIZE start = 0) const;

    FX_STRSIZE				ReverseFind(FX_CHAR ch) const;

    void					MakeLower();

    void					MakeUpper();

    void					TrimRight();

    void					TrimRight(FX_CHAR chTarget);

    void					TrimRight(FX_BSTR lpszTargets);

    void					TrimLeft();

    void					TrimLeft(FX_CHAR chTarget);

    void					TrimLeft(FX_BSTR lpszTargets);

    FX_STRSIZE				Replace(FX_BSTR lpszOld, FX_BSTR lpszNew);

    FX_STRSIZE				Remove(FX_CHAR ch);

    CFX_WideString			UTF8Decode() const;

    void					ConvertFrom(const CFX_WideString& str, CFX_CharMap* pCharMap = NULL);

    FX_DWORD				GetID(FX_STRSIZE start_pos = 0) const;

    static CFX_ByteString	LoadFromFile(FX_BSTR file_path);
#define FXFORMAT_SIGNED			1
#define FXFORMAT_HEX			2
#define FXFORMAT_CAPITAL		4

    static CFX_ByteString	FormatInteger(FX_INT32 i, FX_DWORD flags = 0);

    static CFX_ByteString	FormatFloat(FX_FLOAT f, int precision = 0);
protected:

    struct CFX_StringData*	m_pData;
    void					AllocCopy(CFX_ByteString& dest, FX_STRSIZE nCopyLen, FX_STRSIZE nCopyIndex, FX_STRSIZE nExtraLen) const;
    void					AssignCopy(FX_STRSIZE nSrcLen, FX_LPCSTR lpszSrcData);
    void					ConcatCopy(FX_STRSIZE nSrc1Len, FX_LPCSTR lpszSrc1Data, FX_STRSIZE nSrc2Len, FX_LPCSTR lpszSrc2Data);
    void					ConcatInPlace(FX_STRSIZE nSrcLen, FX_LPCSTR lpszSrcData);
    void					CopyBeforeWrite();
    void					AllocBeforeWrite(FX_STRSIZE nLen);
};
inline CFX_ByteStringC::CFX_ByteStringC(const CFX_ByteString& src)
{
    m_Ptr = (FX_LPCBYTE)src;
    m_Length = src.GetLength();
}
inline CFX_ByteStringC& CFX_ByteStringC::operator = (const CFX_ByteString& src)
{
    m_Ptr = (FX_LPCBYTE)src;
    m_Length = src.GetLength();
    return *this;
}

inline CFX_ByteString operator + (FX_BSTR str1, FX_BSTR str2)
{
    return CFX_ByteString(str1, str2);
}
inline CFX_ByteString operator + (FX_BSTR str1, FX_LPCSTR str2)
{
    return CFX_ByteString(str1, str2);
}
inline CFX_ByteString operator + (FX_LPCSTR str1, FX_BSTR str2)
{
    return CFX_ByteString(str1, str2);
}
inline CFX_ByteString operator + (FX_BSTR str1, FX_CHAR ch)
{
    return CFX_ByteString(str1, CFX_ByteStringC(ch));
}
inline CFX_ByteString operator + (FX_CHAR ch, FX_BSTR str2)
{
    return CFX_ByteString(CFX_ByteStringC(ch), str2);
}
inline CFX_ByteString operator + (const CFX_ByteString& str1, const CFX_ByteString& str2)
{
    return CFX_ByteString(str1, str2);
}
inline CFX_ByteString operator + (const CFX_ByteString& str1, FX_CHAR ch)
{
    return CFX_ByteString(str1, CFX_ByteStringC(ch));
}
inline CFX_ByteString operator + (FX_CHAR ch, const CFX_ByteString& str2)
{
    return CFX_ByteString(CFX_ByteStringC(ch), str2);
}
inline CFX_ByteString operator + (const CFX_ByteString& str1, FX_LPCSTR str2)
{
    return CFX_ByteString(str1, str2);
}
inline CFX_ByteString operator + (FX_LPCSTR str1, const CFX_ByteString& str2)
{
    return CFX_ByteString(str1, str2);
}
inline CFX_ByteString operator + (const CFX_ByteString& str1, FX_BSTR str2)
{
    return CFX_ByteString(str1, str2);
}
inline CFX_ByteString operator + (FX_BSTR str1, const CFX_ByteString& str2)
{
    return CFX_ByteString(str1, str2);
}
class CFX_WideStringC : public CFX_Object
{
public:

    CFX_WideStringC()
    {
        m_Ptr = NULL;
        m_Length = 0;
    }

    CFX_WideStringC(FX_LPCWSTR ptr)
    {
        m_Ptr = ptr;
        m_Length = ptr ? (FX_STRSIZE)FXSYS_wcslen(ptr) : 0;
    }

    CFX_WideStringC(FX_WCHAR& ch)
    {
        m_Ptr = &ch;
        m_Length = 1;
    }

    CFX_WideStringC(FX_LPCWSTR ptr, FX_STRSIZE len)
    {
        m_Ptr = ptr;
        if (len == -1) {
            m_Length = (FX_STRSIZE)FXSYS_wcslen(ptr);
        } else {
            m_Length = len;
        }
    }

    CFX_WideStringC(const CFX_WideStringC& src)
    {
        m_Ptr = src.m_Ptr;
        m_Length = src.m_Length;
    }

    CFX_WideStringC(const CFX_WideString& src);

    CFX_WideStringC& operator = (FX_LPCWSTR src)
    {
        m_Ptr = src;
        m_Length = (FX_STRSIZE)FXSYS_wcslen(src);
        return *this;
    }

    CFX_WideStringC& operator = (const CFX_WideStringC& src)
    {
        m_Ptr = src.m_Ptr;
        m_Length = src.m_Length;
        return *this;
    }

    CFX_WideStringC& operator = (const CFX_WideString& src);

    bool			operator == (const CFX_WideStringC& str) const
    {
        return 	str.m_Length == m_Length && FXSYS_memcmp32(str.m_Ptr, m_Ptr, m_Length * sizeof(FX_WCHAR)) == 0;
    }

    bool			operator != (const CFX_WideStringC& str) const
    {
        return 	str.m_Length != m_Length || FXSYS_memcmp32(str.m_Ptr, m_Ptr, m_Length * sizeof(FX_WCHAR)) != 0;
    }

    FX_LPCWSTR		GetPtr() const
    {
        return m_Ptr;
    }

    FX_STRSIZE		GetLength() const
    {
        return m_Length;
    }

    bool			IsEmpty() const
    {
        return m_Length == 0;
    }

    FX_WCHAR		GetAt(FX_STRSIZE index) const
    {
        return m_Ptr[index];
    }

    CFX_WideStringC	Left(FX_STRSIZE count) const
    {
        if (count < 1) {
            return CFX_WideStringC();
        }
        if (count > m_Length) {
            count = m_Length;
        }
        return CFX_WideStringC(m_Ptr, count);
    }

    CFX_WideStringC	Mid(FX_STRSIZE index, FX_STRSIZE count = -1) const
    {
        if (index < 0) {
            index = 0;
        }
        if (index > m_Length) {
            return CFX_WideStringC();
        }
        if (count < 0 || count > m_Length - index) {
            count = m_Length - index;
        }
        return CFX_WideStringC(m_Ptr + index, count);
    }

    CFX_WideStringC	Right(FX_STRSIZE count) const
    {
        if (count < 1) {
            return CFX_WideStringC();
        }
        if (count > m_Length) {
            count = m_Length;
        }
        return CFX_WideStringC(m_Ptr + m_Length - count, count);
    }
protected:

    FX_LPCWSTR		m_Ptr;

    FX_STRSIZE		m_Length;
private:

    void*			operator new (size_t) throw()
    {
        return NULL;
    }
};
typedef const CFX_WideStringC&	FX_WSTR;
#define FX_WSTRC(wstr) CFX_WideStringC((FX_LPCWSTR)wstr, sizeof(wstr) / sizeof(FX_WCHAR) - 1)
struct CFX_StringDataW {

    long		m_nRefs;

    FX_STRSIZE	m_nDataLength;

    FX_STRSIZE	m_nAllocLength;

    FX_WCHAR	m_String[1];
};
class CFX_WideString : public CFX_Object
{
public:

    CFX_WideString()
    {
        m_pData = NULL;
    }

    CFX_WideString(const CFX_WideString& str);

    CFX_WideString(FX_LPCWSTR ptr, FX_STRSIZE len = -1)
    {
        InitStr(ptr, len);
    }

    CFX_WideString(FX_WCHAR ch);

    CFX_WideString(const CFX_WideStringC& str);

    CFX_WideString(const CFX_WideStringC& str1, const CFX_WideStringC& str2);

    ~CFX_WideString();

    static CFX_WideString	FromLocal(const char* str, FX_STRSIZE len = -1);

    static CFX_WideString	FromUTF8(const char* str, FX_STRSIZE len = -1);

    static CFX_WideString	FromUTF16LE(const unsigned short* str, FX_STRSIZE len = -1);
    static CFX_WideString	FromUTF16BE(const unsigned short* str, FX_STRSIZE len = -1);

    static FX_STRSIZE       WStringLength(const unsigned short* str);

    operator FX_LPCWSTR() const
    {
        return m_pData ? m_pData->m_String : (FX_WCHAR*)L"";
    }

    void					Empty();


    FX_BOOL					IsEmpty() const
    {
        return !GetLength();
    }

    FX_STRSIZE				GetLength() const
    {
        return m_pData ? m_pData->m_nDataLength : 0;
    }

    const CFX_WideString&	operator = (FX_LPCWSTR str);

    const CFX_WideString&	operator =(const CFX_WideString& stringSrc);

    const CFX_WideString&	operator =(const CFX_WideStringC& stringSrc);

    const CFX_WideString&	operator += (FX_LPCWSTR str);

    const CFX_WideString&	operator += (FX_WCHAR ch);

    const CFX_WideString&	operator += (const CFX_WideString& str);

    const CFX_WideString&	operator += (const CFX_WideStringC& str);

    FX_WCHAR				GetAt(FX_STRSIZE nIndex) const
    {
        return m_pData ? m_pData->m_String[nIndex] : 0;
    }

    FX_WCHAR				operator[](FX_STRSIZE nIndex) const
    {
        return m_pData ? m_pData->m_String[nIndex] : 0;
    }

    void					SetAt(FX_STRSIZE nIndex, FX_WCHAR ch);

    int						Compare(FX_LPCWSTR str) const;

    int						Compare(const CFX_WideString& str) const;

    int						CompareNoCase(FX_LPCWSTR str) const;

    bool					Equal(const CFX_WideStringC& str) const;

    CFX_WideString			Mid(FX_STRSIZE first) const;

    CFX_WideString			Mid(FX_STRSIZE first, FX_STRSIZE count) const;

    CFX_WideString			Left(FX_STRSIZE count) const;

    CFX_WideString			Right(FX_STRSIZE count) const;

    FX_STRSIZE				Insert(FX_STRSIZE index, FX_WCHAR ch);

    FX_STRSIZE				Delete(FX_STRSIZE index, FX_STRSIZE count = 1);

    void					Format(FX_LPCWSTR lpszFormat, ... );

    void					FormatV(FX_LPCWSTR lpszFormat, va_list argList);

    void					MakeLower();

    void					MakeUpper();

    void					TrimRight();

    void					TrimRight(FX_WCHAR chTarget);

    void					TrimRight(FX_LPCWSTR lpszTargets);

    void					TrimLeft();

    void					TrimLeft(FX_WCHAR chTarget);

    void					TrimLeft(FX_LPCWSTR lpszTargets);

    void					Reserve(FX_STRSIZE len);

    FX_LPWSTR				GetBuffer(FX_STRSIZE len);

    FX_LPWSTR				LockBuffer();

    void					ReleaseBuffer(FX_STRSIZE len = -1);

    int						GetInteger() const;

    FX_FLOAT				GetFloat() const;

    FX_STRSIZE				Find(FX_LPCWSTR lpszSub, FX_STRSIZE start = 0) const;

    FX_STRSIZE				Find(FX_WCHAR ch, FX_STRSIZE start = 0) const;

    FX_STRSIZE				Replace(FX_LPCWSTR lpszOld, FX_LPCWSTR lpszNew);

    FX_STRSIZE				Remove(FX_WCHAR ch);

    CFX_ByteString			UTF8Encode() const;

    CFX_ByteString			UTF16LE_Encode(FX_BOOL bTerminate = TRUE) const;

    void					ConvertFrom(const CFX_ByteString& str, CFX_CharMap* pCharMap = NULL);
protected:
    void					InitStr(FX_LPCWSTR ptr, int len);

    CFX_StringDataW*		m_pData;
    void					CopyBeforeWrite();
    void					AllocBeforeWrite(FX_STRSIZE nLen);
    void					ConcatInPlace(FX_STRSIZE nSrcLen, FX_LPCWSTR lpszSrcData);
    void					ConcatCopy(FX_STRSIZE nSrc1Len, FX_LPCWSTR lpszSrc1Data, FX_STRSIZE nSrc2Len, FX_LPCWSTR lpszSrc2Data);
    void					AssignCopy(FX_STRSIZE nSrcLen, FX_LPCWSTR lpszSrcData);
    void					AllocCopy(CFX_WideString& dest, FX_STRSIZE nCopyLen, FX_STRSIZE nCopyIndex, FX_STRSIZE nExtraLen) const;
};
inline CFX_WideStringC::CFX_WideStringC(const CFX_WideString& src)
{
    m_Ptr = (FX_LPCWSTR)src;
    m_Length = src.GetLength();
}
inline CFX_WideStringC& CFX_WideStringC::operator = (const CFX_WideString& src)
{
    m_Ptr = (FX_LPCWSTR)src;
    m_Length = src.GetLength();
    return *this;
}

inline CFX_WideString operator + (const CFX_WideStringC& str1, const CFX_WideStringC& str2)
{
    return CFX_WideString(str1, str2);
}
inline CFX_WideString operator + (const CFX_WideStringC& str1, FX_LPCWSTR str2)
{
    return CFX_WideString(str1, str2);
}
inline CFX_WideString operator + (FX_LPCWSTR str1, const CFX_WideStringC& str2)
{
    return CFX_WideString(str1, str2);
}
inline CFX_WideString operator + (const CFX_WideStringC& str1, FX_WCHAR ch)
{
    return CFX_WideString(str1, CFX_WideStringC(ch));
}
inline CFX_WideString operator + (FX_WCHAR ch, const CFX_WideStringC& str2)
{
    return CFX_WideString(ch, str2);
}
inline CFX_WideString operator + (const CFX_WideString& str1, const CFX_WideString& str2)
{
    return CFX_WideString(str1, str2);
}
inline CFX_WideString operator + (const CFX_WideString& str1, FX_WCHAR ch)
{
    return CFX_WideString(str1, CFX_WideStringC(ch));
}
inline CFX_WideString operator + (FX_WCHAR ch, const CFX_WideString& str2)
{
    return CFX_WideString(ch, str2);
}
inline CFX_WideString operator + (const CFX_WideString& str1, FX_LPCWSTR str2)
{
    return CFX_WideString(str1, str2);
}
inline CFX_WideString operator + (FX_LPCWSTR str1, const CFX_WideString& str2)
{
    return CFX_WideString(str1, str2);
}
inline CFX_WideString operator + (const CFX_WideString& str1, const CFX_WideStringC& str2)
{
    return CFX_WideString(str1, str2);
}
inline CFX_WideString operator + (const CFX_WideStringC& str1, const CFX_WideString& str2)
{
    return CFX_WideString(str1, str2);
}

bool operator==(const CFX_WideString& s1, const CFX_WideString& s2);
bool operator==(const CFX_WideString& s1, const CFX_WideStringC& s2);
bool operator==(const CFX_WideStringC& s1, const CFX_WideString& s2);
bool operator== (const CFX_WideString& s1, FX_LPCWSTR s2);
bool operator==(FX_LPCWSTR s1, const CFX_WideString& s2);
bool operator!=(const CFX_WideString& s1, const CFX_WideString& s2);
bool operator!=(const CFX_WideString& s1, const CFX_WideStringC& s2);
bool operator!=(const CFX_WideStringC& s1, const CFX_WideString& s2);
bool operator!= (const CFX_WideString& s1, FX_LPCWSTR s2);
bool operator!=(FX_LPCWSTR s1, const CFX_WideString& s2);
FX_FLOAT FX_atof(FX_BSTR str);
void FX_atonum(FX_BSTR str, FX_BOOL& bInteger, void* pData, int sizeOfData = 4);
FX_STRSIZE FX_ftoa(FX_FLOAT f, FX_LPSTR buf);
CFX_ByteString	FX_UTF8Encode(FX_LPCWSTR pwsStr, FX_STRSIZE len);
inline CFX_ByteString	FX_UTF8Encode(FX_WSTR wsStr)
{
    return FX_UTF8Encode(wsStr.GetPtr(), wsStr.GetLength());
}
inline CFX_ByteString	FX_UTF8Encode(const CFX_WideString &wsStr)
{
    return FX_UTF8Encode((FX_LPCWSTR)wsStr, wsStr.GetLength());
}
inline FX_BOOL FX_IsUtf16SurrogatePair(FX_WCHAR first, FX_WCHAR second)
{
    return ((first & 0xFC00) == 0xD800 && (second & 0xFC00) == 0xDC00);
}
inline FX_DWORD FX_CreateCodePointFromUtf16SurrogatePair(FX_WCHAR first,
        FX_WCHAR second)
{
    const FX_DWORD mask = (1 << 10) - 1;
    return  (((first & mask) << 10) | (second & mask)) + 0x10000;
}
#endif
