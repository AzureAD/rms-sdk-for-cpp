#include "Basic.h"

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
// class PDFBinaryBufImpl
PDFBinaryBufImpl::PDFBinaryBufImpl(CFX_BinaryBuf *pBinBuf)
{
    m_pBinBuf = pBinBuf;
}

PDFBinaryBufImpl::~PDFBinaryBufImpl()
{
    m_pBinBuf = nullptr;
}

bool PDFBinaryBufImpl::AppendBlock(const void* pBuf, uint32_t size)
{
    if(m_pBinBuf)
    {
        return m_pBinBuf->AppendBlock(pBuf, size);
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
// class FileStreamImpl
FileStreamImpl::FileStreamImpl(rmscrypto::api::SharedStream ioStream)
    : m_sharedIOStream(ioStream)
{

}

FileStreamImpl::~FileStreamImpl()
{

}

IFX_FileStream* FileStreamImpl::Retain()
{
	return nullptr;
}

void FileStreamImpl::Release()
{

}

FX_FILESIZE FileStreamImpl::GetSize()
{
    uint64_t size = m_sharedIOStream->Size();
    return size;
}

FX_BOOL FileStreamImpl::IsEOF()
{
    uint64_t size = m_sharedIOStream->Size();
    uint64_t pos = m_sharedIOStream->Position();
    if(pos = size - 1)
    {
        return true;
    }
    return false;
}

FX_FILESIZE FileStreamImpl::GetPosition()
{
    uint64_t pos = m_sharedIOStream->Position();
    return pos;
}

FX_BOOL FileStreamImpl::ReadBlock(void* buffer, FX_FILESIZE offset, size_t size)
{
    m_sharedIOStream->Seek(offset);
    m_sharedIOStream->Read(reinterpret_cast<unsigned char*>(buffer), size);
	return true;
}

size_t FileStreamImpl::ReadBlock(void* buffer, size_t size)
{
    int64_t read = m_sharedIOStream->Read(reinterpret_cast<unsigned char*>(buffer), size);
    return read;
}

FX_BOOL FileStreamImpl::WriteBlock(const void* buffer, FX_FILESIZE offset, size_t size)
{
    m_sharedIOStream->Seek(offset);
    m_sharedIOStream->Write(reinterpret_cast<const unsigned char*>((FX_LPVOID)buffer), size);
	return true;
}

FX_BOOL FileStreamImpl::Flush()
{
    bool bFlush = m_sharedIOStream->Flush();
    return bFlush;
}

void Utility::UTF16ToUTF8(UTF16* pUTF16Start, UTF16* pUTF16End, UTF8* pUTF8Start, UTF8* pUTF8End)
{
    UTF16* pTempUTF16 = pUTF16Start;
    UTF8* pTempUTF8 = pUTF8Start;

    while (pTempUTF16 < pUTF16End)
    {
        if (*pTempUTF16 <= UTF8_ONE_END
            && pTempUTF8 + 1 < pUTF8End)
        {
            //0000 - 007F  0xxxxxxx
            *pTempUTF8++ = (UTF8)*pTempUTF16;
        }
        else if(*pTempUTF16 >= UTF8_TWO_START && *pTempUTF16 <= UTF8_TWO_END
            && pTempUTF8 + 2 < pUTF8End)
        {
            //0080 - 07FF 110xxxxx 10xxxxxx
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else if(*pTempUTF16 >= UTF8_THREE_START && *pTempUTF16 <= UTF8_THREE_END
            && pTempUTF8 + 3 < pUTF8End)
        {
            //0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else
        {
            break;
        }
        pTempUTF16++;
    }
    *pTempUTF8 = 0;
}

} // namespace pdfobjectmodel
} // namespace rmscore
