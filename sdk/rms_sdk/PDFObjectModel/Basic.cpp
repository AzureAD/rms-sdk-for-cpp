#include "Basic.h"

namespace rmscore {
namespace pdfobjectmodel {

//////////////////////////////////////////////////////////////////////////
// class PDFBinaryBufImpl
PDFBinaryBufImpl::PDFBinaryBufImpl(std::shared_ptr<CFX_BinaryBuf> bin_buf) {
  shared_bin_buffer_ = bin_buf;
}

PDFBinaryBufImpl::~PDFBinaryBufImpl() {
  shared_bin_buffer_ = nullptr;
}

bool PDFBinaryBufImpl::AppendBlock(const void* buffer_pointer, uint32_t size) {
  if (shared_bin_buffer_) {
    return shared_bin_buffer_->AppendBlock(buffer_pointer, size);
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////
// class FileStreamImpl
FileStreamImpl::FileStreamImpl(rmscrypto::api::SharedStream ioStream)
    : shared_io_stream_(ioStream) {

}

FileStreamImpl::~FileStreamImpl() {

}

IFX_FileStream* FileStreamImpl::Retain() {
  return nullptr;
}

void FileStreamImpl::Release() {

}

FX_FILESIZE FileStreamImpl::GetSize() {
  uint64_t size = shared_io_stream_->Size();
  return size;
}

FX_BOOL FileStreamImpl::IsEOF() {
  uint64_t size = shared_io_stream_->Size();
  uint64_t pos = shared_io_stream_->Position();
  if (pos = size - 1) {
    return true;
  }
  return false;
}

FX_FILESIZE FileStreamImpl::GetPosition() {
  uint64_t pos = shared_io_stream_->Position();
  return pos;
}

FX_BOOL FileStreamImpl::ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) {
  shared_io_stream_->Seek(offset);
  shared_io_stream_->Read(reinterpret_cast<unsigned char*>(buffer), size);
  return true;
}

size_t FileStreamImpl::ReadBlock(void* buffer, size_t size) {
  int64_t read = shared_io_stream_->Read(reinterpret_cast<unsigned char*>(buffer), size);
  return read;
}

FX_BOOL FileStreamImpl::WriteBlock(const void* buffer, FX_FILESIZE offset, size_t size) {
  shared_io_stream_->Seek(offset);
  shared_io_stream_->Write(reinterpret_cast<const unsigned char*>((FX_LPVOID)buffer), size);
  return true;
}

FX_BOOL FileStreamImpl::Flush() {
  bool flush_result = shared_io_stream_->Flush();
  return flush_result;
}

void utility::UTF16ToUTF8(
    UTF16* utf_16_start,
    UTF16* utf_16_end,
    UTF8* utf_8_start,
    UTF8* utf_8_end) {
  UTF16* temp_utf_16 = utf_16_start;
  UTF8* temp_utf_8 = utf_8_start;

  while (temp_utf_16 < utf_16_end) {
    if (*temp_utf_16 <= UTF8_ONE_END && temp_utf_8 + 1 < utf_8_end) {
      //0000 - 007F  0xxxxxxx
      *temp_utf_8++ = (UTF8)*temp_utf_16;
    } else if (*temp_utf_16 >= UTF8_TWO_START &&
               *temp_utf_16 <= UTF8_TWO_END &&
               temp_utf_8 + 2 < utf_8_end) {
      //0080 - 07FF 110xxxxx 10xxxxxx
      *temp_utf_8++ = (*temp_utf_16 >> 6) | 0xC0;
      *temp_utf_8++ = (*temp_utf_16 & 0x3F) | 0x80;
    } else if (*temp_utf_16 >= UTF8_THREE_START &&
               *temp_utf_16 <= UTF8_THREE_END &&
               temp_utf_8 + 3 < utf_8_end) {
      //0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
      *temp_utf_8++ = (*temp_utf_16 >> 12) | 0xE0;
      *temp_utf_8++ = ((*temp_utf_16 >> 6) & 0x3F) | 0x80;
      *temp_utf_8++ = (*temp_utf_16 & 0x3F) | 0x80;
    } else {
      break;
    }
    temp_utf_16++;
  }
  *temp_utf_8 = 0;
}

void utility::UCS4ToUCS2(
    CFX_WideString ucs_4,
    FX_LPBYTE *output_ucs_2,
    FX_DWORD *output_ucs_2_length) {
  int ucs_4_size = ucs_4.GetLength() * sizeof(wchar_t);
  FX_LPBYTE ucs_4_temp = (FX_LPBYTE)(FX_LPCWSTR)ucs_4;

  *output_ucs_2_length = ucs_4_size / 2;
  *output_ucs_2 = new FX_BYTE[*output_ucs_2_length + 2];
  memset(*output_ucs_2, 0, sizeof(FX_BYTE) * (*output_ucs_2_length + 2));
  int j = 0, index = 0;
  for (int i = 0; i < ucs_4_size; i++) {
    memcpy(*output_ucs_2 + j, ucs_4_temp + i, sizeof(FX_BYTE));
    j++;
    index++;
    if(index == 2) {
      index = 0;
      i += 2;
    }
  }
}


} // namespace pdfobjectmodel
} // namespace rmscore
