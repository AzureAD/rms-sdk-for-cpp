/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_QDATASTREAM_H_
#define _RMS_LIB_QDATASTREAM_H_

#include <mutex>
#include "../ModernAPIExport.h"
#include <QSharedPointer>
#include <QDataStream>
#include <CryptoAPI.h>

class DLL_PUBLIC_RMS QTStreamImpl :
  public rmscrypto::api::IStream,
  public std::enable_shared_from_this<QTStreamImpl>{
public:

  static rmscrypto::api::SharedStream Create(QSharedPointer<QDataStream>stream);
  virtual ~QTStreamImpl() {}


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

private:

  QTStreamImpl(QSharedPointer<QDataStream>stream);
  QTStreamImpl() = delete;

  QSharedPointer<QDataStream> stream_;
  std::mutex locker_; // QDataStream is not thread safe!!!
};
#endif // ifndef _RMS_LIB_QDATASTREAM_H_
