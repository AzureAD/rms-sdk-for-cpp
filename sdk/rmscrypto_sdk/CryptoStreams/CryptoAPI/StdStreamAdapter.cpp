/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <assert.h>
#include "StdStreamAdapter.h"
#include "RMSCryptoExceptions.h"

using namespace std;
namespace rmscrypto {
namespace api {
StdStreamAdapter::StdStreamAdapter(shared_ptr<iostream>backingIOStream)
  : m_locker(new mutex)
  , m_iBackingStream(static_pointer_cast<istream>(backingIOStream))
  , m_oBackingStream(static_pointer_cast<ostream>(backingIOStream))
{}

StdStreamAdapter::StdStreamAdapter(shared_ptr<ostream>backingOStream)
  : m_locker(new mutex)
  , m_oBackingStream(backingOStream)
{}

StdStreamAdapter::StdStreamAdapter(shared_ptr<istream>backingIStream)
  : m_locker(new mutex)
  , m_iBackingStream(backingIStream)
{}

// copy all properties include mutex to support thread safety work!
StdStreamAdapter::StdStreamAdapter(std::shared_ptr<StdStreamAdapter>from)
  : m_locker(from->m_locker)
  , m_iBackingStream(from->m_iBackingStream)
  , m_oBackingStream(from->m_oBackingStream)
{}

shared_future<int64_t>StdStreamAdapter::ReadAsync(uint8_t      *pbBuffer,
                                                  const int64_t cbBuffer,
                                                  const int64_t cbOffset,
                                                  bool          fCreateBackingThread)
{
  auto selfPtr = shared_from_this();

  return async(fCreateBackingThread ? launch::async : launch::deferred, [](
                 shared_ptr<StdStreamAdapter>self,
                 uint8_t      *buffer,
                 const int64_t size,
                 int64_t offset) -> int64_t {
        // first lock object
        self->m_locker->lock();

        if (self->m_iBackingStream.get() != nullptr) {
          self->m_iBackingStream->seekg(offset);
        } else {
          self->m_locker->unlock();

          // unavailable
          throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoIOException::OperationUnavailable,
            "Operation unavailable!");
        }
        auto ret = static_cast<int64_t>(self->ReadInternal(buffer, size));
        self->m_locker->unlock();

        return ret;
      }, selfPtr, pbBuffer, cbBuffer, cbOffset);
}

shared_future<int64_t>StdStreamAdapter::WriteAsync(const uint8_t *cpbBuffer,
                                                   const int64_t  cbBuffer,
                                                   const int64_t  cbOffset,
                                                   bool           fCreateBackingThread)
{
  auto selfPtr = shared_from_this();

  return async(fCreateBackingThread ? launch::async : launch::deferred, [](
                 shared_ptr<StdStreamAdapter>self,
                 const uint8_t *buffer,
                 const int64_t size,
                 int64_t offset) -> int64_t {
        // first lock object
        self->m_locker->lock();

        // seek to position and write
        if (self->m_oBackingStream.get() != nullptr) {
          self->m_oBackingStream->seekp(offset);
        } else {
          self->m_locker->unlock();

          // unavailable
          throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoIOException::OperationUnavailable,
            "Operation unavailable!");
        }
        int64_t ret = 0;

        try {
          ret = static_cast<int64_t>(self->WriteInternal(buffer, size));
        }
        catch (exception& e) {
          self->m_locker->unlock();
          throw e;
        }
        self->m_locker->unlock();

        return ret;
      }, selfPtr, cpbBuffer, cbBuffer, cbOffset);
}

future<bool>StdStreamAdapter::FlushAsync(bool fCreateBackingThread) {
  auto selfPtr = shared_from_this();

  return async(fCreateBackingThread ? launch::async : launch::deferred, [](
                 shared_ptr<StdStreamAdapter>self) -> bool {
        return self->Flush();
      }, selfPtr);
}

