/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _CRYPTO_STREAMS_LIB_SIMPLEPROTECTEDSTREAM_H_
#define _CRYPTO_STREAMS_LIB_SIMPLEPROTECTEDSTREAM_H_

#include "IStream.h"
#include "ICryptoProvider.h"

namespace rmscrypto {
namespace api {
class SimpleProtectedStream : public IStream,
                              public std::enable_shared_from_this
                              <SimpleProtectedStream>{
  friend class CachedBlock;

public:

  SimpleProtectedStream(std::shared_ptr<ICryptoProvider>pCryptoProvider,
                        std::shared_ptr<IStream>        pBackingStream,
                        uint64_t                        u64ContentStart,
                        uint64_t                        u64ContentSize);

  // IStream implementation
  virtual std::shared_future<int64_t>ReadAsync(uint8_t      *pbBuffer,
                                               int64_t cbBuffer,
                                               int64_t cbOffset,
                                               std::launch   launchType)
  override;
  virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t  cbBuffer,
                                                int64_t  cbOffset,
                                                std::launch    launchType)
  override;
  virtual std::future<bool>FlushAsync(std::launch launchType)
  override;

  virtual int64_t          Read(uint8_t      *pbBuffer,
                                int64_t cbBuffer) override;
  virtual int64_t          Write(const uint8_t *cpbBuffer,
                                 int64_t  cbBuffer) override;
  virtual bool             Flush() override;

  virtual SharedStream     Clone() override;

  virtual void             Seek(uint64_t u64Position) override;
  virtual bool             CanRead()  const           override;
  virtual bool             CanWrite() const           override;
  virtual uint64_t         Position()                 override;
  virtual uint64_t         Size()                     override;
  virtual void             Size(uint64_t u64Value)    override;

  virtual ~SimpleProtectedStream() override;

  std::shared_future<int64_t>ReadInternalAsync(uint8_t      *pbBuffer,
                                               int64_t cbBuffer,
                                               int64_t cbOffset,
                                               std::launch   launchType,
                                               uint32_t      u32StartingBlockNumber,
                                               bool          bIsFinal);
  std::shared_future<int64_t>WriteInternalAsync(const uint8_t *cpbBuffer,
                                                int64_t  cbBuffer,
                                                int64_t  cbOffset,
                                                std::launch    launchType,
                                                uint32_t       u32StartingBlockNumber,
                                                bool           bIsFinal);

private:

  uint64_t               SizeInternal();
  void                   SeekInternal(uint64_t u64Position);

  SimpleProtectedStream(const SimpleProtectedStream&)            = delete;
  SimpleProtectedStream& operator=(const SimpleProtectedStream&) = delete;

private:

  void AppendZeros(uint64_t u64Count);

private:

  std::shared_ptr<std::mutex> m_locker;

  std::shared_ptr<ICryptoProvider> m_pCryptoProvider;
  std::shared_ptr<IStream> m_pBackingStream;

  uint64_t m_u64ContentStart;
  uint64_t m_u64ContentSize;
  bool     m_bIsPlainText;
};
} // namespace api
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_SIMPLEPROTECTEDSTREAM_H_
