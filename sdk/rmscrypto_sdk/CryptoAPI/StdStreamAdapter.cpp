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

shared_future<int64_t>StdStreamAdapter::ReadAsync(uint8_t *pbBuffer,
                                                  int64_t  cbBuffer,
                                                  int64_t  cbOffset,
                                                  launch   launchType)
{
  auto selfPtr = shared_from_this();

  return async(launchType, [](shared_ptr<StdStreamAdapter>self,
                              uint8_t      *buffer,
                              int64_t size,
                              int64_t offset) -> int64_t {
        // first lock object
        lock_guard<mutex>lock(*self->m_locker);

        if (self->m_iBackingStream.get() != nullptr) {
          self->m_iBackingStream->clear();
          self->m_iBackingStream->seekg(offset);
        } else {
          // unavailable
          throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoIOException::OperationUnavailable,
            "Operation unavailable!");
        }
        auto ret = static_cast<int64_t>(self->ReadInternal(buffer, size));

        return ret;
      }, selfPtr, pbBuffer, cbBuffer, cbOffset);
}

shared_future<int64_t>StdStreamAdapter::WriteAsync(const uint8_t *cpbBuffer,
                                                   int64_t        cbBuffer,
                                                   int64_t        cbOffset,
                                                   launch         launchType)
{
  auto selfPtr = shared_from_this();

  return async(launchType, [](shared_ptr<StdStreamAdapter>self,
                              const uint8_t *buffer,
                              int64_t size,
                              int64_t offset) -> int64_t {
        // first lock object
        lock_guard<mutex>lock(*self->m_locker);

        // seek to position and write
        if (self->m_oBackingStream.get() != nullptr) {
          self->m_oBackingStream->seekp(offset);
        } else {
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
          throw e;
        }
        return ret;
      }, selfPtr, cpbBuffer, cbBuffer, cbOffset);
}

future<bool>StdStreamAdapter::FlushAsync(launch launchType) {
  auto selfPtr = shared_from_this();

  return async(launchType, [](shared_ptr<StdStreamAdapter>self) -> bool {
        return self->Flush();
      }, selfPtr);
}

// Sync methods
int64_t StdStreamAdapter::Read(uint8_t *pbBuffer,
                               int64_t  cbBuffer) {
  if (m_iBackingStream.get() == nullptr) {
    // unavailable
    throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoIOException::OperationUnavailable,
            "Operation unavailable!");
  }

  lock_guard<mutex> locker(*m_locker);
  return ReadInternal(pbBuffer, cbBuffer);
}

int64_t StdStreamAdapter::ReadInternal(uint8_t *pbBuffer,
                                       int64_t  cbBuffer) {
    auto fail = m_iBackingStream->fail();
    // Windows implementation of StringStream has a bug
    // that for an empty stream fail bit is set after calling seekg(0).
    // Workaround the bug by calling clear() which clears the failures.
    if(fail){
        m_iBackingStream->clear();
    }
  m_iBackingStream->read(reinterpret_cast<char *>(pbBuffer), cbBuffer);
  return m_iBackingStream->gcount();
}

int64_t StdStreamAdapter::Write(const uint8_t *cpbBuffer,
                                int64_t        cbBuffer) {
  if (m_oBackingStream.get() == nullptr) {
    // unavailable
    throw exceptions::RMSCryptoIOException(
            exceptions::RMSCryptoIOException::OperationUnavailable,
            "Operation unavailable!");
  }

  // first lock object
  lock_guard<mutex> lock(*m_locker);
  try {
    WriteInternal(cpbBuffer, cbBuffer);
  }
  catch (exception& e) {
    throw e;
  }
  return cbBuffer;
}

int64_t StdStreamAdapter::WriteInternal(const uint8_t *cpbBuffer,
                                        int64_t        cbBuffer) {
  assert(cpbBuffer != nullptr || cbBuffer == 0);

  auto fail = m_oBackingStream->fail();
  // Windows implementation of StringStream has a bug
  // that for an empty stream fail bit is set after calling seekg(0).
  // Workaround the bug by calling clear() which clears the failures.
  if(fail){
      m_oBackingStream->clear();
  }
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
    m_iBackingStream->clear();
    m_iBackingStream->seekg(u64Position);
  }

  if (m_oBackingStream.get() != nullptr) {
    m_oBackingStream->seekp(u64Position);
  }
}

bool StdStreamAdapter::CanRead() const {
  return m_iBackingStream.get() != nullptr;
}

bool StdStreamAdapter::CanWrite() const {
  return m_oBackingStream.get() != nullptr;
}

uint64_t StdStreamAdapter::Position() {
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

uint64_t StdStreamAdapter::Size() {
  int ret = 0;

  lock_guard<mutex> locker(*m_locker);

  if (m_iBackingStream.get() != nullptr) {
    m_iBackingStream->clear();
    auto oldPos =  m_iBackingStream->tellg();
    m_iBackingStream->seekg(0, ios_base::end);
    ret =  static_cast<int>(m_iBackingStream->tellg());
    m_iBackingStream->seekg(oldPos);
    m_iBackingStream->clear();
  }

  if (m_oBackingStream.get() != nullptr) {
    m_oBackingStream->clear();
    auto oldPos =  m_oBackingStream->tellp();
    m_oBackingStream->seekp(0, ios_base::end);
    ret =  static_cast<int>(m_oBackingStream->tellp());
    m_oBackingStream->seekp(oldPos);
    m_oBackingStream->clear();
  }
  return static_cast<uint64_t>(ret);
}

void StdStreamAdapter::Size(uint64_t /*u64Value*/) {}
} // namespace api
} // namespace rmscrypto
