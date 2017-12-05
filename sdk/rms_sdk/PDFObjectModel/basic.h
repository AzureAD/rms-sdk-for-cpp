#ifndef RMSSDK_PDFOBJECTMODEL_BASIC_H_
#define RMSSDK_PDFOBJECTMODEL_BASIC_H_

#include "pdf_object_model.h"
#include "core.h"

namespace rmscore {
namespace pdfobjectmodel {

/**
 * @brief The implementaion class of interface class PDFBinaryBuf.
 * Please refer to comments of PDFBinaryBuf.
 * The interface header file of PDF object model is pdf_object_model.h.
 */
class PDFBinaryBufImpl : public PDFBinaryBuf {
 public:
  explicit PDFBinaryBufImpl(CFX_BinaryBuf* bin_buf);
  virtual ~PDFBinaryBufImpl();

  virtual bool AppendBlock(const void* buffer_pointer, uint32_t size);

 private:
  CFX_BinaryBuf* bin_buffer_;
};

/**
 * @brief The implementaion class of interface class IFX_FileStream.
 * It is invoked by pdf object model when reading and writing pdf document.
 * IFX_FileStream is the file stream interface (reading & writing) of Foxit core.
 * Please refer to comments of IFX_FileStream of Foxit core.
 */
class FileStreamImpl : public IFX_FileStream {
 public:
  explicit FileStreamImpl(PDFSharedStream ioStream);
  virtual ~FileStreamImpl();

  virtual IFX_FileStream* Retain();

  virtual void Release();

  virtual FX_FILESIZE GetSize();

  virtual FX_BOOL IsEOF();

  virtual FX_FILESIZE GetPosition();

  virtual FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size);

  virtual size_t ReadBlock(void* buffer, size_t size);

  virtual FX_BOOL WriteBlock(const void* buffer, FX_FILESIZE offset, size_t size);

  virtual FX_BOOL Flush();

 private:
  PDFSharedStream shared_io_stream_;
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

/**
 * @brief It is the utility class for pdf object model, like encoding converting.
 */

namespace utility {
void UTF16ToUTF8(UTF16* utf_16_start, UTF16* utf_16_end, UTF8* utf_8_start, UTF8* utf_8_end);
void UCS4ToUCS2(CFX_WideString ucs_4, FX_LPBYTE *output_ucs_2, FX_DWORD *output_ucs_2_length);
}  // namespace utility

} // namespace pdfobjectmodel
} // namespace rmscore

#endif // RMSSDK_PDFOBJECTMODEL_BASIC_H_
