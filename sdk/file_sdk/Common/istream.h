#ifndef FILE_SDK_ISTREAM_H
#define FILE_SDK_ISTREAM_H

#include <vector>
#include <memory>

namespace mip {
namespace file {

class IStream {
public:
  virtual int64_t Read(uint8_t *buffer, int64_t bufferLength) = 0;
  virtual int64_t Write(const uint8_t *buffer, int64_t bufferLength) = 0;
  virtual bool Flush() = 0;
  virtual void Seek(uint64_t position) = 0;
  virtual bool CanRead() const = 0;
  virtual bool CanWrite() const = 0;
  virtual uint64_t Position() = 0;
  virtual uint64_t Size() = 0;
  virtual void Size(uint64_t value) = 0;
  virtual std::shared_ptr<IStream> Clone() = 0;

protected:
  virtual ~IStream() {}

}; // class IStream

} //namespace file
} //namespace mip

#endif // FILE_SDK_ISTREAM_H
