#ifndef RMS_SDK_PDF_BASIC_H
#define RMS_SDK_PDF_BASIC_H

#include "PDFObjectModel.h"
#include "CoreInc.h"

namespace rmscore {
namespace pdfobjectmodel {

class PDFBinaryBufImpl : public PDFBinaryBuf
{
public:
    PDFBinaryBufImpl(CFX_BinaryBuf* pBinBuf);
	virtual ~PDFBinaryBufImpl();

	virtual bool AppendBlock(const void* pBuf, uint32_t size);

private:
    CFX_BinaryBuf* m_pBinBuf;
};

class FileStreamImpl : public IFX_FileStream
{
public:
    FileStreamImpl(rmscrypto::api::SharedStream ioStream);
	virtual ~FileStreamImpl();

	virtual IFX_FileStream* Retain();

	virtual void Release();

	virtual FX_FILESIZE GetSize();

	virtual FX_BOOL IsEOF();

	virtual FX_FILESIZE GetPosition();

	virtual FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size);

	virtual size_t ReadBlock(void* buffer, size_t size);

	virtual	FX_BOOL WriteBlock(const void* buffer, FX_FILESIZE offset, size_t size);

	virtual FX_BOOL Flush();

private:
    rmscrypto::api::SharedStream m_sharedIOStream;
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMS_SDK_PDF_BASIC_H
