/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <stdint.h>
#include "../Platform/Logger/Logger.h"
#include "SimpleProtectedStream.h"
#include "RMSCryptoExceptions.h"

using namespace std;
using namespace rmscrypto::platform::logger;
namespace rmscrypto {
namespace api {
SimpleProtectedStream::SimpleProtectedStream(
  shared_ptr<ICryptoProvider>pCryptoProvider,
  shared_ptr<IStream>        pBackingStream,
  uint64_t                   u64ContentStart,
  uint64_t                   u64ContentSize)
  : m_locker(new mutex)
  , m_pCryptoProvider(pCryptoProvider)
  , m_pBackingStream(pBackingStream)
  , m_u64ContentStart(u64ContentStart)
  , m_u64ContentSize(u64ContentSize)
  , m_bIsPlainText(pCryptoProvider == nullptr)
{
  if (pBackingStream.get() == nullptr) {
    throw exceptions::RMSCryptoInvalidArgumentException("Bad parameter");
  }

  if (u64ContentStart > pBackingStream->Size()) {
    throw exceptions::RMSCryptoInvalidArgumentException("Bad parameter");
  }

  // m_u64ContentSize can be std::numeric_limits<uint64_t>::max(), which means
  // "until the end of the stram".
  // that's why we need to take a min of the actual stream size and the
  // m_u64ContentSize
  m_u64ContentSize = min(m_u64ContentSize,
                         m_pBackingStream->Size() - m_u64ContentStart);

  // seek to the start
  Seek(0);
}

shared_future<int64_t>SimpleProtectedStream::ReadAsync(uint8_t    *pbBuffer,
                                                       int64_t     cbBuffer,
                                                       int64_t     cbOffset,
                                                       std::launch launchType)
{
  return ReadInternalAsync(pbBuffer,
                           cbBuffer,
                           cbOffset,
                           launchType,
                           0,
                           false);
}

shared_future<int64_t>SimpleProtectedStream::ReadInternalAsync(
  uint8_t    *pbBuffer,
  int64_t     cbBuffer,
  int64_t     cbOffset,
  std::launch launchType,
  uint32_t    u32StartingBlockNumber,
  bool        bIsFinal)
{
  if (m_bIsPlainText)
  {
    // lock resources
    unique_lock<mutex> lock(*m_locker);

    // calculate the number of uint8_ts left in the stream
    uint64_t u64ContentLeft = m_u64ContentSize - cbOffset;
    uint64_t toReadSize     =
      min(static_cast<uint64_t>(cbBuffer), u64ContentLeft);

    return m_pBackingStream->ReadAsync(pbBuffer, toReadSize, cbOffset,
                                       launchType);
  }

  auto selfPtr = this->shared_from_this();

  return std::async(launchType, [](shared_ptr<SimpleProtectedStream>self,
                                   uint8_t *buffer,
                                   int64_t  bSize,
                                   int64_t  offset,
                                   uint32_t startingBlockNumber,
                                   bool     isFinal) -> int64_t
      {
        // lock resources
        unique_lock<mutex>lock(*self->m_locker);

        // calculate the number of uint8_ts left in the stream
        uint64_t u64ContentLeft = self->m_u64ContentSize - offset;
        uint64_t toRead = min(static_cast<uint64_t>(bSize), u64ContentLeft);

        // seek to Read
        self->SeekInternal(offset);

        // read the cipherText from the backing stream (make sure we don't read
        // more
        // than u64ContentLeft)
        vector<uint8_t>cipherText = self->m_pBackingStream->Read(toRead);

        // decrypt the ciphertext into the supplied buffer
        uint32_t cbOut = 0;

        if (cipherText.size() > 0)
        {
          self->m_pCryptoProvider->Decrypt(&cipherText[0],
                                           static_cast<uint32_t>(cipherText.size()),
                                           startingBlockNumber, isFinal,
                                           buffer, static_cast<uint32_t>(bSize),
                                           &cbOut);
        }

        return static_cast<int64_t>(cbOut);
      }, selfPtr, pbBuffer, cbBuffer, cbOffset, u32StartingBlockNumber,
                    bIsFinal);
}

shared_future<int64_t>SimpleProtectedStream::WriteAsync(const uint8_t *cpbBuffer,
                                                        int64_t        cbBuffer,
                                                        int64_t        cbOffset,
                                                        std::launch    launchType)
{
  return WriteInternalAsync(cpbBuffer,
                            cbBuffer,
                            cbOffset,
                            launchType,
                            0,
                            false);
}

shared_future<int64_t>SimpleProtectedStream::WriteInternalAsync(
  const uint8_t *cpbBuffer,
  int64_t        cbBuffer,
  int64_t        cbOffset,
  std::launch    launchType,
  uint32_t       u32StartingBlockNumber,
  bool           bIsFinal)
{
  auto selfPtr = this->shared_from_this();

  return std::async(launchType, [](shared_ptr<SimpleProtectedStream>self,
                                   const uint8_t *buffer,
                                   int64_t  bSize,
                                   int64_t  offset,
                                   uint32_t       startingBlockNumber,
                                   bool           isFinal) -> int64_t
      {
        uint32_t cbOut = (uint32_t)(bSize);
        vector<uint8_t>cipherText;

        // lock resources
        unique_lock<mutex>lock(*self->m_locker);

        if (self->m_bIsPlainText)
        {
          cipherText = vector<uint8_t>(buffer, buffer + bSize);
        }
        else
        {
          // get the encrypted size
          auto encryptedSize = self->m_pCryptoProvider->GetCipherTextSize(bSize);

          cipherText.resize(encryptedSize);

          Logger::Hidden("writing block #%d", startingBlockNumber);

          // encrypt the supplied buffer into cipherText
          self->m_pCryptoProvider->Encrypt(buffer, static_cast<uint32_t>(bSize),
                                           startingBlockNumber, isFinal,
                                           &cipherText[0],
                                           static_cast<uint32_t>(cipherText.size()),
                                           &cbOut);
        }

        // cipherText.size();
        // write the cipherText to the buffer
        int64_t u64Written =
          self->m_pBackingStream->WriteAsync(&cipherText[0],
                                             cbOut,
                                             offset + self->m_u64ContentStart,
                                             std::launch::deferred).get();

        // adjust the content size
        self->m_u64ContentSize =
          max(self->m_u64ContentSize, offset + static_cast<uint64_t>(bSize));

        return u64Written;
      }, selfPtr, cpbBuffer,
                    cbBuffer,
                    cbOffset,
                    u32StartingBlockNumber,
                    bIsFinal);
}

future<bool>SimpleProtectedStream::FlushAsync(std::launch launchType)
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return m_pBackingStream->FlushAsync(launchType);
}

int64_t SimpleProtectedStream::Read(uint8_t *pbBuffer,
                                    int64_t  cbBuffer) {
  return ReadAsync(pbBuffer, cbBuffer, Position(), std::launch::deferred).get();
}

int64_t SimpleProtectedStream::Write(const uint8_t *cpbBuffer,
                                     int64_t        cbBuffer) {
  return WriteAsync(cpbBuffer, cbBuffer, Position(), std::launch::deferred).get();
}

bool SimpleProtectedStream::Flush() {
  return FlushAsync(std::launch::deferred).get();
}

void SimpleProtectedStream::Seek(uint64_t u64Position)
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return SeekInternal(u64Position);
}

