/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_CRYPTO_ISTREAM_H_
#define _RMS_CRYPTO_ISTREAM_H_

#include <future>
#include <string>
#include <memory>
#include <vector>
#include "CryptoAPIExport.h"

namespace rmscrypto {
namespace api {
class IStream;
typedef std::shared_ptr<IStream> SharedStream;

/*!
@brief Base interface for protected streams.

Ported from [Windows::Storage::Streams::IRandomAccessStream][IRandomAccessStream] and
[Windows::Storage::Streams::FileRandomAccessStream][FileRandomAccessStream].

[IRandomAccessStream]: https://msdn.microsoft.com/en-us/library/windows/apps/windows.storage.streams.irandomaccessstream.aspx
[FileRandomAccessStream]: https://msdn.microsoft.com/en-us/library/windows/apps/windows.storage.streams.filerandomaccessstream.aspx

@todo Derive from [std::iostream](http://www.cplusplus.com/reference/istream/iostream/)?
*/
class IStream {
public:

  // Async methods. Be sure buffer exists until result will be got from
  // std::future
  virtual std::shared_future<int64_t> ReadAsync(uint8_t    *pbBuffer,
                                               int64_t     cbBuffer,
                                               int64_t     cbOffset,
                                               std::launch launchType)
    = 0;
  virtual std::shared_future<int64_t> WriteAsync(const uint8_t *cpbBuffer,
                                                int64_t        cbBuffer,
                                                int64_t        cbOffset,
                                                std::launch    launchType)
    = 0;
  virtual std::future<bool> FlushAsync(std::launch launchType) = 0;

  // Sync methods
  virtual int64_t             Read(uint8_t *pbBuffer,
                                   int64_t  cbBuffer) = 0;
  virtual int64_t             Write(const uint8_t *cpbBuffer,
                                    int64_t        cbBuffer) = 0;
  virtual bool                Flush()                        = 0;

  virtual SharedStream        Clone() = 0;

  virtual void                Seek(uint64_t u64Position) = 0;
  virtual bool                CanRead()  const           = 0;
  virtual bool                CanWrite() const           = 0;
  virtual uint64_t            Position()                 = 0;
  virtual uint64_t            Size()                     = 0;
  virtual void                Size(uint64_t u64Value)    = 0;

  virtual std::vector<uint8_t> Read(uint64_t u64size)
  {
    std::vector<uint8_t> plainText;

    if (u64size > 0)
    {
      plainText.resize(u64size);
      int actualSize =
        static_cast<int>(Read(&plainText[0], static_cast<int>(plainText.size())));
      plainText.resize(actualSize);
    }
    return plainText;
  }

protected:

  virtual ~IStream() {}
}; // class IStream
} // namespace api
} // namespace rmscrypto
#endif // _RMS_CRYPTO_ISTREAM_H_