// Sync methods
int64_t StdStreamAdapter::Read(uint8_t      *pbBuffer,
                               const int64_t cbBuffer) {
  if (m_iBackingStream.get() == nullptr) {
    // unavailable
    throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoIOException::OperationUnavailable,
            "Operation unavailable!");
  }

  lock_guard<mutex> locker(*m_locker);
  return ReadInternal(pbBuffer, cbBuffer);
}

int64_t StdStreamAdapter::ReadInternal(uint8_t      *pbBuffer,
                                       const int64_t cbBuffer) {
  m_iBackingStream->read(reinterpret_cast<char *>(pbBuffer), cbBuffer);
  return m_iBackingStream->gcount();
}

int64_t StdStreamAdapter::Write(const uint8_t *cpbBuffer,
                                const int64_t  cbBuffer) {
  if (m_oBackingStream.get() == nullptr) {
    // unavailable
    throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoIOException::OperationUnavailable,
            "Operation unavailable!");
  }
  m_locker->lock();
  try {
    WriteInternal(cpbBuffer, cbBuffer);
  }
  catch (exception& e) {
    m_locker->unlock();
    throw e;
  }
  m_locker->unlock();

  return cbBuffer;
}

int64_t StdStreamAdapter::WriteInternal(const uint8_t *cpbBuffer,
                                        const int64_t  cbBuffer) {
  assert(cpbBuffer != nullptr || cbBuffer == 0);

  m_oBackingStream->write(reinterpret_cast<const char *>(cpbBuffer), cbBuffer);

  return cbBuffer;
}

bool StdStreamAdapter::Flush()                        {
  // first lock object
  lock_guard<mutex> lock(*m_locker);

  // seek to position and write
  if (m_oBackingStream.get() != nullptr) {
    m_oBackingStream->flush();
  }
  return true;
}

SharedStream StdStreamAdapter::Clone() {
  lock_guard<mutex> locker(*m_locker);
  return static_pointer_cast<IStream>(shared_ptr<StdStreamAdapter>(new
                                                                   StdStreamAdapter(
                                                                     shared_from_this())));
}

void StdStreamAdapter::Seek(uint64_t u64Position) {
  // first lock object
  lock_guard<mutex> lock(*m_locker);

  if (m_iBackingStream.get() != nullptr) {
    m_iBackingStream->seekg(u64Position);
  }

  if (m_oBackingStream.get() != nullptr) {
    m_oBackingStream->seekp(u64Position);
  }
}

bool StdStreamAdapter::CanRead()                  {
  return m_iBackingStream.get() != nullptr;
}

bool StdStreamAdapter::CanWrite()                 {
  return m_oBackingStream.get() != nullptr;
}

uint64_t StdStreamAdapter::Position()                 {
  int ret = 0;

  lock_guard<mutex> locker(*m_locker);

  if (m_iBackingStream.get() != nullptr) {
    ret = static_cast<int>(m_iBackingStream->tellg());
  }

  if (m_oBackingStream.get() != nullptr) {
    ret = static_cast<int>(m_oBackingStream->tellp());
  }
  return static_cast<uint64_t>(ret);
}

uint64_t StdStreamAdapter::Size()                     {
  int ret = 0;

  lock_guard<mutex> locker(*m_locker);

  if (m_iBackingStream.get() != nullptr) {
    auto oldPos =  m_iBackingStream->tellg();
    m_iBackingStream->seekg(0, ios_base::end);
    ret =  static_cast<int>(m_iBackingStream->tellg());
    m_iBackingStream->seekg(oldPos);
  }

  if (m_oBackingStream.get() != nullptr) {
    auto oldPos =  m_oBackingStream->tellp();
    m_oBackingStream->seekp(0, ios_base::end);
    ret =  static_cast<int>(m_oBackingStream->tellp());
    m_oBackingStream->seekp(oldPos);
  }
  return static_cast<uint64_t>(ret);
}

void StdStreamAdapter::Size(uint64_t /*u64Value*/) {}
} // namespace api
} // namespace rmscrypto
