/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_STDSTREAMADAPTER_H
#define _CRYPTO_STREAMS_LIB_STDSTREAMADAPTER_H

#include <iostream>
#include "IStream.h"

namespace rmscrypto {
namespace api {
class StdStreamAdapter : public IStream,
        public std::enable_shared_from_this<StdStreamAdapter>
{
public:
    StdStreamAdapter(std::shared_ptr<std::iostream> backingIOStream);
    StdStreamAdapter(std::shared_ptr<std::ostream>  backingOStream);
    StdStreamAdapter(std::shared_ptr<std::istream>  backingIStream);

    virtual std::shared_future<int64_t>ReadAsync(uint8_t      *pbBuffer,
                                                 const int64_t cbBuffer,
                                                 const int64_t cbOffset,
                                                 bool          fCreateBackingThread) override;
    virtual std::shared_future<int64_t>WriteAsync(const uint8_t *cpbBuffer,
                                                  const int64_t  cbBuffer,
                                                  const int64_t  cbOffset,
                                                  bool           fCreateBackingThread) override;
    virtual std::future<bool>   FlushAsync(bool fCreateBackingThread) override;

    // Sync methods
    virtual int64_t             Read(uint8_t      *pbBuffer,
                                     const int64_t cbBuffer) override;
    virtual int64_t             Write(const uint8_t *cpbBuffer,
                                      const int64_t  cbBuffer) override;
    virtual bool                Flush()                        override;

    virtual SharedStream        Clone() override;

    virtual void                Seek(uint64_t u64Position) override;
    virtual bool                CanRead()                  override;
    virtual bool                CanWrite()                 override;
    virtual uint64_t            Position()                 override;
    virtual uint64_t            Size()                     override;
    virtual void                Size(uint64_t u64Value)    override;

private:
    StdStreamAdapter(std::shared_ptr<StdStreamAdapter> from);

    std::shared_ptr<std::mutex>    m_locker;
    std::shared_ptr<std::istream>  m_iBackingStream;
    std::shared_ptr<std::ostream>  m_oBackingStream;

    int64_t             ReadInternal(uint8_t      *pbBuffer,
                                     const int64_t cbBuffer);
    int64_t             WriteInternal(const uint8_t *cpbBuffer,
                                      const int64_t  cbBuffer);
};

} // namespace api
} // namespace rmscrypto
#endif // _CRYPTO_STREAMS_LIB_STDSTREAMADAPTER_H
