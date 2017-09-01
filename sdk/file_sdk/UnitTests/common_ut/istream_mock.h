#ifndef ISTREAMMOCK_H
#define ISTREAMMOCK_H

#include <IStream.h>

using rmscrypto::api::IStream;

class IStreamMock : public IStream,
                           public std::enable_shared_from_this<IStreamMock>{
public:
  IStreamMock();

  virtual std::shared_future<int64_t>ReadAsync(uint8_t    *pbBuffer,
                                               int64_t     cbBuffer,
                                               int64_t     cbOffset,
                                               std::launch launchType) override;
  virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
  override;
  virtual std::future<bool>FlushAsync(std::launch launchType) override;

  // Sync methods
  virtual int64_t          Read(uint8_t *pbBuffer,
                                int64_t  cbBuffer) override;
  virtual int64_t          Write(const uint8_t *cpbBuffer,
                                 int64_t        cbBuffer) override;
  virtual bool             Flush()                        override;

  virtual std::shared_ptr<IStream>     Clone() override;

  virtual void             Seek(uint64_t u64Position) override;
  virtual bool             CanRead()  const           override;
  virtual bool             CanWrite() const           override;
  virtual uint64_t         Position()                 override;
  virtual uint64_t         Size()                     override;
  virtual void             Size(uint64_t u64Value)    override;

  virtual ~IStreamMock() override;
};

#endif // ISTREAMMOCK_H
