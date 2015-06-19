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

  virtual std::shared_future<int64_t>ReadAsync(uint8_t      *pbBuffer,
                                               const int64_t cbBuffer,
                                               const int64_t cbOffset,
                                               bool          fCreateBackingThread)
  override;
  virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                const int64_t  cbBuffer,
                                                const int64_t  cbOffset,
                                                bool           fCreateBackingThread)
  override;
  virtual std::future<bool>FlushAsync(bool fCreateBackingThread) override;

  // Sync methods
  virtual int64_t          Read(uint8_t      *pbBuffer,
                                const int64_t cbBuffer) override;
  virtual int64_t          Write(const uint8_t *cpbBuffer,
                                 const int64_t  cbBuffer) override;
  virtual bool             Flush() override;

  virtual SharedStream     Clone() = 0;

  virtual void             Seek(uint64_t u64Position) override;
  virtual bool             CanRead()                  override;
  virtual bool             CanWrite()                 override;
  virtual uint64_t         Position()                 override;
  virtual uint64_t         Size()                     override;
  virtual void             Size(uint64_t u64Value)    override;
};
} // namespace api
} // namespace rmscrypto
#endif // ifndef _CRYPTO_STREAMS_LIB_SECRETSTREAM_H_
