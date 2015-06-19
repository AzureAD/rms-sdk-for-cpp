/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <limits>
#include "BlockBasedProtectedStream.h"
#include "RMSCryptoExceptions.h"
using namespace std;
namespace rmscrypto {
namespace api {
shared_ptr<BlockBasedProtectedStream>BlockBasedProtectedStream::Create(
  shared_ptr<ICryptoProvider>pCryptoProvider,
  shared_ptr<IStream>        pBackingStream,
  uint64_t                   u64ContentStart,
  uint64_t                   u64ContentSize,
  uint64_t                   u64BlockSize) {
  return std::shared_ptr<BlockBasedProtectedStream>(
    new BlockBasedProtectedStream(pCryptoProvider,
                                  pBackingStream,
                                  u64ContentStart,
                                  u64ContentSize,
                                  u64BlockSize));
}

BlockBasedProtectedStream::BlockBasedProtectedStream(
  shared_ptr<ICryptoProvider>pCryptoProvider,
  shared_ptr<IStream>        pBackingStream,
  uint64_t                   u64ContentStart,
  uint64_t                   u64ContentSize,
  uint64_t                   u64BlockSize)
  : m_locker(new mutex)
  , m_u64Position(0)
  , m_bIsPositionValid(true)
  , m_bSizeChangeRequested(false)
  , m_u64NewSize(0)
  , m_bIsPlainText(pCryptoProvider == nullptr)
{
  m_pSimple.reset(new SimpleProtectedStream(pCryptoProvider, pBackingStream,
                                            u64ContentStart, u64ContentSize));
  m_pCachedBlock.reset(new CachedBlock(m_pSimple, u64BlockSize));
}

BlockBasedProtectedStream::BlockBasedProtectedStream(
  const BlockBasedProtectedStream& rhs)
  : enable_shared_from_this<BlockBasedProtectedStream>(rhs)
  , m_locker(new mutex)
  , m_u64Position(0)
  , m_bIsPositionValid(true)
  , m_bSizeChangeRequested(false)
  , m_u64NewSize(0)
  , m_bIsPlainText(rhs.m_bIsPlainText)
{
  m_pSimple = dynamic_pointer_cast<SimpleProtectedStream>(rhs.m_pSimple->Clone());

  if (m_pSimple.get() == nullptr) {
    throw exceptions::RMSCryptoNullPointerException("Failed to clone stream");
  }
  m_pCachedBlock.reset(new CachedBlock(m_pSimple,
                                       rhs.m_pCachedBlock->GetBlockSize()));
}

shared_future<int64_t>BlockBasedProtectedStream::ReadAsync(uint8_t      *pbBuffer,
                                                           const int64_t cbBuffer,
                                                           const int64_t cbOffset,
                                                           bool          fCreateBackingThread)
{
  if (cbBuffer > 0)
  {
    if (pbBuffer == nullptr) {
      throw exceptions::RMSCryptoInvalidArgumentException("Invalid argument");
    }
  }

  if (!CanRead()) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid operation");
  }

  // lock resources
  unique_lock<mutex> lock(*m_locker);

  if (m_bIsPlainText)
  {
    return m_pSimple->ReadAsync(pbBuffer, cbBuffer, cbOffset,
                                fCreateBackingThread);
  }

  if (!m_bIsPositionValid) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid operation");
  }


  auto selfPtr = this->shared_from_this();

  return async(fCreateBackingThread ? launch::async : launch::deferred,
               [](shared_ptr<BlockBasedProtectedStream>self,
                  uint8_t      *buffer,
                  int64_t       bSize,
                  const int64_t offset) -> int64_t
      {
        // lock resources
        unique_lock<mutex>lock(*self->m_locker);

        // seek to offset
        self->SeekInternal(offset);

        if (self->m_bSizeChangeRequested)
        {
          self->ProcessSizeChangeRequest();
        }

        int64_t u64Size = bSize;

        while (u64Size > 0 && self->m_u64Position < self->SizeInner())
        {
          self->m_pCachedBlock->UpdateBlock(self->m_u64Position);

          uint64_t u64Read = self->m_pCachedBlock->ReadFromBlock(buffer,
                                                                 self->
                                                                 m_u64Position,
                                                                 u64Size);

          if (0 == u64Read)
          {
            // nothing to read anymore
            break;
          }

          // advance the buffer pointer
          buffer += u64Read;

          // advance the current position
          self->m_u64Position += u64Read;

          // decrease the number of uint8_ts to be read
          u64Size -= u64Read;
        }

        return static_cast<uint64_t>((bSize) - u64Size);
      }, move(selfPtr), pbBuffer, cbBuffer, cbOffset);
}

