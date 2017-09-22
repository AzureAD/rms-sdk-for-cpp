#include "xmpio_over_istream.h"
#include <Common/std_stream_adapter.h>
#include <iostream>
#include <fstream>

namespace mip {
namespace file {

XMPIOOverIStream::XMPIOOverIStream(std::shared_ptr<IStream> stream) {
  mBaseStream = stream;
}

XMPIOOverIStream::~XMPIOOverIStream() {
  delete mTemp;
}

XMP_Uns32 XMPIOOverIStream::Read(void* buffer, XMP_Uns32 count, bool readAll) {
  if (buffer == nullptr)
    throw new XMP_Error(kXMPErr_BadParam, "Buffer is null");

  auto readCount = mBaseStream->Read(static_cast<uint8_t*>(buffer), count);
  if (readAll && (XMP_Uns32)readCount < count)
    throw new XMP_Error(kXMPErr_EnforceFailure, "readCount is lower than expected");

  return (XMP_Uns32)readCount;
}

void XMPIOOverIStream::Write(const void* buffer, XMP_Uns32 count) {
  if (buffer == nullptr)
    throw new XMP_Error(kXMPErr_BadParam, "Buffer is null");

  mBaseStream->Write(static_cast<const uint8_t*>(buffer), (int64_t)count);
}

XMP_Int64 XMPIOOverIStream::Seek(XMP_Int64 offset, SeekMode mode) {

  switch(mode) {
  case SeekMode::kXMP_SeekFromStart: {
    mBaseStream->Seek((uint64_t)offset);
    break;
  }
  case SeekMode::kXMP_SeekFromCurrent: {
    mBaseStream->Seek(offset + mBaseStream->Position());
    break;
  }
  case SeekMode::kXMP_SeekFromEnd: {
    mBaseStream->Seek(mBaseStream->Size() + offset);
    break;
  }
  }

  return  mBaseStream->Position();
}

XMP_Int64 XMPIOOverIStream::Length() {
  return mBaseStream->Size();
}

void XMPIOOverIStream::Truncate(XMP_Int64 length) {
  mBaseStream->Size((uint64_t)length);
}

XMP_IO* XMPIOOverIStream::DeriveTemp() {
  if(mTemp)
    return mTemp;
  auto backingStdStream = make_shared<std::stringstream>(std::ios::in | std::ios::out | std::ios::binary);
  auto stream = StdStreamAdapter::Create(static_pointer_cast<std::iostream>(backingStdStream));
  mTemp = new XMPIOOverIStream(stream);
  return mTemp;
}

void XMPIOOverIStream::AbsorbTemp() {
  if (mTemp == nullptr)
    return;
  mTemp->mBaseStream->Seek(0);
  mBaseStream->Seek(0);
  int readCount = 0;
  do{
    uint8_t buffer[1024];
    readCount = mTemp->mBaseStream->Read(&buffer[0], 1024);
    mBaseStream->Write(buffer, readCount);
  }
  while (readCount > 0);
}

void XMPIOOverIStream::DeleteTemp() {
  delete mTemp;
  mTemp = nullptr;
}

} // namespace file
} // namespace mip
