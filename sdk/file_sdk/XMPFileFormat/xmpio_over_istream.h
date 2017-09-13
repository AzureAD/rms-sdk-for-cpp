#ifndef XMPIO_OVER_ISTREAM_H
#define XMPIO_OVER_ISTREAM_H

#include <Common/istream.h>
#include <XMP_IO.hpp>
#include <XMP_Const.h>
#include <sstream>

using mip::file::IStream;
using std::static_pointer_cast;
using std::make_shared;

namespace mip {
namespace file {

class XMPIOOverIStream : public XMP_IO
{
public:
  XMPIOOverIStream(std::shared_ptr<IStream> stream);

  ~XMPIOOverIStream() override;

  XMP_Uns32 Read(void* buffer, XMP_Uns32 count, bool readAll = false) override;

  void Write(const void* buffer, XMP_Uns32 count) override;

  XMP_Int64 Seek(XMP_Int64 offset, SeekMode mode) override;

  XMP_Int64 Length() override;

  void Truncate(XMP_Int64 length) override;

  XMP_IO* DeriveTemp() override;

  void AbsorbTemp() override;

  void DeleteTemp() override;

private:
  XMPIOOverIStream* mTemp = nullptr;
  std::shared_ptr<IStream> mBaseStream;
};

} // namespace file
} // namespace mip

#endif // XMPIO_OVER_ISTREAM_H
