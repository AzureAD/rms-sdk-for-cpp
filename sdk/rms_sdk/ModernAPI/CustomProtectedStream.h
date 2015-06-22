/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_CUSTOMPROTECTEDSTREAM_H_
#define _RMS_LIB_CUSTOMPROTECTEDSTREAM_H_

#include "UserPolicy.h"
#include "IStream.h"
#include "ModernAPIExport.h"

namespace rmscore {
namespace modernapi {
class DLL_PUBLIC_RMS CustomProtectedStream : public rmscrypto::api::IStream {
public:

  static std::shared_ptr<CustomProtectedStream>Create(
    std::shared_ptr<UserPolicy>  policy,
    rmscrypto::api::SharedStream stream,
    uint64_t                     contentStartPosition,
    uint64_t                     contentSize);

  static uint64_t GetEncryptedContentLength(
    std::shared_ptr<UserPolicy>policy,
    uint64_t                   contentLength);

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
  virtual std::future<bool>            FlushAsync(std::launch launchType)
  override;

  virtual int64_t                      Read(uint8_t *pbBuffer,
                                            int64_t  cbBuffer) override;
  virtual int64_t                      Write(const uint8_t *cpbBuffer,
                                             int64_t        cbBuffer) override;
  virtual bool                         Flush() override;

  virtual rmscrypto::api::SharedStream Clone() override;

  virtual void                         Seek(uint64_t u64Position) override;
  virtual bool                         CanRead()  const           override;
  virtual bool                         CanWrite() const           override;
  virtual uint64_t                     Position()                 override;
  virtual uint64_t                     Size()                     override;
  virtual void                         Size(uint64_t u64Value)    override;

  //
  virtual ~CustomProtectedStream();

protected:

  CustomProtectedStream(std::shared_ptr<IStream>pImpl);
  std::shared_ptr<IStream>GetImpl();

private:

  std::shared_ptr<IStream> m_pImpl;
};
} // namespace modernapi
} // namespace rmscore
#endif // _RMS_LIB_CUSTOMPROTECTEDSTREAM_H_
