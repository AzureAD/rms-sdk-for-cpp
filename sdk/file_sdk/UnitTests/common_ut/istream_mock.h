#ifndef ISTREAMMOCK_H
#define ISTREAMMOCK_H

#include "istream.h"

using mip::file::IStream;

class IStreamMock
    : public IStream,
      public std::enable_shared_from_this<IStreamMock> {
public:

  virtual int64_t Read(uint8_t *buffer, int64_t bufferLength) override;
  virtual int64_t Write(const uint8_t *buffer, int64_t bufferLength) override;
  virtual bool Flush() override;
  virtual void Seek(uint64_t position) override;
  virtual bool CanRead() const override;
  virtual bool CanWrite() const override;
  virtual uint64_t Position() override;
  virtual uint64_t Size() override;
  virtual void Size(uint64_t value) override;
  virtual std::shared_ptr<IStream> Clone() override;
  virtual ~IStreamMock() override;
};

#endif // ISTREAMMOCK_H