void SimpleProtectedStream::SeekInternal(uint64_t u64Position)
{
  m_pBackingStream->Seek(m_u64ContentStart + u64Position);
}

shared_ptr<IStream>SimpleProtectedStream::Clone()
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return static_pointer_cast<IStream>(shared_ptr<SimpleProtectedStream>(
                                        new SimpleProtectedStream(
                                          m_pCryptoProvider,
                                          m_pBackingStream
                                          ->Clone(),
                                          m_u64ContentStart,
                                          m_u64ContentSize)));
}

bool SimpleProtectedStream::CanRead() const
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return m_pBackingStream->CanRead();
}

bool SimpleProtectedStream::CanWrite() const
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return m_pBackingStream->CanWrite();
}

uint64_t SimpleProtectedStream::Position()
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return m_pBackingStream->Position() - m_u64ContentStart;
}

uint64_t SimpleProtectedStream::Size()
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return SizeInternal();
}

uint64_t SimpleProtectedStream::SizeInternal()
{
  return m_u64ContentSize;
}

void SimpleProtectedStream::Size(uint64_t u64Value)
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  if (m_u64ContentSize > u64Value)
  {
    m_u64ContentSize = u64Value;
    m_pBackingStream->Size(m_u64ContentStart + m_u64ContentSize);
  }
  else if (m_u64ContentSize < u64Value)
  {
    AppendZeros(u64Value - m_u64ContentSize);
  }
}

void SimpleProtectedStream::AppendZeros(uint64_t u64Count)
{
  vector<uint8_t> zeros(4096, static_cast<uint8_t>(0));

  uint64_t u64OriginalPosition = Position();
  try
  {
    SeekInternal(m_u64ContentSize);

    while (u64Count > 0)
    {
      uint64_t u64Written =
        WriteInternalAsync(&zeros[0],
                           min<uint64_t>(zeros.size(), u64Count),
                           Position(),
                           std::launch::deferred,
                           0,
                           false).get();

      if (0 == u64Written)
      {
        throw exceptions::RMSCryptoIOException(
                exceptions::RMSCryptoException::UnknownError,
                "Write error");
      }

      u64Count -= u64Written;
    }
  }
  catch (exceptions::RMSCryptoException)
  {
    Seek(u64OriginalPosition);
    throw;
  }
  Seek(u64OriginalPosition);
}

SimpleProtectedStream::~SimpleProtectedStream()
{
  // NOT SURE (need detach)
}
} // namespace api
} // namespace rmscrypto
