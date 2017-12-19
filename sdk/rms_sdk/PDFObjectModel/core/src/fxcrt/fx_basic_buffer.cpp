#include "../../include/fxcrt/fx_basic.h"
FX_STRSIZE FX_ftoa(FX_FLOAT f, FX_LPSTR buf);
CFX_BinaryBuf::CFX_BinaryBuf()
    : m_AllocStep(0)
    , m_pBuffer(NULL)
    , m_DataSize(0)
    , m_AllocSize(0)
{
}
CFX_BinaryBuf::CFX_BinaryBuf(FX_STRSIZE size)
    : m_AllocStep(0)
    , m_DataSize(size)
    , m_AllocSize(size)
{
    m_pBuffer = FX_Alloc(FX_BYTE, size);
}
CFX_BinaryBuf::~CFX_BinaryBuf()
{
    if (m_pBuffer) {
        FX_Free(m_pBuffer);
    }
}
void CFX_BinaryBuf::Delete(int start_index, int count)
{
    if (!m_pBuffer || start_index < 0 || start_index + count > m_DataSize) {
        return;
    }
    FXSYS_memmove32(m_pBuffer + start_index, m_pBuffer + start_index + count, m_DataSize - start_index - count);
    m_DataSize -= count;
}
void CFX_BinaryBuf::Clear()
{
    m_DataSize = 0;
}
void CFX_BinaryBuf::DetachBuffer()
{
    m_DataSize = 0;
    m_pBuffer = NULL;
    m_AllocSize = 0;
}
void CFX_BinaryBuf::AttachData(void* buffer, FX_STRSIZE size)
{
    if (m_pBuffer) {
        FX_Free(m_pBuffer);
    }
    m_DataSize = size;
    m_pBuffer = (FX_LPBYTE)buffer;
    m_AllocSize = size;
}
void CFX_BinaryBuf::TakeOver(CFX_BinaryBuf& other)
{
    AttachData(other.GetBuffer(), other.GetSize());
    other.DetachBuffer();
}
FX_BOOL CFX_BinaryBuf::EstimateSize(FX_STRSIZE size, FX_STRSIZE step)
{
    m_AllocStep = step;
    if (m_AllocSize >= size) {
        return TRUE;
    }
    return ExpandBuf(size - m_DataSize);
}
FX_BOOL CFX_BinaryBuf::ExpandBuf(FX_STRSIZE add_size)
{
    FX_STRSIZE new_size = add_size + m_DataSize;
    if (m_AllocSize >= new_size) {
        return TRUE;
    }
    int alloc_step;
    if (m_AllocStep == 0) {
        alloc_step = m_AllocSize / 4;
        if (alloc_step < 128 ) {
            alloc_step = 128;
        }
    } else {
        alloc_step = m_AllocStep;
    }
    new_size = (new_size + alloc_step - 1) / alloc_step * alloc_step;
    FX_LPBYTE pNewBuffer = m_pBuffer;
    if (pNewBuffer) {
        pNewBuffer = FX_Realloc(FX_BYTE, m_pBuffer, new_size);
    } else {
        pNewBuffer = FX_Alloc(FX_BYTE, new_size);
    }
    if (!pNewBuffer) {
        return FALSE;
    }
    m_pBuffer = pNewBuffer;
    m_AllocSize = new_size;
    return TRUE;
}
FX_BOOL CFX_BinaryBuf::CopyData(const void* pStr, FX_STRSIZE size)
{
    if (size == 0) {
        m_DataSize = 0;
        return TRUE;
    }
    if (m_AllocSize < size)
        if (!ExpandBuf(size - m_DataSize)) {
            return FALSE;
        }
    if (!m_pBuffer) {
        return FALSE;
    }
    FXSYS_memcpy32(m_pBuffer, pStr, size);
    m_DataSize = size;
    return TRUE;
}
FX_BOOL CFX_BinaryBuf::AppendBlock(const void* pBuf, FX_STRSIZE size)
{
    if (!ExpandBuf(size)) {
        return FALSE;
    }
    if (!m_pBuffer) {
        return FALSE;
    }
    if (pBuf) {
        FXSYS_memcpy32(m_pBuffer + m_DataSize, pBuf, size);
    }
    m_DataSize += size;
    return TRUE;
}
FX_BOOL CFX_BinaryBuf::InsertBlock(FX_STRSIZE pos, const void* pBuf, FX_STRSIZE size)
{
    if (!ExpandBuf(size)) {
        return FALSE;
    }
    if (!m_pBuffer) {
        return FALSE;
    }
    FXSYS_memmove32(m_pBuffer + pos + size, m_pBuffer + pos, m_DataSize - pos);
    if (pBuf) {
        FXSYS_memcpy32(m_pBuffer + pos, pBuf, size);
    }
    m_DataSize += size;
    return TRUE;
}
FX_BOOL CFX_BinaryBuf::AppendFill(FX_BYTE byte, FX_STRSIZE count)
{
    if (!ExpandBuf(count)) {
        return FALSE;
    }
    if (!m_pBuffer) {
        return FALSE;
    }
    FXSYS_memset8(m_pBuffer + m_DataSize, byte, count);
    m_DataSize += count;
    return TRUE;
}
CFX_ByteStringC CFX_BinaryBuf::GetByteString() const
{
    return CFX_ByteStringC(m_pBuffer, m_DataSize);
}
CFX_ByteTextBuf& CFX_ByteTextBuf::operator << (FX_BSTR lpsz)
{
    AppendBlock((FX_LPCBYTE)lpsz, lpsz.GetLength());
    return *this;
}
CFX_ByteTextBuf& CFX_ByteTextBuf::operator << (int i)
{
    char buf[32];
    FXSYS_itoa(i, buf, 10);
    AppendBlock(buf, (FX_STRSIZE)FXSYS_strlen(buf));
    return *this;
}
CFX_ByteTextBuf& CFX_ByteTextBuf::operator << (FX_DWORD i)
{
    char buf[32];
    FXSYS_itoa(i, buf, 10);
    AppendBlock(buf, (FX_STRSIZE)FXSYS_strlen(buf));
    return *this;
}
CFX_ByteTextBuf& CFX_ByteTextBuf::operator << (double f)
{
    char buf[32];
    FX_STRSIZE len = FX_ftoa((FX_FLOAT)f, buf);
    AppendBlock(buf, len);
    return *this;
}
CFX_ByteTextBuf& CFX_ByteTextBuf::operator << (const CFX_ByteTextBuf& buf)
{
    AppendBlock(buf.m_pBuffer, buf.m_DataSize);
    return *this;
}
void CFX_ByteTextBuf::operator =(const CFX_ByteStringC& str)
{
    CopyData((FX_LPCBYTE)str, str.GetLength());
}
FX_BOOL CFX_WideTextBuf::AppendChar(FX_WCHAR ch)
{
    if (m_AllocSize < m_DataSize + (FX_STRSIZE)sizeof(FX_WCHAR))
        if(!ExpandBuf(sizeof(FX_WCHAR))) {
            return FALSE;
        }
    if (!m_pBuffer) {
        return FALSE;
    }
    *(FX_WCHAR*)(m_pBuffer + m_DataSize) = ch;
    m_DataSize += sizeof(FX_WCHAR);
    return TRUE;
}
CFX_WideTextBuf& CFX_WideTextBuf::operator << (FX_WSTR str)
{
    AppendBlock(str.GetPtr(), str.GetLength() * sizeof(FX_WCHAR));
    return *this;
}
CFX_WideTextBuf& CFX_WideTextBuf::operator << (const CFX_WideString &str)
{
    AppendBlock((FX_LPCWSTR)str, str.GetLength() * sizeof(FX_WCHAR));
    return *this;
}
CFX_WideTextBuf& CFX_WideTextBuf::operator << (int i)
{
    char buf[32];
    FXSYS_itoa(i, buf, 10);
    FX_STRSIZE len = (FX_STRSIZE)FXSYS_strlen(buf);
    if (m_AllocSize < m_DataSize + (FX_STRSIZE)(len * sizeof(FX_WCHAR))) {
        ExpandBuf(len * sizeof(FX_WCHAR));
    }
    ASSERT(m_pBuffer != NULL);
    FX_LPWSTR str = (FX_WCHAR*)(m_pBuffer + m_DataSize);
    for (FX_STRSIZE j = 0; j < len; j ++) {
        *str ++ = buf[j];
    }
    m_DataSize += len * sizeof(FX_WCHAR);
    return *this;
}
CFX_WideTextBuf& CFX_WideTextBuf::operator << (double f)
{
    char buf[32];
    FX_STRSIZE len = FX_ftoa((FX_FLOAT)f, buf);
    if (m_AllocSize < m_DataSize + (FX_STRSIZE)(len * sizeof(FX_WCHAR))) {
        ExpandBuf(len * sizeof(FX_WCHAR));
    }
    ASSERT(m_pBuffer != NULL);
    FX_LPWSTR str = (FX_WCHAR*)(m_pBuffer + m_DataSize);
    for (FX_STRSIZE i = 0; i < len; i ++) {
        *str ++ = buf[i];
    }
    m_DataSize += len * sizeof(FX_WCHAR);
    return *this;
}
CFX_WideTextBuf& CFX_WideTextBuf::operator << (FX_LPCWSTR lpsz)
{
    AppendBlock(lpsz, (FX_STRSIZE)FXSYS_wcslen(lpsz)*sizeof(FX_WCHAR));
    return *this;
}
CFX_WideTextBuf& CFX_WideTextBuf::operator << (const CFX_WideTextBuf& buf)
{
    AppendBlock(buf.m_pBuffer, buf.m_DataSize);
    return *this;
}
void CFX_WideTextBuf::operator =(FX_WSTR str)
{
    CopyData(str.GetPtr(), str.GetLength() * sizeof(FX_WCHAR));
}
CFX_WideStringC CFX_WideTextBuf::GetWideString() const
{
    return CFX_WideStringC((FX_LPCWSTR)m_pBuffer, m_DataSize / sizeof(FX_WCHAR));
}
void CFX_BitStream::Init(FX_LPCBYTE pData, FX_DWORD dwSize)
{
    m_pData = pData;
    m_BitSize = dwSize * 8;
    m_BitPos = 0;
}
void CFX_BitStream::ByteAlign()
{
    int mod = m_BitPos % 8;
    if (mod == 0) {
        return;
    }
    m_BitPos += 8 - mod;
}
FX_DWORD CFX_BitStream::GetBits(FX_DWORD nBits)
{
    if (nBits > m_BitSize || m_BitPos + nBits > m_BitSize) {
        return 0;
    }
    if (nBits == 1) {
        int bit = (m_pData[m_BitPos / 8] & (1 << (7 - m_BitPos % 8))) ? 1 : 0;
        m_BitPos ++;
        return bit;
    }
    FX_DWORD byte_pos = m_BitPos / 8;
    FX_DWORD bit_pos = m_BitPos % 8, bit_left = nBits;
    FX_DWORD result = 0;
    if (bit_pos) {
        if (8 - bit_pos >= bit_left) {
            result = (m_pData[byte_pos] & (0xff >> bit_pos)) >> (8 - bit_pos - bit_left);
            m_BitPos += bit_left;
            return result;
        }
        bit_left -= 8 - bit_pos;
        result = (m_pData[byte_pos++] & ((1 << (8 - bit_pos)) - 1)) << bit_left;
    }
    while (bit_left >= 8) {
        bit_left -= 8;
        result |= m_pData[byte_pos++] << bit_left;
    }
    if (bit_left) {
        result |= m_pData[byte_pos] >> (8 - bit_left);
    }
    m_BitPos += nBits;
    return result;
}
IFX_BufferArchive::IFX_BufferArchive(FX_STRSIZE size)
    : m_BufSize(size)
    , m_pBuffer(NULL)
    , m_Length(0)
{
}
void IFX_BufferArchive::Clear()
{
    m_Length = 0;
    if (m_pBuffer) {
        FX_Free(m_pBuffer);
        m_pBuffer = NULL;
    }
}
FX_BOOL IFX_BufferArchive::Flush()
{
    FX_BOOL bRet = DoWork(m_pBuffer, m_Length);
    m_Length = 0;
    return bRet;
}
FX_INT32 IFX_BufferArchive::AppendBlock(const void* pBuf, size_t size)
{
    if (!pBuf || size < 1) {
        return 0;
    }
    if (!m_pBuffer) {
        m_pBuffer = FX_Alloc(FX_BYTE, m_BufSize);
        if (!m_pBuffer) {
            return -1;
        }
    }
    FX_LPBYTE buffer = (FX_LPBYTE)pBuf;
    FX_STRSIZE temp_size = (FX_STRSIZE)size;
    while (temp_size > 0) {
        FX_STRSIZE buf_size = FX_MIN(m_BufSize - m_Length, (FX_STRSIZE)temp_size);
        FXSYS_memcpy32(m_pBuffer + m_Length, buffer, buf_size);
        m_Length += buf_size;
        if (m_Length == m_BufSize) {
            if (!Flush()) {
                return -1;
            }
        }
        temp_size -= buf_size;
        buffer += buf_size;
    }
    return (FX_INT32)size;
}
FX_INT32 IFX_BufferArchive::AppendByte(FX_BYTE byte)
{
    return AppendBlock(&byte, 1);
}
FX_INT32 IFX_BufferArchive::AppendDWord(FX_DWORD i)
{
    char buf[32];
    FXSYS_itoa(i, buf, 10);
    return AppendBlock(buf, (size_t)FXSYS_strlen(buf));
}
FX_INT32 IFX_BufferArchive::AppendInt64(FX_INT64 i)
{
    FX_CHAR buf[32] = {0};
    FXSYS_i64toa(i, buf, 10);
    return AppendBlock(buf, (size_t)FXSYS_strlen(buf));
}
FX_INT32 IFX_BufferArchive::AppendString(FX_BSTR lpsz)
{
    return AppendBlock((FX_LPCBYTE)lpsz, lpsz.GetLength());
}
CFX_FileBufferArchive::CFX_FileBufferArchive(FX_STRSIZE size)
    : IFX_BufferArchive(size)
    , m_pFile(NULL)
    , m_bTakeover(FALSE)
{
}
CFX_FileBufferArchive::~CFX_FileBufferArchive()
{
    Clear();
}
void CFX_FileBufferArchive::Clear()
{
    if (m_pFile && m_bTakeover) {
        m_pFile->Release();
    }
    m_pFile = NULL;
    m_bTakeover = FALSE;
    IFX_BufferArchive::Clear();
}
FX_BOOL CFX_FileBufferArchive::AttachFile(IFX_StreamWrite *pFile, FX_BOOL bTakeover )
{
    if (!pFile) {
        return FALSE;
    }
    if (m_pFile && m_bTakeover) {
        m_pFile->Release();
    }
    m_pFile = pFile;
    m_bTakeover = bTakeover;
    return TRUE;
}
FX_BOOL CFX_FileBufferArchive::AttachFile(FX_LPCWSTR filename)
{
    if (!filename) {
        return FALSE;
    }
    if (m_pFile && m_bTakeover) {
        m_pFile->Release();
    }
    m_pFile = FX_CreateFileWrite(filename);
    if (!m_pFile) {
        return FALSE;
    }
    m_bTakeover = TRUE;
    return TRUE;
}
FX_BOOL CFX_FileBufferArchive::AttachFile(FX_LPCSTR filename)
{
    if (!filename) {
        return FALSE;
    }
    if (m_pFile && m_bTakeover) {
        m_pFile->Release();
    }
    m_pFile = FX_CreateFileWrite(filename);
    if (!m_pFile) {
        return FALSE;
    }
    m_bTakeover = TRUE;
    return TRUE;
}
FX_BOOL CFX_FileBufferArchive::DoWork(const void* pBuf, size_t size)
{
    if (!m_pFile) {
        return FALSE;
    }
    if (!pBuf || size < 1) {
        return TRUE;
    }
    return m_pFile->WriteBlock(pBuf, size);
}
