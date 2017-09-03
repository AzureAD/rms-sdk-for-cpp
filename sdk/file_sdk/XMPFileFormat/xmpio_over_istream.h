#ifndef XMPIO_OVER_ISTREAM_H
#define XMPIO_OVER_ISTREAM_H

#include <IStream.h>
#include <XMP_IO.hpp>
#include <XMP_Const.h>
#include <CryptoAPI.h>
#include <sstream>

using rmscrypto::api::IStream;
using std::static_pointer_cast;
using std::make_shared;

namespace mip {
namespace file {

class XMPIOOverIStream : public XMP_IO
{
public:
  XMPIOOverIStream(std::shared_ptr<IStream> stream) {
    mBaseStream = stream;
  }

  ~XMPIOOverIStream() override {
    if (mTemp != nullptr)
      delete mTemp;
  }

  XMP_Uns32 Read(void* buffer, XMP_Uns32 count, bool readAll = false) override {
    if (buffer == nullptr)
      throw new XMP_Error(kXMPErr_BadParam, "Buffer is null");

    auto readCount = mBaseStream->Read(static_cast<uint8_t*>(buffer), count);
    if(readAll && (XMP_Uns32)readCount < count)
      throw new XMP_Error(kXMPErr_BadParam, "readCount is lower than expected");

    return (XMP_Uns32)readCount;
  }

  void Write(const void* buffer, XMP_Uns32 count) override {

    if (buffer == nullptr)
      throw new XMP_Error(kXMPErr_BadParam, "Buffer is null");

    mBaseStream->Write(static_cast<const uint8_t*>(buffer), (int64_t)count);
  }

  XMP_Int64 Seek(XMP_Int64 offset, SeekMode mode) override {

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

  XMP_Int64 Length() override {
    return mBaseStream->Size();
  }

  void Truncate(XMP_Int64 length) override  {
    mBaseStream->Size((uint64_t)length);
  }

  XMP_IO* DeriveTemp() override  {
    auto backingStdStream = make_shared<std::stringstream>(std::ios::in | std::ios::out | std::ios::binary);
    auto stream = rmscrypto::api::CreateStreamFromStdStream(static_pointer_cast<std::iostream>(
                                                              backingStdStream));
    auto streamAdapter = static_pointer_cast<IStream>(stream);
    mTemp = new XMPIOOverIStream(streamAdapter);
    return mTemp;
  }

  void AbsorbTemp() override {
    mBaseStream->Seek(0);
    mTemp->mBaseStream->Seek(0);
    mTemp->mBaseStream = mTemp->mBaseStream->Clone();
    mTemp->mBaseStream->Size(mTemp->mBaseStream->Size());
 }

  void DeleteTemp() override {
    if(mTemp)
      delete mTemp;
    mTemp = nullptr;
  }

private:
  XMPIOOverIStream* mTemp = nullptr;
  std::shared_ptr<IStream> mBaseStream;
};

} // namespace file
} // namespace mip

#endif // XMPIO_OVER_ISTREAM_H
