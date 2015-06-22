/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _CRYPTO_STREAMS_LIB_SECRETSTREAM_H_
#define _CRYPTO_STREAMS_LIB_SECRETSTREAM_H_
#include <stdint.h>
#include "IStream.h"
#include "CryptoAPI.h"

namespace rmscrypto {
namespace api {
class ICryptoStream : public IStream {
public:

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

  // Sync methods
  virtual int64_t          Read(uint8_t *pbBuffer,
                                int64_t  cbBuffer) override;
  virtual int64_t          Write(const uint8_t *cpbBuffer,
                                 int64_t        cbBuffer) override;
  virtual bool             Flush() override;

  virtual SharedStream     Clone() = 0;

  virtual void             Seek(uint64_t u64Position) override;
  virtual bool             CanRead()  const           override;
  virtual bool             CanWrite() const           override;
  virtual uint64_t         Position()                 override;
  virtual uint64_t         Size()                     override;
  virtual void             Size(uint64_t u64Value)    override;
};
} // namespace api
} // namespace rmscrypto
#endif // ifndef _CRYPTO_STREAMS_LIB_SECRETSTREAM_H_
