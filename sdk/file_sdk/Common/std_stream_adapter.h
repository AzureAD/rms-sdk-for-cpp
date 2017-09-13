#ifndef FILE_SDK_STDSTREAMADAPTER_H
#define FILE_SDK_STDSTREAMADAPTER_H

#include <iostream>
#include "istream.h"
#include <Common/exceptions.h>

using std::static_pointer_cast;
using std::make_shared;

namespace mip {
namespace file {

class StdStreamAdapter
    : public IStream,
      public std::enable_shared_from_this<StdStreamAdapter> {
public:

  static std::shared_ptr<IStream> Create(std::shared_ptr<std::istream> stdInputStream) {
    return static_pointer_cast<IStream>(make_shared<StdStreamAdapter>(stdInputStream));
  }

  static std::shared_ptr<IStream> Create(std::shared_ptr<std::ostream> stdOutputStream) {
    return static_pointer_cast<IStream>(make_shared<StdStreamAdapter>(stdOutputStream));
  }

  static std::shared_ptr<IStream> Create(std::shared_ptr<std::iostream> stdStream) {
    return static_pointer_cast<IStream>(make_shared<StdStreamAdapter>(stdStream));
  }

  StdStreamAdapter(std::shared_ptr<std::iostream> stdStream);
  StdStreamAdapter(std::shared_ptr<std::ostream> stdOutputStream);
  StdStreamAdapter(std::shared_ptr<std::istream> stdInputStream);

  virtual int64_t Read(uint8_t *buffer, int64_t  bufferLength) override;
  virtual int64_t Write(const uint8_t *buffer, int64_t bufferLength) override;
  virtual bool Flush() override;
  virtual void Seek(uint64_t position) override;
  virtual bool CanRead() const override;
  virtual bool CanWrite() const override;
  virtual uint64_t Position() override;
  virtual uint64_t Size() override;
  virtual void Size(uint64_t value) override;
  virtual std::shared_ptr<IStream> Clone() override;

private:

  StdStreamAdapter(std::shared_ptr<StdStreamAdapter>from);

  std::shared_ptr<std::istream> mInputStream;
  std::shared_ptr<std::ostream> mOutputStream;

  int64_t ReadInternal(uint8_t *buffer, int64_t  bufferLength);
  int64_t WriteInternal(const uint8_t *buffer, int64_t bufferLength);
};

class StdStreamIOException : public IOException
{
public:
  StdStreamIOException(const std::string& error, const std::string& message) : IOException(error, message){}
  StdStreamIOException(const std::string& error) : StdStreamIOException(error, ""){}
};

} //namespace file
} //namespace mip

#endif // FILE_SDK_STDSTREAMADAPTER_H