shared_future<int64_t>BlockBasedProtectedStream::WriteAsync(
  const uint8_t *cpbBuffer,
  const int64_t  cbBuffer,
  const int64_t  cbOffset,
  bool           fCreateBackingThread)
{
  try {
    return WriteInternalAsync(cpbBuffer,
                              cbBuffer,
                              cbOffset,
                              fCreateBackingThread,
                              true);
  }
  catch (exception& e)
  {
    m_locker->unlock();
    throw e;
  }
}

shared_future<int64_t>BlockBasedProtectedStream::WriteInternalAsync(
  const uint8_t *cpbBuffer,
  const int64_t  cbBuffer,
  const int64_t  cbOffset,
  bool           fCreateBackingThread,
  bool           fLockResources)
{
  if (cbBuffer > 0)
  {
    if (cpbBuffer == nullptr) {
      throw exceptions::RMSCryptoInvalidArgumentException("Invalid argument");
    }
  }

  if ((fLockResources && !CanWrite()) || (!fLockResources && !CanWriteInner())) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid operation");
  }

  if (m_bIsPlainText)
  {
    // lock resources
    if (fLockResources) m_locker->lock();
    auto ret = m_pSimple->WriteAsync(cpbBuffer,
                                     cbBuffer,
                                     cbOffset,
                                     fCreateBackingThread);

    if (fLockResources) m_locker->unlock();

    return ret;
  }

  auto selfPtr = this->shared_from_this();

  return async(fCreateBackingThread ? launch::async : launch::deferred,
               [](shared_ptr<BlockBasedProtectedStream>self,
                  const uint8_t *buffer,
                  int64_t bSize,
                  const int64_t offset,
                  bool          fNeedLock) -> int64_t
      {
        // lock resources
        if (fNeedLock) self->m_locker->lock();

        if (!self->m_bIsPositionValid) {
          throw exceptions::RMSCryptoInvalidArgumentException("Invalid operation");
        }

        if (self->m_bSizeChangeRequested)
        {
          self->ProcessSizeChangeRequest();
        }

        // seek to write
        self->SeekInternal(offset);

        // the number of uint8_ts to write
        uint64_t sizeRemaining = bSize;

        while (sizeRemaining > 0)
        {
          self->m_pCachedBlock->UpdateBlock(self->m_u64Position);

          uint64_t u64Written = self->m_pCachedBlock->WriteToBlock(
            buffer,
            self->m_u64Position,
            sizeRemaining);

          if (0 == u64Written)
          {
            // nothing to write
            break;
          }

          // advance the buffer pointer
          buffer += u64Written;

          // advance the current position
          self->m_u64Position += u64Written;

          // decrease the number to be written
          sizeRemaining -= u64Written;
        }

        if (fNeedLock) self->m_locker->unlock();

        // return total number of written
        return static_cast<int64_t>(bSize - sizeRemaining);
      }, move(selfPtr), cpbBuffer, cbBuffer, cbOffset, fLockResources);
}

future<bool>BlockBasedProtectedStream::FlushAsync(bool fCreateBackingThread)
{
  if (m_bIsPlainText)
  {
    // lock resources
    unique_lock<mutex> lock(*m_locker);
    return m_pSimple->FlushAsync(fCreateBackingThread);
  }

  auto selfPtr = this->shared_from_this();

  return async(fCreateBackingThread ? launch::async : launch::deferred,
               [](shared_ptr<BlockBasedProtectedStream>self) -> bool
      {
        // lock resources
        unique_lock<mutex>lock(*self->m_locker);

        if (self->m_bSizeChangeRequested)
        {
          self->ProcessSizeChangeRequest();
        }

        return self->m_pCachedBlock->Flush();
      }, move(selfPtr));
}

int64_t BlockBasedProtectedStream::Read(uint8_t      *pbBuffer,
                                        const int64_t cbBuffer) {
  return ReadAsync(pbBuffer, cbBuffer, Position(), false).get();
}

int64_t BlockBasedProtectedStream::Write(const uint8_t *cpbBuffer,
                                         const int64_t  cbBuffer) {
  return WriteAsync(cpbBuffer, cbBuffer, Position(), false).get();
}

bool BlockBasedProtectedStream::Flush() {
  return FlushAsync(false).get();
}

