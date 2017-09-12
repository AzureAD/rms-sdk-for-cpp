#include <assert.h>
#include "std_stream_adapter.h"

using std::enable_shared_from_this;

namespace mip {
namespace file {

StdStreamAdapter::StdStreamAdapter(std::shared_ptr<std::iostream> stdStream)
  : mInputStream(static_pointer_cast<std::istream>(stdStream)),
    mOutputStream(static_pointer_cast<std::ostream>(stdStream))
{}

StdStreamAdapter::StdStreamAdapter(std::shared_ptr<std::ostream> stdOutputStream)
  : mOutputStream(stdOutputStream)
{}

StdStreamAdapter::StdStreamAdapter(std::shared_ptr<std::istream> stdInputStream)
  : mInputStream(stdInputStream)
{}

StdStreamAdapter::StdStreamAdapter(std::shared_ptr<StdStreamAdapter> from)
  : mInputStream(from->mInputStream),
    mOutputStream(from->mOutputStream)
{}

int64_t StdStreamAdapter::Read(uint8_t *buffer, int64_t bufferLength) {
  if (mInputStream.get() == nullptr) {
    throw StdStreamIOException("Operation unavailable");
  }

  auto fail = mInputStream->fail();
  // Windows implementation of StringStream has a bug
  // that for an empty stream fail bit is set after calling seekg(0).
  // Workaround the bug by calling clear() which clears the failures.
  if(fail){
    mInputStream->clear();
  }
  mInputStream->read(reinterpret_cast<char *>(buffer), bufferLength);
  return mInputStream->gcount();
}

int64_t StdStreamAdapter::Write(const uint8_t *buffer, int64_t bufferLength) {
  if (mOutputStream.get() == nullptr) {
    throw StdStreamIOException("Operation unavailable");
  }

  assert(buffer != nullptr || bufferLength == 0);

  auto fail = mOutputStream->fail();
  // Windows implementation of StringStream has a bug
  // that for an empty stream fail bit is set after calling seekg(0).
  // Workaround the bug by calling clear() which clears the failures.
  if(fail){
    mOutputStream->clear();
  }
  mOutputStream->write(reinterpret_cast<const char *>(buffer), bufferLength);
  return bufferLength;
}

bool StdStreamAdapter::Flush()                        {
  if (mOutputStream.get() != nullptr) {
    mOutputStream->flush();
  }
  return true;
}

void StdStreamAdapter::Seek(uint64_t position) {

  if (mInputStream.get() != nullptr) {
    mInputStream->clear();
    mInputStream->seekg(position);
  }

  if (mOutputStream.get() != nullptr) {
    mOutputStream->seekp(position);
  }
}

bool StdStreamAdapter::CanRead() const {
  return mInputStream.get() != nullptr;
}

bool StdStreamAdapter::CanWrite() const {
  return mOutputStream.get() != nullptr;
}

uint64_t StdStreamAdapter::Position() {
  if (mInputStream.get() != nullptr)
    return static_cast<uint64_t>(mInputStream->tellg());

  if (mOutputStream.get() != nullptr)
    return static_cast<uint64_t>(mOutputStream->tellp());

  return 0;
}

uint64_t GetSize(std::shared_ptr<std::iostream> stream) {
  uint64_t position = 0;
  stream->clear();
  auto savedPosition =  stream->tellp();
  stream->seekp(0, std::ios_base::end);
  position =  static_cast<uint64_t>(stream->tellp());
  stream->seekp(savedPosition);
  stream->clear();
  return position;
}

uint64_t StdStreamAdapter::Size() {
  int position = 0;

  if (mInputStream.get() != nullptr) {
    mInputStream->clear();
    auto currentPosition =  mInputStream->tellg();
    mInputStream->seekg(0, std::ios_base::end);
    position =  static_cast<int>(mInputStream->tellg());
    mInputStream->seekg(currentPosition);
    mInputStream->clear();
  } else if (mOutputStream.get() != nullptr) {
    mOutputStream->clear();
    auto currentPosition =  mOutputStream->tellp();
    mOutputStream->seekp(0, std::ios_base::end);
    position =  static_cast<int>(mOutputStream->tellp());
    mOutputStream->seekp(currentPosition);
    mOutputStream->clear();
  }
  return static_cast<uint64_t>(position);
}

void StdStreamAdapter::Size(uint64_t) {}

SharedStream StdStreamAdapter::Clone() {
  return static_pointer_cast<IStream>(std::shared_ptr<StdStreamAdapter>(new StdStreamAdapter(shared_from_this())));
}

} // namespace file
} // namespace mip
