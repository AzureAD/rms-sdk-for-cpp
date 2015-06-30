/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _CRYPTO_STREAMS_LIB_PROTECTION_BLOCKBASEDPROTECTEDSTREAM_H_
#define _CRYPTO_STREAMS_LIB_PROTECTION_BLOCKBASEDPROTECTEDSTREAM_H_

#include <mutex>
#include "CryptoAPIExport.h"
#include "IStream.h"
#include "ICryptoProvider.h"
#include "SimpleProtectedStream.h"
#include "CachedBlock.h"

namespace rmscrypto {
namespace api {
class BlockBasedProtectedStream : public IStream,
                                  public std::enable_shared_from_this<
                                    BlockBasedProtectedStream>{
public:

  static DLL_PUBLIC_CRYPTO std::shared_ptr<BlockBasedProtectedStream>Create(
    std::shared_ptr<ICryptoProvider>pCryptoProvider,
    std::shared_ptr<IStream>        pBackingStream,
    uint64_t                        u64ContentStart,
    uint64_t                        u64ContentSize,
    uint64_t                        u64BlockSize);

  // IStream implementation
  virtual std::shared_future<int64_t>ReadAsync(uint8_t    *pbBuffer,
                                               int64_t     cbBuffer,
                                               int64_t     cbOffset,
                                               std::launch launchType)
  override;
  virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
  override;
  virtual std::future<bool>FlushAsync(std::launch launchType) override;

  virtual int64_t          Read(uint8_t *pbBuffer,
                                int64_t  cbBuffer) override;
  virtual int64_t          Write(const uint8_t *cpbBuffer,
                                 int64_t        cbBuffer) override;
  virtual bool             Flush() override;

  virtual SharedStream     Clone() override;

  virtual void             Seek(uint64_t u64Position) override;
  virtual bool             CanRead()  const           override;
  virtual bool             CanWrite() const           override;
  virtual uint64_t         Position()                 override;
  virtual uint64_t         Size()                     override;
  virtual void             Size(uint64_t u64Value)    override;

  virtual ~BlockBasedProtectedStream() override;

private:

  BlockBasedProtectedStream(
    std::shared_ptr<ICryptoProvider>pCryptoProvider,
    std::shared_ptr<IStream>        pBackingStream,
    uint64_t                        u64ContentStart,
    uint64_t                        u64ContentSize,
    uint64_t                        u64BlockSize);

  void                       SeekInternal(uint64_t u64Position);
  uint64_t                   PositionInner();
  uint64_t                   SizeInner();
  void                       SizeInner(uint64_t value);
  bool                       CanWriteInner() const;
  std::shared_future<int64_t>WriteInternalAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType,
                                                bool           fLockResources);

  void                       ProcessSizeChangeRequest();
  void                       SizeInternal(uint64_t size);
  void                       FillWithZeros(uint64_t newSize);

  BlockBasedProtectedStream(const BlockBasedProtectedStream&);
  BlockBasedProtectedStream& operator=(
    const BlockBasedProtectedStream&) = delete;

private:

  std::shared_ptr<std::mutex> m_locker;

  std::shared_ptr<SimpleProtectedStream> m_pSimple;
  std::shared_ptr<CachedBlock> m_pCachedBlock;

  uint64_t m_u64Position;
  bool     m_bIsPositionValid;
  uint64_t m_u64NewSize;
  bool     m_bIsPlainText;
};
} // namespace api
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_PROTECTION_BLOCKBASEDPROTECTEDSTREAM_H_