shared_ptr<IStream>BlockBasedProtectedStream::Clone()
{
  return static_pointer_cast<IStream>(
    shared_ptr<BlockBasedProtectedStream>(new BlockBasedProtectedStream(*this)));
}

void BlockBasedProtectedStream::Seek(uint64_t u64Position)
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);
  SeekInternal(u64Position);
}

bool BlockBasedProtectedStream::CanRead()
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return m_pSimple->CanRead();
}

bool BlockBasedProtectedStream::CanWrite()
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);

  return CanWriteInner();
}

bool BlockBasedProtectedStream::CanWriteInner()
{
  return m_pSimple->CanWrite();
}

uint64_t BlockBasedProtectedStream::Position()
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);
  return PositionInner();
}

uint64_t BlockBasedProtectedStream::Size()
{
  // lock resources
  unique_lock<mutex> lock(*m_locker);
  return SizeInner();
}

void BlockBasedProtectedStream::SeekInternal(uint64_t u64Position) {
  if (m_bIsPlainText)
  {
    m_pSimple->Seek(u64Position);
    return;
  }

  if (u64Position > SizeInner())
  {
    if (CanWriteInner())
    {
      SizeInner(u64Position);
    }
    else
    {
      m_bIsPositionValid = false;
      return;
    }
  }

  m_bIsPositionValid = true;
  m_u64Position      = u64Position;
}

uint64_t BlockBasedProtectedStream::PositionInner() {
  if (m_bIsPlainText)
  {
    return m_pSimple->Position();
  }

  return m_u64Position;
}

uint64_t BlockBasedProtectedStream::SizeInner()
{
  if (m_bIsPlainText)
  {
    return m_pSimple->Size();
  }

  if (m_bSizeChangeRequested)
  {
    return m_u64NewSize;
  }

  return m_pCachedBlock->GetSizeInternal();
}

void BlockBasedProtectedStream::SizeInner(uint64_t value) {
  if (SizeInner() != value)
  {
    if (m_bIsPlainText)
    {
      m_pSimple->Size(value);
      return;
    }

    m_bSizeChangeRequested = true;
    m_u64NewSize           = value;
  }
}

void BlockBasedProtectedStream::Size(uint64_t value) {
  if (!CanWrite()) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid operation");
  }

  // lock resources
  unique_lock<mutex> lock(*m_locker);
  SizeInner(value);
}

void BlockBasedProtectedStream::ProcessSizeChangeRequest()
{
  // Set this to false before writing, otherwise we will fall into an infinite
  // recursion
  // when doing the actual writing.
  m_bSizeChangeRequested = false;

  uint64_t oldSize = m_pCachedBlock->GetSizeInternal();

  // we need to remember the original position, because we are going to
  // write to the stream, which will modify the current position.
  uint64_t originalPosition = PositionInner();

  if (m_u64NewSize > oldSize)
  {
    FillWithZeros(m_u64NewSize);
  }
  else if (m_u64NewSize < oldSize)
  {
    m_pCachedBlock->RewriteFinalBlock(m_u64NewSize);
    SizeInternal(m_u64NewSize);
  }

  // Seek to the original position, if the original position is within the new
  // Size.
  // Otherwise seek to the size.
  SeekInternal(min(originalPosition, SizeInner()));
}

void BlockBasedProtectedStream::FillWithZeros(uint64_t newSize)
{
  const uint64_t bufferSize = m_pCachedBlock->GetBlockSize();

  vector<uint8_t> zeros((unsigned int)bufferSize, 0x0);

  SeekInternal(m_pCachedBlock->GetSizeInternal());

  // Note that we might end-up writing more zeros than newSize - oldSize,
  // because of the CBC padding.
  while (newSize > m_pCachedBlock->GetSizeInternal())
  {
    uint64_t toWrite =
      min(newSize - m_pCachedBlock->GetSizeInternal(), bufferSize);

    WriteInternalAsync(zeros.data(), toWrite, PositionInner(), false,
                       false).get();
  }

  SizeInternal(newSize);
}

void BlockBasedProtectedStream::SizeInternal(uint64_t size)
{
  m_pSimple->Size(size);

  // Adjust the position if needed
  if (m_u64Position > size)
  {
    m_u64Position = size - 1;
  }

  // update the cached block if needed
  m_pCachedBlock->SizeInternal(size);
}

BlockBasedProtectedStream::~BlockBasedProtectedStream()
{}
} // namespace api
} // namespace rmscrypto
