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

#define UTF8_ONE_START      (0xOOO1)
#define UTF8_ONE_END        (0x007F)
#define UTF8_TWO_START      (0x0080)
#define UTF8_TWO_END        (0x07FF)
#define UTF8_THREE_START    (0x0800)
#define UTF8_THREE_END      (0xFFFF)

typedef unsigned long   UTF32;  /* at least 32 bits */
typedef unsigned short  UTF16;  /* at least 16 bits */
typedef unsigned char   UTF8;   /* typically 8 bits */
typedef unsigned int    INT;

class Utility
{
public:
    static void UTF16ToUTF8(UTF16* pUTF16Start, UTF16* pUTF16End, UTF8* pUTF8Start, UTF8* pUTF8End);
    static void UCS4ToUCS2(CFX_WideString wsUCS4, FX_LPBYTE *ppUCS2, FX_DWORD *dwUCS2Length);
};

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMS_SDK_PDF_BASIC_H
