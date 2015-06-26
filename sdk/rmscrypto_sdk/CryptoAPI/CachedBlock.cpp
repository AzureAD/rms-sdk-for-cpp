/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#include <cstring>
#include "SimpleProtectedStream.h"
#include "CachedBlock.h"
#include "RMSCryptoExceptions.h"

using namespace std;
namespace rmscrypto {
namespace api {
CachedBlock::CachedBlock(shared_ptr<SimpleProtectedStream>pSimple,
                         uint64_t                         u64BlockSize)
  : m_pSimple(pSimple)
  , m_u64BlockSize(u64BlockSize)
  , m_u64CacheStart(numeric_limits<uint64_t>::max())
  , m_u64CacheSize(0)
  , m_cache(static_cast<size_t>(u64BlockSize))
  , m_bFinalBlockHasBeenWritten(false)
  , m_bWritePending(false)
{}

uint64_t CachedBlock::GetBlockSize()
{
  return m_u64BlockSize;
}

void CachedBlock::UpdateBlock(uint64_t u64Position)
{
  uint32_t u32BlockNumber = CalculateBlockNumber(u64Position);

  if ((numeric_limits<uint64_t>::max() != m_u64CacheStart) &&
      (CalculateBlockNumber(m_u64CacheStart) == u32BlockNumber))
  {
    // the current cache is up-to-date no need to overwrite it
    return;
  }

  if (m_bWritePending)
  {
    // we're going to overwrite the current cached block, so we need to write
    // the current block to the backing stream

    // determine if this is the final block
    bool bCurrentBlockIsLastBlock =
      (m_u64CacheStart + m_u64BlockSize >= m_pSimple->Size());
    bool bCurrentBlockIsFinal = false;

    if (bCurrentBlockIsLastBlock)
    {
      if (u64Position < m_u64CacheStart + m_u64BlockSize)
      {
        // This is the last block, and the new position is not past this block,
        // so we don't expect any more data coming and this block should be
        // treated as final.
        bCurrentBlockIsFinal = true;
      }
    }

    // Write the block
    m_pSimple->WriteInternalAsync(m_cache.data(),
                                  m_u64CacheSize,
                                  m_u64CacheStart,
                                  std::launch::deferred,
                                  CalculateBlockNumber(m_u64CacheStart),
                                  bCurrentBlockIsFinal).get();

    if (bCurrentBlockIsFinal)
    {
      m_bFinalBlockHasBeenWritten = true;
    }

    m_bWritePending = false;
  }

  // calculate the start of the block
  m_u64CacheStart = static_cast<uint64_t>(u32BlockNumber) * m_u64BlockSize;

  // determine if this is the final block
  bool bNewBlockIsFinal =
    (m_u64CacheStart + m_u64BlockSize >= m_pSimple->Size());

  // go to the start of the block and read
  m_u64CacheSize = m_pSimple->ReadInternalAsync(&m_cache[0],
                                                m_u64BlockSize,
                                                m_u64CacheStart,
                                                std::launch::deferred,
                                                u32BlockNumber,
                                                bNewBlockIsFinal).get();
}

uint64_t CachedBlock::ReadFromBlock(uint8_t *pbBuffer,
                                    uint64_t u64Position,
                                    uint64_t u64Size)
{
  // if the position is in the cache range (i.e. [cacheStart, cacheStart +
  // cacheSize))
  if ((m_u64CacheStart <= u64Position) &&
      (u64Position < m_u64CacheStart + m_u64CacheSize))
  {
    // calculate the position in the cache where we need to read from
    uint64_t u64PositionInCache = u64Position - m_u64CacheStart;

    // calculate the number of uint8_ts available to read
    uint64_t u64ToRead =
      min(u64Size, m_u64CacheSize - u64PositionInCache);

    // copy the data
    memcpy(pbBuffer, &m_cache[u64PositionInCache], u64ToRead);

    return u64ToRead;
  }
  else
  {
    // nothing in cache to read
    return 0;
  }
}

uint64_t CachedBlock::WriteToBlock(
  const uint8_t *pbBuffer,
  uint64_t       u64Position,
  uint64_t       u64Size)
{
  // if the position is in the cache range (i.e. [cacheStart, cacheStart +
  // blockSize))
  if ((m_u64CacheStart <= u64Position) &&
      (u64Position < m_u64CacheStart + m_u64BlockSize))
  {
    // calculate the position in the cache where we need to write to
    uint64_t u64PositionInCache = u64Position - m_u64CacheStart;

    // calculate the number of uint8_ts we can write to the current block
    uint64_t u64ToWrite =
      min(u64Size, m_u64BlockSize - u64PositionInCache);

    if (u64ToWrite > 0)
    {
      m_bWritePending = true;
    }

    // write the data
    memcpy(&m_cache[u64PositionInCache], pbBuffer, u64ToWrite);

    // update cache size
    m_u64CacheSize = max(m_u64CacheSize, u64PositionInCache + u64ToWrite);

    // determine if this is the final block
    bool bCurrentBlockIsFinal =
      (m_u64CacheStart + m_u64BlockSize >= m_pSimple->Size());

    if (bCurrentBlockIsFinal)
    {
      m_bFinalBlockHasBeenWritten = false;
    }

    return u64ToWrite;
  }
  else
  {
    // nothing to write to this cached position
    return 0;
  }
}

uint32_t CachedBlock::CalculateBlockNumber(uint64_t u64Position) const
{
  uint64_t u64BlockNumber = u64Position / m_u64BlockSize;

  if (m_bFinalBlockHasBeenWritten &&
      (u64BlockNumber * m_u64BlockSize == GetSizeInternal()) &&
      (u64Position != GetSizeInternal()))
  {
    // This is the case when the (encrypted final block size) == (block size)
    // and the u64Position is in the final block but not pointing to the end of
    // the block.
    // This means that there is still space left in the final block (the CBC
    // padding).
    if (u64BlockNumber > 0)
    {
      --u64BlockNumber;
    }
  }

  return static_cast<uint32_t>(u64BlockNumber);
}

void CachedBlock::RewriteFinalBlock(uint64_t newSize)
{
  // shrinking the backing stream
  if (newSize != 0)
  {
    // The new size can be somewhere in the middle of the block. If we delete
    // everything after new size
    // we won't be able to decrypt it and finalize it. The idea is
    //      1. to read the last uint8_t (which will cause decrypting the block
    // into
    // the cache),
    //      2. to set the new size (to get rid of everything after the new size
    // in the cache)
    //      3. and finally to write the last uint8_t back (which will make it
    // reencrypt the cached block, i.e., will set the _isEncryptPending flag)

    // Read the last byte as if the size has never been changed
    UpdateBlock(newSize - 1);

    uint8_t byte;

    /*uint64_t bytesRead = */ ReadFromBlock(&byte, newSize - 1, 1);

    // There is no final block anymore
    m_bFinalBlockHasBeenWritten = false;

    // Now write the uint8_t
    WriteToBlock(&byte, newSize - 1, 1);
  }
}

bool CachedBlock::Flush()
{
  uint64_t cacheStart = m_u64CacheStart;

  if (m_u64CacheStart == numeric_limits<uint64_t>::max())
  {
    // nothing written to cache, still need to flush the padding
    cacheStart = 0;
  }

  // determine if this is the final block
  bool bIsFinal = (cacheStart + m_u64BlockSize >= m_pSimple->Size());

  m_pSimple->WriteInternalAsync(m_cache.data(), m_u64CacheSize, cacheStart, std::launch::deferred,
                                CalculateBlockNumber(cacheStart), bIsFinal).get();

  if (bIsFinal)
  {
    m_bFinalBlockHasBeenWritten = true;
  }

  m_bWritePending = false;

  return m_pSimple->Flush();
}

uint64_t CachedBlock::GetSizeInternal() const
{
  // if we're pending write somewhere in the middle, then the backing stream
  // size is still valid
  // but if we're pending write on the final block, then we need to calculate
  // the size from our own cache
  if (m_bWritePending && !m_bFinalBlockHasBeenWritten)
  {
    return (CalculateBlockNumber(m_u64CacheStart) *
            static_cast<uint64_t>(m_u64BlockSize)) +
           static_cast<uint64_t>(m_u64CacheSize);
  }

  return m_pSimple->Size();
}

void CachedBlock::SizeInternal(uint64_t u64Size)
{
  // Make sure that the current cached block doesn't go beyond the new size
  uint64_t u64BlockPosition = CalculateBlockNumber(m_u64CacheStart) *
                              static_cast<uint64_t>(m_u64BlockSize);

  // This method should only be called after the cache has been updated.
  // Validate that the current block is the correct block in the cache (not
  // beyond the new size)
  if (u64BlockPosition > u64Size) {
    throw exceptions::RMSCryptoInvalidArgumentException("Invalid argument");
  }

  m_u64CacheSize = min(m_u64CacheSize, u64Size - u64BlockPosition);
}
} // namespace api
} // namespace rmscrypto
