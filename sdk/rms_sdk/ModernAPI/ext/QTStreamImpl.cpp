/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include "QTStreamImpl.h"
#include "../../Platform/Logger/Logger.h"
using namespace rmscrypto::api;
using namespace rmscore::platform::logger;
using namespace std;


SharedStream QTStreamImpl::Create(QSharedPointer<QDataStream>stream) {
  auto self = new QTStreamImpl(stream);

  return static_pointer_cast<IStream>(shared_ptr<QTStreamImpl>(self));
}

QTStreamImpl::QTStreamImpl(QSharedPointer<QDataStream>stream)
  : stream_(stream) {}

shared_future<int64_t>QTStreamImpl::ReadAsync(uint8_t    *pbBuffer,
                                              int64_t     cbBuffer,
                                              int64_t     cbOffset,
                                              std::launch launchType)
{
  auto selfPtr = shared_from_this();

  return std::async(launchType, [](
                      std::shared_ptr<QTStreamImpl>self,
                      uint8_t      *buffer,
                      int64_t size,
                      int64_t offset) -> int64_t {
    // first lock object
    unique_lock<mutex>lock(self->locker_);

    // seek to position and read
    self->stream_->device()->seek(offset);
    auto ret = static_cast<int64_t>(self->stream_->readRawData(
                                      reinterpret_cast<char *>(buffer),
                                      static_cast<int>(size)));
    return ret;
  }, selfPtr, pbBuffer, cbBuffer, cbOffset);
}

shared_future<int64_t>QTStreamImpl::WriteAsync(const uint8_t *cpbBuffer,
                                               int64_t        cbBuffer,
                                               int64_t        cbOffset,
                                               std::launch    launchType)
{
  auto selfPtr = shared_from_this();

  return std::async(launchType, [](
                      std::shared_ptr<QTStreamImpl>self,
                      const uint8_t *buffer,
                      int64_t size,
                      int64_t offset) -> int64_t {
    // first lock object
    unique_lock<mutex>lock(self->locker_);

    Logger::Hidden("Write %I64d bytes at offset = %I64d from buffer = %x", size,
                   offset, buffer);

    // seek to position and write
    self->stream_->device()->seek(offset);
    auto ret = static_cast<int64_t>(self->stream_->writeRawData(
                                      reinterpret_cast<const char *>(buffer),
                                      static_cast<int>(size)));
    return ret;
  }, selfPtr, cpbBuffer, cbBuffer, cbOffset);
}

std::future<bool>QTStreamImpl::FlushAsync(std::launch /*launchType*/) {
  // it's not used in QDataStream
  std::promise<bool> val;
  std::future<bool>  res = val.get_future();

  val.set_value(true);
  return res;
}

void QTStreamImpl::Seek(uint64_t u64Position) {
  // first lock object
  unique_lock<mutex> lock(locker_);
  stream_->device()->seek(u64Position);
}

int64_t QTStreamImpl::Read(uint8_t *pbBuffer,
                           int64_t  cbBuffer) {
  return ReadAsync(pbBuffer, cbBuffer, Position(), std::launch::deferred).get();
}

int64_t QTStreamImpl::Write(const uint8_t *cpbBuffer,
                            int64_t        cbBuffer) {
  return WriteAsync(cpbBuffer, cbBuffer, Position(), std::launch::deferred).get();
}

bool QTStreamImpl::Flush() {
  return FlushAsync(std::launch::deferred).get();
}

SharedStream QTStreamImpl::Clone() {
  // first lock object
  unique_lock<mutex> lock(locker_);
  auto self =
    new QTStreamImpl(QSharedPointer<QDataStream>::create(stream_->device()));

  return static_pointer_cast<IStream>(shared_ptr<QTStreamImpl>(self));
}

bool QTStreamImpl::CanRead() const {
  return (stream_->device()->openMode() & QIODevice::ReadOnly) ==
         QIODevice::ReadOnly;
}

bool QTStreamImpl::CanWrite() const {
  return (stream_->device()->openMode() & QIODevice::WriteOnly) ==
         QIODevice::WriteOnly;
}

uint64_t QTStreamImpl::Position() {
  // first lock object
  unique_lock<mutex> lock(locker_);
  return stream_->device()->pos();
}

uint64_t QTStreamImpl::Size() {
  // first lock object
  unique_lock<mutex> lock(locker_);
  return stream_->device()->size();
}

void QTStreamImpl::Size(uint64_t /*u64Value*/) {}
