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
    BOOL bFlush = m_sharedIOStream->Flush();
    return bFlush;
}

} // namespace pdfobjectmodel
} // namespace rmscore
